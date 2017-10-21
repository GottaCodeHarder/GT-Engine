#ifndef __Component__
#define __Component__

#include "Globals.h"

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

	componentType type;
	bool active = true;
	GameObject* gameObject = nullptr;

private:

	virtual void RealUpdate() {};
};
#endif // !