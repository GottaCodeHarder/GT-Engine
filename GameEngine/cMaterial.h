#include "Component.h"
#include "MathGeoLib/MathGeoLib.h"

class GameObject;
class cMaterial : public Component
{
public:
	cMaterial(GameObject* _gameObject);
	~cMaterial();

	float3 color;
	uint buffTexture = 0;
	std::string path;
	ImVec2 imageDimensions;

	void RealUpdate()
	{
		int i = 5;
	};

	void DrawUI();
	void LoadTexture();

private:
};