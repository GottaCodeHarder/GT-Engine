#ifndef __RESOURCEMANAGER__
#define __RESOURCEMANAGER__

#include <map>

class Application;
class ResourceMesh;
class ResourceTexture;
class Resources;

class ResourceManager : public Module
{
public:

	ResourceManager(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ResourceManager();

	bool Init();
	bool Start();
	void AddImGui();
	bool CleanUp();

	ResourceMesh* LoadResourceMesh(const aiScene* scene, int meshIndex , std::string name);
	ResourceTexture* LoadResourceTexture(const aiScene* scene, int textIndex, std::string path, std::string name , cMaterial* componentMaterial);
	ResourceTexture* LoadResourceTexture(std::string name , cMaterial* componentMaterial);
	void UnLoadResource(Resources* resourceUnLoad);

	std::map<std::string , Resources*> resources;
};

#endif // !__RESOURCES__
