#ifndef __IMPORTER__
#define __IMPORTER__

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

class GameObject;
class cMaterial;
class ResourceMesh;
class ResourceTexture;

enum FileExtensions
{
	Scene3D = 0,
	Image,
	Folder,
	Unsupported
};

class Importer
{

public:

	static FileExtensions GetExtension(const char* path);
	static bool FileExists(const std::string& name);
	GLuint LoadImageFile(const char* file);
	GLuint LoadImageFile(const char* file , cMaterial* material);

	ImVec2 imageDimensions;

	static GameObject* ImportFbx(const char* path);
	ResourceMesh* LoadMesh(const aiScene* scene , int meshIndex);
	ResourceTexture* LoadTexture(const aiScene* scene , int textIndex , const char* path , cMaterial* material);
	AABB maxBox;

private:
	bool bDevilInit = false;
	bool bCalcRet = false;
	std::vector<AABB> meshesBoxes;
	
};
#endif // !__IMPORTER__