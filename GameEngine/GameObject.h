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
	
	std::map<componentType, Component*> components;

	void PreUpdate();
	void Update();
	Component* FindComponent(componentType);

	void DrawUI();
	void UpdateAABB(float4x4 matrix);

	void DrawHeriarchy(GameObject* son);
	void DrawProperties();
	bool SonHasMesh();

	void AddComponent(Component*);

	void Enable();
	void Disable();

std::string name;
	GameObject* parent;
	std::vector<GameObject*> sons;
	bool active = true;
	bool collapsedHierarchy = false;
	bool clicked = false;
	bool insideFrustum = false;
	AABB aabbBox;

private:

};

#endif // ! __GAMEOBJECT__