#ifndef  __GAMEOBJECT__
#define __GAMEOBJECT__

#include "Globals.h"
#include "Component.h"
#include <map>
#include "MathGeoLib/MathGeoLib.h"

class GameObject 
{
public:
	GameObject(std::string name, bool active, GameObject* parent);
	//FALTA DESTRUCTOR QUE DESTRUEIXI ELS FILLS
	std::string name;
	GameObject* parent;
	std::vector<GameObject*> sons;
	bool active = true;
	bool collapsedHierarchy = false;
	bool clicked = false;

	AABB aabbBox;

	std::map<componentType, Component*> components;

	void Update();
	Component* FindComponent(componentType);

	void DrawUI();

	void DrawHeriarchy(GameObject* son);
	void DrawProperties();

	void AddComponent(Component*);
	//FUNCIO ACTIVAR I DESACTIVAR

	void Enable();
	void Disable();


private:

};

#endif // ! __GAMEOBJECT__