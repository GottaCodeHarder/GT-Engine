#pragma once
#include "Module.h"
#include "MathGeoLib/MathGeoLib.h"
#include <map>

struct Style
{
	bool active = true;

	float3 cText;
	float3 cHead;
	float3 cArea;
	float3 cBody;
	float3 cPops;
};

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

	void ConfigApplication(bool &bFreeze, Timer &start, float &cap, float &mil, float &fps);

private:
	ImGuiTextBuffer console_buffer;

	Style blueStyle;
	Style mintStyle;
	Style arcStyle;
	Style gteStyle;

	float sizeX;

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
	void MenuWindow();
	void MenuTools();
	void MenuAbout();
	void Console();
	void Configuration();
	void Heriarchy();
	void Properties();

	void SetStyle(Style style);
	void InitStyles();

private:

	// Random Temporal
	float fRandomMin = 0.0f;
	float fRandomMax = 0.0f;
	float fResult = 0.0f;
	int iSelectedRand = 0;
};
