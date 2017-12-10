#include "Component.h"
#include "MathGeoLib/MathGeoLib.h"

class ResourceTexture;
class GameObject;

enum class TransparencyType
{
	OPAQUE,
	ALPHA_TEST,
	BLEND
};

class cMaterial : public Component
{
public:
	cMaterial(GameObject* _gameObject);
	~cMaterial();



	ResourceTexture* resource;

	void RealUpdate() {};

	void DrawUI();
	void LoadTexture();

	uint Serialize(char* &buffer);
	uint DeSerialize(char* &buffer, GameObject* parent);

	TransparencyType type = TransparencyType::OPAQUE;
	float alpha = 0.3f;

private:
};