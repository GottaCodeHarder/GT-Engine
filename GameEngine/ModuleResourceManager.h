#ifndef __RESOURCEMANAGER__
#define __RESOURCEMANAGER__

#include <map>

class Application;
class ResourceMesh;
class Resources;

class ResourceManager : public Module
{
public:

	ResourceManager(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ResourceManager();

	bool Init();
	void AddImGui();
	bool CleanUp();

	ResourceMesh* LoadResourceMesh(const aiScene* scene, int meshIndex , std::string name);
	void UnLoadResource(Resources* resourceUnLoad);

	std::map<std::string , Resources*> resources;
};

#endif // !__RESOURCES__
