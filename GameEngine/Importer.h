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

enum FileExtensions
{
	Scene3D = 0,
	Image,
	Unsupported
};

class Importer
{

public:

	FileExtensions GetExtension(const char* path);
	bool FileExists(const std::string& name);
	GLuint LoadImageFile(const char* file);

	ImVec2 imageDimensions;

	GameObject* LoadFbx(const char* path);
	AABB maxBox;

private:
	bool bDevilInit = false;
	bool bCalcRet = false;
	std::vector<AABB> meshesBoxes;
	
};