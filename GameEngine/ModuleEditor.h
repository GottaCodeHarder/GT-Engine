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
	
	bool bShowExample;
	bool showIntersections;
	bool showFigures;
	std::list<AABB> box_list;
	void Intersections();

	bool MenuFile();
	void MenuView();
	void MenuTools();
	void MenuAbout();
};
