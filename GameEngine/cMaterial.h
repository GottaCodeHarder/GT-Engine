#include "Component.h"
#include "MathGeoLib/MathGeoLib.h"

class ResourceTexture;
class GameObject;

class cMaterial : public Component
{
public:
	cMaterial(GameObject* _gameObject);
	~cMaterial();

	ResourceTexture* resource;

	void RealUpdate()
	{
		int i = 5;
	};

	void DrawUI();
	void LoadTexture();

	uint Serialize(char* &buffer);
	void DeSerialize(char* &buffer, GameObject* parent);

private:
};