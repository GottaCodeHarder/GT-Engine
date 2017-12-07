#include "ModuleGUI.h"
#include "Application.h"

ModuleGUI::ModuleGUI(Application * app, bool start_enabled) : Module(app, start_enabled)
{

}

bool ModuleGUI::Init()
{
	return true;
}

update_status ModuleGUI::Update(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleGUI::CleanUp()
{
	return true;
}

void ModuleGUI::AddImGui()
{

}
