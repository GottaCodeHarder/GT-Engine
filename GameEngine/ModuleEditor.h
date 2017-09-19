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
	bool bShowRandom;
	bool bShowGeometry;
	bool bGeometryFigures;
	std::list<AABB> box_list;

	void Intersections();
	void ToolRandom();

	bool bExit;

	void MenuFile();
	void MenuView();
	void MenuTools();
	void MenuAbout();
};
