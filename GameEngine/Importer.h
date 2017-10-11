#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "Globals.h"
#include "MathGeoLib/MathGeoLib.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

class Mesh
{
public:

	std::vector<float3> normals;
	std::vector<float3> vertex;

	uint buff_normals = 0;
	uint buff_uv = 0;
	uint colors = 0;
	uint buff_vertex = 0;
	uint buff_index = 0;
	uint num_index = 0;
	uint buff_texture = 0;

	math::AABB aabbBox;
	float vec3MinPoint[3] = { 0.0f, 0.0f, 0.0f };
	float vec3MaxPoint[3] = { 0.0f, 0.0f, 0.0f };
private:

};


class Importer
{

public:

	std::vector<Mesh*> CreateMesh(char* path);

};