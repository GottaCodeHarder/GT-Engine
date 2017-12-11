#include "ModuleGUI.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleScene.h"
#include "UIComponents.h"

#include "GTInterface/GTInterface.h"

ModuleGUI::ModuleGUI(Application * app, bool start_enabled) : Module(app, start_enabled)
{
	uiComponents.clear();
	canvas = nullptr;
}

bool ModuleGUI::Init()
{
	return true;
}
bool ModuleGUI::Start()
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

bool ModuleGUI::AddUIImage(char* path)
{
	if (!canvas)
	{
		GameObject* canvas_go = App->scene->CreateGameObject("Canvas");
		canvas = new cCanvas(canvas_go);
	}

	GameObject* image_go = App->scene->CreateGameObject("Image", true, canvas->gameObject);
	cImage* image = new cImage(image_go);
	image->image->buffTexture = GTI::GTInterface.LoadTexture(path);
	return image->image->buffTexture != 0;
}
