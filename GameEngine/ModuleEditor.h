#pragma once
#include "Module.h"
#include "MathGeoLib/MathGeoLib.h"
#include <map>

class GameObject;

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

	void ConfigApplication( Timer &start, float &cap);

	GameObject* selected = nullptr;

private:
	ImGuiTextBuffer console_buffer;

	Style blueStyle;
	Style mintStyle;
	Style arcStyle;
	Style gteStyle;

	float sizeX;
	bool bGeometryFigures = false;

	std::list<AABB> box_list;
	std::map<std::string, bool> window_menu;
	std::map<std::string, bool> file_menu;
	std::map<std::string, bool> help_menu;
	std::map<std::string, bool> tools_menu;

	void MenuFile();
	void MenuWindow();
	void MenuTools();
	void MenuHelp();
	void Console();
	void Configuration();
	void Heriarchy();
	void Properties();
	void LoadFile();
	void StyleEditor();
	void ToolRandom();
	void ViewGeometry();
	void HelpAbout();

	void SetStyle(Style style);
	void InitStyles();

private:

	// Random Temporal
	float fRandomMin = 0.0f;
	float fRandomMax = 0.0f;
	float fResult = 0.0f;
	int iSelectedRand = 0;

	// Config Temporal
	float fpsArr[100] = {};
	float msArr[100] = {};
	float millisec = 0.0f;
	float fps = 60.0f;
	bool bFreeze = false;
};
