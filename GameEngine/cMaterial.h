#include "Component.h"
#include "MathGeoLib/MathGeoLib.h"

class ResourceTexture;
class GameObject;

enum class TransparencyType
{
	MT_OPAQUE,
	MT_ALPHA_TEST,
	MT_BLEND
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

	TransparencyType type = TransparencyType::MT_OPAQUE;
	float alpha = 0.3f;

private:
};