#include "GameObject.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "cTransform.h"
#include "cMesh.h"
#include "cCamera.h"
#include "MathGeoLib/MathGeoLib.h"

#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

class cTransform;
GameObject::GameObject(std::string _name, bool _active, GameObject * _parent) : name(_name) , active(_active) , parent(_parent)
{
	aabbBox.SetNegativeInfinity();
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
			if (((cMesh*)FindComponent(MESH)) != nullptr)
			{
				aabbBox.SetNegativeInfinity();
				OBB obb = ((cMesh*)FindComponent(MESH))->aabbBox.Transform(matrix);

				aabbBox.Enclose(obb);
				App->scene->quad;

			}
			//IF has frustum it is modified
			if (((cCamera*)FindComponent(CAMERA)) != nullptr)
			{
				((cCamera*)FindComponent(CAMERA))->frustum.Transform(matrix1);
				//AABB aabbFrustumBox = ((cCamera*)FindComponent(CAMERA))->frustum.MinimalEnclosingAABB();
				//aabbFrustumBox.SetNegativeInfinity();
				//OBB obb = aabbFrustumBox.Transform(matrix);
				//aabbFrustumBox.Enclose(obb);
				//((cCamera*)FindComponent(CAMERA))->frustum.
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

		App->renderer3D->DrawGameObject(this);

		glPopMatrix();
	}
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

void GameObject::AddComponent(Component* addComponent)
{
	components.insert(std::pair<componentType, Component*>(addComponent->type, addComponent));
}

void GameObject::Enable()
{
	active =true ;
}

void GameObject::Disable()
{
	active = false;
}
