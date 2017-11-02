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
	std::vector<uint> index;

	uint buffNormals = 0;
	uint buffUv = 0;
	uint buffVertex = 0;
	uint buffIndex = 0;
	uint numIndex = 0;
	uint numVertex = 0;


	AABB aabbBox;

	void RealUpdate();

	void DrawUI();
	void DrawAABB(AABB aabbBox);
	bool aabbTransform = false;

private:
	bool aabbActive = false;


};