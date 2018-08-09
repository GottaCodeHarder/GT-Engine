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
class cImage;
class cLabel;
class cButton;
class cCheckbox;
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
	void CreateDemo(); //3rd Assigment Scene

	void CreateCanvas();
	cImage* AddUIImage(GameObject* parent = nullptr, const char* path = nullptr);
	cLabel* AddUILabel(GameObject* parent = nullptr, const char* text = nullptr, uint size = 24, const char* path = nullptr);
	cButton* AddUIButton(GameObject* parent = nullptr);
	cCheckbox* AddUICheckBox(GameObject* parent = nullptr);
	const char* AddUIFont(const char* path = nullptr);
	
	uint LoadUIImage(char* path, cUI* component = nullptr);

private:
	bool toggle = true;
	bool justOnce = true;
	cImage* imageA = nullptr;
	cCheckbox* check = nullptr;

	std::list<cUI*> uiComponents;
	cCanvas* canvas;

	Random rand;
};

#endif // !__MODULE_GUI__