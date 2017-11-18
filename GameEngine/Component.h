#ifndef __Component__
#define __Component__

#include "Globals.h"
#include "JSON/parson.h"

enum componentType
{
	MESH = 0,
	TRANSFORM,
	MATERIAL,
	CAMERA
};

class GameObject;

class Component
{
public:
	Component(componentType _type, GameObject* _gameObject) : type(_type), gameObject(_gameObject){}

	virtual void Enable() {};

	void Update()
	{
		if (active)
		{
			RealUpdate();
		}
	}
	
	virtual void Disable() {};

	virtual void DrawUI() {};

	virtual void Save(JSON_Object &object) const {};
	virtual void Load(const JSON_Object &object) {};
	
	virtual uint Serialize(char* &buffer) { return 0; };
	virtual void DeSerialize(char* &buffer, GameObject* parent) {};

	componentType type;
	bool active = true;
	GameObject* gameObject = nullptr;

private:

	virtual void RealUpdate() {};
};
#endif // !