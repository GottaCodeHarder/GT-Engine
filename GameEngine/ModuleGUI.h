#ifndef __MODULE_GUI__
#define __MODULE_GUI__

#include "Module.h"

class ModuleGUI : public Module
{
public:
	ModuleGUI(Application* app, bool start_enabled = true);

	bool Init();
	update_status Update(float dt);
	bool CleanUp();

	void AddImGui();
};

#endif // !__MODULE_GUI__
