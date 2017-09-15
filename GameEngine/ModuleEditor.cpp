#include "Globals.h"
#include "Application.h"


ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleEditor::~ModuleEditor()
{

}

// Load assets
bool ModuleEditor::Start()
{
	LOG("Loading Editor");
	bool ret = true;

	show_test_window = true;
	done = false;

	return ret;

}

// Load assets
bool ModuleEditor::CleanUp()
{
	LOG("Unloading Editor stuff");

	return true;
}

// Update
update_status ModuleEditor::Update(float dt)
{

	// Temporal ImGui
	//ImGui::ShowTestWindow(&show_test_window);

	return UPDATE_CONTINUE;
}