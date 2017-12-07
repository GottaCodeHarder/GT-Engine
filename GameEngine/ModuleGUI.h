#pragma once
#include "Module.h"
#include "Globals.h"

class Application;

class ModuleGUI : public Module
{
public:
	ModuleGUI(Application* app, bool start_enabled = true);

	bool Init();
	update_status Update(float dt);
	bool CleanUp();

	void AddImGui();
};