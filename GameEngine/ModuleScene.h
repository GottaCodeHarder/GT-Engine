#ifndef __ModuleScene_H__
#define __ModuleScene_H__

#include "Module.h"
#include "Importer.h"
#include "SDL/include/SDL.h"

class Application;
class GameObject;
//class Importer;

class ModuleScene : public Module
{
public:

	ModuleScene(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleScene();

	bool Init();
	void AddHeriarchyGui();
	bool CleanUp();
	update_status PreUpdate(float dt);
	update_status Update(float dt);

	GameObject* CreateGameObject(std::string name = "GameObject", bool active = true, GameObject* parent = nullptr);
	void CreateFbx(char* path);
	void ResetScene();

public:
	GameObject* root;
	Importer importer;
	std::vector<AABB> fbxMaxBoxes;

private:

};

#endif // __ModuleScene_H__