#pragma once
#include "Module.h"
#include "MathGeoLib\MathGeoLib.h"

class ModuleEditor : public Module
{
public:
	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

private:
	void Intersections();
	bool bShowExample;
	std::list<AABB> box_list;

	bool MenuFile();
	void MenuView();
	void MenuTools();
	void MenuAbout();
};
