#include "Globals.h"
#include "MathGeoLib/MathGeoLib.h"

#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

class Mesh
{
public:

	~Mesh();

	std::vector<float3> normals;
	std::vector<float3> vertex;

	uint buffNormals = 0;
	uint buffUv = 0;
	uint buffVertex = 0;
	uint buffIndex = 0;
	uint numIndex = 0;
	GLuint buffTexture = 0;

	math::AABB aabbBox;
private:

};


class Importer
{

public:
	bool FileExists(const std::string& name);
	GLuint LoadImage(const char* file);

	std::vector<Mesh*> CreateMesh(const char* path);

private:
	bool bDevilInit = false;
};