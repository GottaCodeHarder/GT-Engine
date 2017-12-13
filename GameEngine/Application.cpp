#include "Application.h"
#include "ImGui\imgui_impl_sdl_gl3.h"
#include "glew\include\glew.h"
#include "JSON\parson.h"

#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleEditor.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics3D.h"
#include "ModuleFileSystem.h"
#include "ModuleHardware.h"
#include "ModuleScene.h"
#include "ModuleResourceManager.h"
#include "ModuleGUI.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	renderer3D = new ModuleRenderer3D(this);
	physics = new ModulePhysics3D(this);
	editor = new ModuleEditor(this);
	fileSystem = new ModuleFileSystem(this);
	hardware = new ModuleHardware(this);
	scene = new ModuleScene(this);
	camera = new ModuleCamera3D(this);
	resourceManager = new ResourceManager(this);
	userinterface = new ModuleGUI(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(input);
	AddModule(audio);
	AddModule(physics);
	AddModule(fileSystem);
	AddModule(hardware);
	AddModule(editor);
	AddModule(resourceManager);
	AddModule(scene);
	AddModule(camera);
	AddModule(userinterface);

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

	// We load Configuration.json
	LoadConfiguration();

	// After all Init calls we call Start() in all modules
	MYLOG("Application Start --------------");
	for (std::vector<Module*>::iterator it = modulesList.begin(); ret == true && it != modulesList.end(); it++)
	{
		(*it)->Start();
	}
	App->window->SetTitle(name.c_str());

	msTimer.Start();
	//msTimerGame.Start();
	dtGame = 0.f;
	return ret;
}

void Application::AddImGui()
{
	std::vector<Module*>::iterator it = modulesList.begin();

	editor->ConfigApplication(startUp, capFramerate, vSync);

	for (it = modulesList.begin(); it != modulesList.end(); it++)
	{
		(*it)->AddImGui();
	}
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)msTimer.Read() / 1000.0f;
	msTimer.Start();

	//Game Timer
	if (isPlaying)
	{
		SetGameDt((float)msTimerGame.Read() / 1000.0f);
	}
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

	SaveConfiguration();

	editor = NULL;
	for (std::vector<Module*>::reverse_iterator it = modulesList.rbegin(); ret == true && it != modulesList.rend(); it++)
	{
		ret = (*it)->CleanUp();
		delete (*it);
		*it = NULL;
	}

	return ret;
}

void Application::SetGameDt(float dtGameNew)
{
	dtGame = dtGameNew;
}

const float Application::GetGameDt()
{
	return dtGame;
}

Timer* Application::GetGameTimer()
{
	return &msTimerGame;
}

void Application::RequestBrowser(std::string link)
{
	ShellExecute(NULL, "open", link.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void Application::AddModule(Module* mod)
{
	modulesList.push_back(mod);
}
void Application::LoadConfiguration()
{
	JSON_Value *config;

	// If there's not Configuration json, it creates one
	if ((config = json_parse_file("Configuration.json")) == NULL)
	{
		MYLOG("Failed to find Configuration File!");
		return;
	}

	JSON_Object * appInfo = json_object_dotget_object(json_object(config), "App");

	name = json_object_dotget_string(appInfo, "Name");
	organization = json_object_dotget_string(appInfo, "Organization");

	// Modules
}

void Application::SaveConfiguration()
{
	JSON_Value *config;

	// If there's not Configuration json, it creates one
	if ((config = json_parse_file("Configuration.json")) == NULL)
	{
		config = json_value_init_object();
	}

	JSON_Object* appInfo = json_object_dotget_object(json_object(config), "App");

	json_object_dotset_string(appInfo, "Name", name.c_str());
	json_object_dotset_string(appInfo, "Organization", organization.c_str());

	// Modules

	json_serialize_to_file(config, "Configuration.json");
}