#pragma once

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics3D.h"
#include "ModulePlayer.h"
#include "ModuleFileSystem.h"
#include "ModuleHardware.h"

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleScene* scene;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModulePhysics3D* physics;
	ModulePlayer* player;
	ModuleEditor* editor;
	ModuleFileSystem* file_system;
	ModuleHardware* hardware;

private:

	Timer	ms_timer;
	float	dt;
	std::vector<Module*> modulesList;

public:

	Application();
	~Application();

	bool Init();
	void AddImGui();
	update_status Update();
	bool CleanUp();

	void RequestBrowser(std::string link);

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

	uint					frameCount = 0;
	Timer					startUp;
	float					second = 0.0f;
	float					arr[50] = {};

	std::string name;
	std::string organization;

	void LoadConfiguration();
	void SaveConfiguration();
};

extern Application* App;