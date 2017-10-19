#include "Component.h"
#include "MathGeoLib/MathGeoLib.h"

class GameObject;
class cMesh : public Component
{
public:
	cMesh(GameObject* _gameObject);
	~cMesh();

	std::vector<float3> normals;
	std::vector<float3> vertex;

	uint buffNormals = 0;
	uint buffUv = 0;
	uint buffVertex = 0;
	uint buffIndex = 0;
	uint numIndex = 0;
	uint buffTexture = 0;

	AABB aabbBox;

	void RealUpdate()
	{
		int i = 5;
	};

	void DrawUI();

private:
};