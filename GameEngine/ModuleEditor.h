#pragma once
#include "Module.h"

class ModuleEditor : public Module
{
public:
	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

private:

	bool show_test_window;
	bool our_test_window;
	bool done;
};
