#include "Globals.h"
#include "Component.h"
#include <vector>
#include <map>



class GameObject 
{
public:
	GameObject(std::string name, bool active, GameObject* parent);
	std::string name;
	GameObject* parent;
	std::vector<GameObject*> sons;
	bool active = true;
	bool collapsedHierarchy = false;

	std::map<componentType, Component*> components;

	void Update();
	Component* FindComponent(componentType);
	void DrawUI() {
		if (ImGui::CollapsingHeader(name.data())) //AMB EL NOM DEL GAME
		{
			for (auto itComp : components)
			{
				itComp.second->DrawUI();
			}
		}
	}

	Component* AddComponent(Component*);
	//FUNCIO ACTIVAR I DESACTIVAR

private:

};