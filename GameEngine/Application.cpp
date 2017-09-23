#include "Application.h"
#include "ImGui\imgui_impl_sdl_gl3.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	scene = new ModuleScene(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	physics = new ModulePhysics3D(this);
	player = new ModulePlayer(this);
	editor = new ModuleEditor(this);
	file_system = new ModuleFileSystem(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(audio);
	AddModule(physics);
	AddModule(file_system);
	AddModule(editor);

	// Scenes
	AddModule(scene);
	AddModule(player);

	// Renderer last!
	AddModule(renderer3D);
}

Application::~Application()
{
	for (std::vector<Module*>::iterator it = modulesList.begin(); it != modulesList.end(); it++)
	{
		delete (*it);
	}
}

bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules
	for (std::vector<Module*>::iterator it = modulesList.begin(); ret == true && it != modulesList.end(); it++)
	{
		(*it)->Init();
	}

	// After all Init calls we call Start() in all modules
	MYLOG("Application Start --------------");
	for (std::vector<Module*>::iterator it = modulesList.begin(); ret == true && it != modulesList.end(); it++)
	{
		(*it)->Start();
	}
	
	ms_timer.Start();
	return ret;
}

void Application::AddImGui()
{
	std::vector<Module*>::iterator it = modulesList.begin();

	if (ImGui::Begin("Configuration"))
	{
		
		if (ImGui::CollapsingHeader("Application"))
		{
			char title[25];
			//sprintf_s(title, "Framerate &.1f", fps_log[fps_log.size() - 1]);

		}
		for (it = modulesList.begin(); it != modulesList.end(); it++)
			{
			(*it)->AddImGui();
			}
		ImGui::End();
	}

	if (ImGui::CollapsingHeader("Hardware"))
	{

	}

	


}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	std::vector<Module*>::iterator it = modulesList.begin();

	for (; ret == UPDATE_CONTINUE && it != modulesList.end(); it++)
	{
		ret = (*it)->PreUpdate(dt);
	}

	for (it = modulesList.begin(); ret == UPDATE_CONTINUE && it != modulesList.end(); it++)
	{
		ret = (*it)->Update(dt);
	}
	
	for (it = modulesList.begin(); ret == UPDATE_CONTINUE && it != modulesList.end(); it++)
	{
		ret = (*it)->PostUpdate(dt);
	}


	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	editor = NULL;
	for (std::vector<Module*>::reverse_iterator it = modulesList.rbegin(); ret == true && it != modulesList.rend(); it++)
	{
		ret = (*it)->CleanUp();
		delete (*it);
		*it = NULL;
	}

	return ret;
}

void Application::AddModule(Module* mod)
{
	modulesList.push_back(mod);
}