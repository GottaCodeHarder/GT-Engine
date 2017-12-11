#include "GameObject.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "cTransform.h"
#include "cMesh.h"
#include "cMaterial.h"
#include "ResourceMesh.h"
#include "cCamera.h"
#include "UIComponents.h"
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
		//Multiply all the matrixTransform with their sons
		glPushMatrix();

		glMultMatrixf(((cTransform*)FindComponent(TRANSFORM))->GetLocalMatrixTransf().Transposed().ptr());

		//Update AABB BOX for Game Objects
		if (((cTransform*)FindComponent(TRANSFORM))->transformChange)
		{
			float4x4 matrix = ((cTransform*)FindComponent(TRANSFORM))->GetGlobalMatrixTransf();
			float4x4 matrix1 = ((cTransform*)FindComponent(TRANSFORM))->GetLocalMatrixTransf();

			//if has a mesh it is modified
			if (SonHasMesh() || resizeAABB)
			{
				UpdateAABB(matrix);
				resizeAABB = false;
			}
			//IF has frustum it is modified
			if (((cCamera*)FindComponent(CAMERA)) != nullptr)
			{
				((cCamera*)FindComponent(CAMERA))->transformFrustum = true;
			}

			//IF has UI it is modified
			if (((cUI*)FindComponent(UI)) != nullptr)
			{
				((cUI*)FindComponent(UI))->transformChanged = true;
			}

			((cTransform*)FindComponent(TRANSFORM))->transformChange = false;
		}

		//UPDATE COMPONENTS
		for (auto comp : components)
		{
			comp.second->Update();
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
		OBB obb = ((cMesh*)FindComponent(MESH))->resource->aabbBox.Transform(matrix);

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

void GameObject::SetRectTransform(uint w, uint h)
{
	cTransform* transform = (cTransform*)FindComponent(componentType::TRANSFORM);
	if (transform)
		transform->SetRectTransform(w, h);
}

void GameObject::SetNormalTransform()
{
	cTransform* transform = (cTransform*)FindComponent(componentType::TRANSFORM);
	if (transform)
		transform->SetTransform();
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

uint GameObject::Serialize(char * &buf)
{
	uint length = 0; // Size of the GameObject
	uint myLength = 0;
	myLength += sizeof(uint); // Size of Name Length variable
	myLength += name.length(); // Size of the Name String
	myLength += sizeof(uint); // Size of Components
	myLength += sizeof(uint); // Size of Childs

	length = myLength;

	char* myself = new char[myLength]; // Actual size of 8
	char* it = myself;

	// Name Length
	uint size = name.length();
	memcpy(it, &size, sizeof(uint));
	it += sizeof(uint);

	// Name
	memcpy(it, name.data(), name.length());
	it += name.length();

	// Components Size
	size = components.size();
	memcpy(it, &size, sizeof(uint));
	it += sizeof(uint);

	// Childs Size
	size = sons.size();
	memcpy(it, &size, sizeof(uint));
	it += sizeof(uint);

	std::vector<std::pair<uint, char*>> toAdd;

	for (auto i : components)
	{
		std::pair<uint, char*> pair;
		pair.first = i.second->Serialize(pair.second);
		length += pair.first;
		toAdd.push_back(pair);
	}

	for (std::vector<GameObject*>::iterator child = sons.begin(); child != sons.end(); child++)
	{
		std::pair<uint, char*> pair;
		pair.first = (*child)->Serialize(pair.second);
		length += pair.first;
		toAdd.push_back(pair);
	}

	buf = new char[length];
	it = buf;

	memcpy(it, myself, myLength);
	it += myLength;

	for (std::vector<std::pair<uint, char*>>::iterator i = toAdd.begin(); i != toAdd.end(); i++)
	{
		memcpy(it, i->second, i->first);
		it += i->first;
		delete[] i->second;
	}

	delete[] myself;

	return length;
}

uint GameObject::DeSerialize(char * &buffer, GameObject * parent)
{
	uint ret = 0;
	uint size = 0;
	uint sizeChilds = 0;
	char* it = buffer;

	// Setting Parent
	if (parent == nullptr)
	{
		this->parent = App->scene->root;
		App->scene->root->sons.push_back(this);
	}
	else
	{
		this->parent = parent;
		parent->sons.push_back(this);
	}

	// Name Length
	memcpy(&size, it, sizeof(uint));
	it += sizeof(uint);
	ret += sizeof(uint);

	// Name
	name.assign(&it[0], size);
	it += size;
	ret += size;
	
	// Components Size
	memcpy(&size, it, sizeof(uint));
	it += sizeof(uint);
	ret += sizeof(uint);

	// Childs Size
	memcpy(&sizeChilds, it, sizeof(uint));
	it += sizeof(uint);
	ret += sizeof(uint);

	if (size > NULL)
	{
		uint tmp = size;

		while (tmp > NULL)
		{
			componentType type;

			// Component Type
			int iType;
			uint t;
			memcpy(&iType, it, sizeof(int));
			it += sizeof(int);
			ret += sizeof(int);

			type = (componentType)iType;

			switch (type)
			{
			case componentType::TRANSFORM:
			{
				//cTransform* c = (cTransform*)FindComponent(TRANSFORM);
				cTransform * c = new cTransform(this);
				this->AddComponent(c);
				t = (c->DeSerialize(it, this));
				it += t;
				ret += t;
				break;
			}
			case componentType::MATERIAL:
			{
				cMaterial * c = new cMaterial(this);
				this->AddComponent(c);
				t = (c->DeSerialize(it, this));
				it += t;
				ret += t;
				break;
			}
			case componentType::MESH:
			{
				cMesh * c = new cMesh(this);
				this->AddComponent(c);
				t = (c->DeSerialize(it, this));
				it += t;
				ret += t;
				break;
			}
			case componentType::CAMERA:
			{
				cCamera * c = new cCamera(this);
				this->AddComponent(c);
				t = (c->DeSerialize(it, this));
				it += t;
				ret += t;
				break;
			}
			default:
			{
				MYLOG("File was corrupted. Emergency exit, possible Scene bug.");
				return ret;
				break;
			}
			}

			tmp--;
		}
	}

	if (sizeChilds > NULL)
	{
		uint tmp = sizeChilds;
		while (tmp > NULL)
		{
			uint t = 0;
			GameObject* go = new GameObject("", true, this);
			t = go->DeSerialize(it, this);
			it += t;
			ret += t;

			tmp--;
		}
	}
	
	return ret;
}

void GameObject::Enable()
{
	active =true ;
}

void GameObject::Disable()
{
	active = false;
}