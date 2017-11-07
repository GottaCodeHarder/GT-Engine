#include "GameObject.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "cTransform.h"
#include "cMesh.h"
#include "cCamera.h"
#include "MathGeoLib/MathGeoLib.h"
#include "JSON/parson.h"

#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

class cTransform;
GameObject::GameObject(std::string _name, bool _active, GameObject * _parent) : name(_name) , active(_active) , parent(_parent)
{
	aabbBox.SetNegativeInfinity();
}

void GameObject::PreUpdate()
{
	if (active)
	{
		for (auto itSons : sons)
		{
			itSons->PreUpdate();
		}
		insideFrustum = false;
	}
}

void GameObject::Update()
{
	if (active)
	{
		for (auto comp : components)
		{
			comp.second->Update();
		}
		
		//Multiply all the matrixTransform with their sons
		glPushMatrix();

		glMultMatrixf(((cTransform*)FindComponent(TRANSFORM))->GetLocalMatrixTransf().Transposed().ptr());

		//Update AABB BOX for Game Objects
		if (((cTransform*)FindComponent(TRANSFORM))->transformChange)
		{
			float4x4 matrix = ((cTransform*)FindComponent(TRANSFORM))->GetGlobalMatrixTransf();
			float4x4 matrix1 = ((cTransform*)FindComponent(TRANSFORM))->GetLocalMatrixTransf();

			//if has a mesh it is modified
			if (SonHasMesh())
			{
				UpdateAABB(matrix);
			}
			//IF has frustum it is modified
			if (((cCamera*)FindComponent(CAMERA)) != nullptr)
			{
				((cCamera*)FindComponent(CAMERA))->transformFrustum = true;
			}

			((cTransform*)FindComponent(TRANSFORM))->transformChange = false;
		}

		if (!sons.empty())

		{
			for (auto itSons : sons)
			{
				itSons->Update();
			}
		}

		if (App->renderer3D->bEnableWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if(insideFrustum)
			App->renderer3D->DrawGameObject(this);

		if (!isInsideQuad && statiC)
		{
			//QUADTREE
			App->scene->quad.AddGameObject(this);
			isInsideQuad = true;
		}

		glPopMatrix();

		if (App->isPlaying)
		{
			IsPlaying(App->GetGameDt());
		}


	}
}

void GameObject::IsPlaying(float dt)
{
}

Component * GameObject::FindComponent(componentType type)
{
	Component * ret = nullptr;

	std::map<componentType, Component*>::iterator it = components.find(type);
	if (it != components.end())
	{
		return it->second;
	}
	return nullptr;
}

void GameObject::DrawUI()
{
	if (ImGui::CollapsingHeader(name.data()))
	{
		for (auto itComp : components)
		{
			itComp.second->DrawUI();
		}
	}
}

void GameObject::UpdateAABB(float4x4 matrix)
{
	for (auto sonsGO : sons)
	{
		sonsGO->UpdateAABB(matrix);
	}
	if (FindComponent(MESH) != nullptr)
	{
		aabbBox.SetNegativeInfinity();
		OBB obb = ((cMesh*)FindComponent(MESH))->aabbBox.Transform(matrix);

		aabbBox.Enclose(obb);
	}
}

void GameObject::DrawHeriarchy(GameObject* son)
{

	//static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
	int node_clicked = -1;
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	//| ImGuiTreeNodeFlags_Selected;
	bool node_open;
	if (node_open = ImGui::TreeNodeEx(name.data(), node_flags))
	{
		if (ImGui::IsItemClicked())
		{
			node_clicked = 7;
			this->clicked = true;
			if (App->editor->selected != nullptr)
			{
			App->editor->selected->clicked = false;
			}
			App->editor->selected = this;
		}

		if (node_open)
		{
			for (auto sonsSons : son->sons)
			{
				sonsSons->DrawHeriarchy(sonsSons);
			}
		}
	

		//if (clicked)
		//{
		//	// Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
		//	if (ImGui::GetIO().KeyCtrl)
		//		selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
		//	else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
		//		selection_mask = (1 << node_clicked);           // Click to single-select
		//}
		ImGui::TreePop();
	}
	else
	{
		if (ImGui::IsItemClicked())
		{
			node_clicked = 7;
			this->clicked = true;
			if (App->editor->selected != nullptr)
			{
				App->editor->selected->clicked = false;
			}
			App->editor->selected = this;
		}
	}


}

void GameObject::DrawProperties()
{
	
	ImGui::Checkbox("Static", &statiC);
	ImGui::SameLine();
	if (!ImGui::Checkbox("Enable/Disable", &active))
	{
		if (active)
		{
			for (auto itComponents : components)
			{
				itComponents.second->DrawUI();
			}
		}
	}

}

bool GameObject::SonHasMesh()
{
	bool ret = false;
	if (FindComponent(MESH) != nullptr)
	{
		ret = true;
	}
	for (auto sonsGO : sons)
	{
		ret = sonsGO->SonHasMesh();
		if (ret)
		{
			break;
		}
	}
	return ret;
}

void GameObject::AddComponent(Component* addComponent)
{
	components.insert(std::pair<componentType, Component*>(addComponent->type, addComponent));
}

void GameObject::Save(JSON_Object *go) const
{
	json_object_set_string(go, "Name", name.data());
	json_object_set_boolean(go, "Active", active);

	JSON_Value* value = json_value_init_array();
	JSON_Array* compArray = json_value_get_array(value);

	for (auto it : components)
	{
		//it.second->Save();
	}

	for (auto child : sons)
	{
		//child->Save();
	}

	json_object_set_value(go, "Components", value);
}

void GameObject::Load(const JSON_Object * go)
{

}

uint GameObject::Serialize(char * buf)
{
	uint length = 0;
	length += sizeof(uint); // Size of Name Length variable
 	length += name.length(); // Size of the Name String
	length += sizeof(uint); // Size of Components
	length += sizeof(uint); // Size of Childs


	buf = new char[length]; // Actual size of 8
	char* it = buf;

	memcpy(it, &length, sizeof(uint));
	it += sizeof(uint);

	memcpy(it, name.data(), name.length());
	it += name.length();

	// Saving Components
	uint size = components.size();
	memcpy(it, &size, sizeof(uint));
	it += sizeof(uint);

	// Saving Childs
	size = sons.size();
	memcpy(it, &size, sizeof(uint));
	it += sizeof(uint);

	for (auto it : components)
	{
		//it.first
		//it.second
	}

	for (std::vector<GameObject*>::iterator child = sons.begin(); child != sons.end(); child++)
	{
		uint childLength = (*child)->Serialize(it);
		length += childLength;
		it += childLength;
	}

	return length;
}

void GameObject::Enable()
{
	active =true ;
}

void GameObject::Disable()
{
	active = false;
}