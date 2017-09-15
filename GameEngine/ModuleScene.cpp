#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "ImGui\imgui.h"
#include "ImGui\examples\sdl_opengl3_example\imgui_impl_sdl_gl3.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleScene::~ModuleScene()
{

}

// Load assets
bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	return ret;

}

// Load assets
bool ModuleScene::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleScene::Update(float dt)
{
	// Title Stuff

	char title[500];
	sprintf_s(title, "GT ENGINE");

	App->window->SetTitle(title);

	return UPDATE_CONTINUE;
}

void ModuleScene::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	
}

bool ModuleScene::Reset()
{
	return true;
}