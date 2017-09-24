#pragma once
#include "Module.h"
#include <map>

class ModuleEditor : public Module
{
public:
	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void AddTextConsole(char* text);

private:
	ImGuiTextBuffer console_buffer;

	bool bShowRandom;
	bool bGeometryFigures;
	std::list<AABB> box_list;
	std::map<std::string, bool> active_menu;

	void Intersections();
	void ToolRandom();

	bool bExit;

	void MenuFile();
	void MenuView();
	void MenuTools();
	void MenuAbout();
	void Console();
	void Configuration();

private:

	// Random Temporal
	float fRandomMin = 0.0f;
	float fRandomMax = 0.0f;
	float fResult = 0.0f;
	int iSelectedRand = 0;
};
