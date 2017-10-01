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

	void AddImGui();

	void AddTextConsole(char* text);

	void Draw();

private:
	ImGuiTextBuffer console_buffer;

	bool bSetStyle = false;
	bool bShowRandom = false;
	bool bShowExample = false;
	bool bShowAbout = false;
	bool bGeometryFigures = false;
	bool bExit = false;

	std::list<AABB> box_list;
	std::map<std::string, bool> active_menu;

	void ToolRandom();
	void ViewGeometry();
	void HelpAbout();

	void MenuFile();
	void MenuView();
	void MenuTools();
	void MenuAbout();
	void Console();
	void Configuration();

	void SetStyle(bool set);

private:

	// Random Temporal
	float fRandomMin = 0.0f;
	float fRandomMax = 0.0f;
	float fResult = 0.0f;
	int iSelectedRand = 0;
};
