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
	void IsPlaying(float dt);
	Component* FindComponent(componentType);

	void DrawUI();
	void UpdateAABB(float4x4 matrix);

	void DrawHeriarchy(GameObject* son);
	void DrawProperties();
	bool SonHasMesh();

	void AddComponent(Component*);

	void Save(JSON_Object *go) const;
	void Load(const JSON_Object *go);

	// Returns the length of the buffer
	uint Serialize(char* &buffer);

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
	bool statiC = false;
	bool isInsideQuad = false;

private:


template <typename var>
void Cpy(var toCopy, char*& it)
{
	uint bytes = szieof(var);
	memcpy(it, &toCopy, bytes);
	it += bytes;
}
// How to declare for uint: Cpy<uint>(sons.size(), it);

};

#endif // ! __GAMEOBJECT__