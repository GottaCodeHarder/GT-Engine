#ifndef __RESOURCEMESH__
#define __RESOURCEMESH__

#include "Resources.h"
#include "MathGeoLib/MathGeoLib.h"

class ResourceMesh : public Resources
{
public:
	componentType GetType()
	{
		return MESH;
	}

	void Destroy()
	{
		glDeleteBuffers(1, &buffNormals);
		glDeleteBuffers(1, &buffIndex);
		glDeleteBuffers(1, &buffVertex);
		glDeleteBuffers(1, &buffUv);
	}

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

};

#endif // !__RESOURCES__
