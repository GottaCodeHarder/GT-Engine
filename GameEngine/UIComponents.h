#include "Component.h"

class GameObject;

class cImage : public Component
{
public:
	cImage(GameObject* _gameObject);
	~cImage();

	void RealUpdate();
	void DrawUI();

	void Save(JSON_Object &object) const;
	void Load(const JSON_Object &object);
	uint Serialize(char* buffer);

public:
	GTInterface::Image image;
};




