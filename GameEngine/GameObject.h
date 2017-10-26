#include "Globals.h"
#include "Component.h"
#include <vector>
#include <map>
#include "MathGeoLib/MathGeoLib.h"

class GameObject 
{
public:
	GameObject(std::string name, bool active, GameObject* parent);
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