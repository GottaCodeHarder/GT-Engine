#include "GameObject.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

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

		if (!sons.empty())
		{
			for (auto itSons : sons)
			{
				itSons->Update();
			}
		}
	}
	App->renderer3D->DrawGameObject(this);
}

Component * GameObject::FindComponent(componentType type)
{
	return components[type];
}

Component * GameObject::AddComponent(Component* addComponent)
{
	components.insert(std::pair<componentType, Component*>(addComponent->type, addComponent));
}
