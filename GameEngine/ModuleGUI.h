#ifndef __MODULE_GUI__
#define __MODULE_GUI__

#include "Module.h"
#include <list>

class cUI; 
class cCanvas;
class GameObject;


class ModuleGUI : public Module
{
public:
	ModuleGUI(Application* app, bool start_enabled = true);

	bool Init();
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void AddImGui();

	void CreateCanvas();

	bool AddUIImage(char* path);
	bool AddUIButton(char* path);
	bool AddUIFont(char* path);
	
	uint LoadUIImage(char* path);

private:
	std::list<cUI*> uiComponents;
	cCanvas* canvas;
};

#endif // !__MODULE_GUI__
