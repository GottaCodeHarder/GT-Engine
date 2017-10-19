#include "GameObject.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "cTransform.h"

#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

class cTransform;
GameObject::GameObject(std::string _name, bool _active, GameObject * _parent) : name(_name) , active(_active) , parent(_parent)
{
	
}

void GameObject::Update()
{
	if (active)
	{
		for (auto comp : components)
		{
			comp.second->Update();
		}
		
		GLfloat matrix[16];
		glPushMatrix();
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		float4x4 tmp = ((cTransform*)FindComponent(TRANSFORM))->GetMatrixTransf();
		float4x4 tmp1 = ((cTransform*)FindComponent(TRANSFORM))->GetMatrixTransf().Transposed();
		float4x4 tmp2 = float4x4::identity;
		glMultMatrixf(((cTransform*)FindComponent(TRANSFORM))->GetMatrixTransf().ptr());
		//glMultMatrixf(tmp2.ptr());
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	//	GLfloat matrix[16];
	//	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	//	matrix;
		if (!sons.empty())
		{
			for (auto itSons : sons)
			{
				itSons->Update();
			}
		}

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

void GameObject::DrawHeriarchy(GameObject* son)
{
	if (ImGui::TreeNodeEx(name.data(), ImGuiTreeNodeFlags_NoAutoOpenOnLog))
	{
		for (auto sonsSons : son->sons)
		{
			sonsSons->DrawHeriarchy(sonsSons);
		}
		ImGui::TreePop();
	}
}

void GameObject::DrawProperties()
{
	//ImGui::Text("This is GameObject Properties");
	for (auto itComponents : components)
	{
		itComponents.second->DrawUI();
	}
}

void GameObject::AddComponent(Component* addComponent)
{
	components.insert(std::pair<componentType, Component*>(addComponent->type, addComponent));
}
