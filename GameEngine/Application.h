#pragma once

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleEditor.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics3D.h"
#include "ModuleFileSystem.h"
#include "ModuleHardware.h"
#include "ModuleScene.h"

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

	float fpsArr[100] = {};
	float msArr[100] = {};

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

	bool bFreeze = false;
	uint  frameCount = 0;
	Timer startUp;
	float millisec = 0.0f;
	float capFramerate = 60.0f;
	float fps = 60.0f;

	void LoadConfiguration();
	void SaveConfiguration();
};

extern Application* App;