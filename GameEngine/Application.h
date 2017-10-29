#pragma once

#include "Globals.h"
#include "Timer.h"
#include "Module.h"


class ModuleWindow;
class ModuleInput;
class ModuleAudio;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModulePhysics3D;
class ModuleEditor;
class ModuleFileSystem;
class ModuleHardware;
class ModuleScene;

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModulePhysics3D* physics;
	ModuleEditor* editor;
	ModuleFileSystem* fileSystem;
	ModuleHardware* hardware;
	ModuleScene* scene;

private:

	Timer	msTimer;
	float	dt;
	std::vector<Module*> modulesList;

public:

	Application();
	~Application();

	bool Init();
	void AddImGui();
	update_status Update();
	bool CleanUp();

	std::string name;
	std::string organization;

	void RequestBrowser(std::string link);

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

	uint  frameCount = 0;
	Timer startUp;
	float capFramerate = 60.0f;

	void LoadConfiguration();
	void SaveConfiguration();
};

extern Application* App;