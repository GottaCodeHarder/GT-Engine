#ifndef __MODULE_GUI__
#define __MODULE_GUI__

#include "Module.h"
#include "Random.h"

#include <list>
#include <vector>
#include <map>
#include <functional>

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
	void AddUIButton(char* path);
	void AddUICheckBox(char* path);
	bool AddUIFont(char* path);
	
	uint LoadUIImage(char* path, cUI* component = nullptr);

private:
	std::list<cUI*> uiComponents;
	cCanvas* canvas;

	Random rand;
};

#endif // !__MODULE_GUI__