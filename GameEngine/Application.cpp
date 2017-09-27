#include "Application.h"
#include "ImGui\imgui_impl_sdl_gl3.h"
#include "glew\include\glew.h"
#include "JSON\parson.h"

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
	hardware = new ModuleHardware(this);

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
	AddModule(hardware);
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

	// We load Configuration.json
	LoadConfiguration();

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
			
			ImGui::Text("App Name: %s", name.c_str());
			ImGui::Text("Organization: %s", organization.c_str());

			static float f1 = 0.0f;
			ImGui::PushItemWidth(250);
			ImGui::SliderFloat("Max FPS", &f1, 0.0f, 144.0f, "%.1f");
			
			if (second <= (startUp.readSec() - 1.f))
			{
				if (second == 0.0f)
					second = startUp.readSec();
				else
					second += 1.0f;

				for (int i = 0; i <= 49; i++)
				{
					if (i == 49)
						fpsArr[i] = ImGui::GetIO().Framerate;
					else
						fpsArr[i] = fpsArr[i + 1];
				}
			}

			for (int i = 0; i <= 99; i++)
			{
				if (i == 99)
					msArr[i] = (startUp.Read() - millisec);
				else
					msArr[i] = msArr[i + 1];
			}
			millisec = startUp.Read();

			char title[25];
			sprintf_s(title, 25, "Framerate %.1f", fpsArr[49]);
			ImGui::PlotHistogram("##framerate", fpsArr, ((int)(sizeof(fpsArr) / sizeof(*fpsArr))), 0, title, 0.0f, 150.0f, ImVec2(310, 100));
			sprintf_s(title, 25, "Milliseconds %.1f", msArr[99]);
			ImGui::PlotHistogram("##milliseconds", msArr, ((int)(sizeof(msArr) / sizeof(*msArr))), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
		}

		for (it = modulesList.begin(); it != modulesList.end(); it++)
		{
			(*it)->AddImGui();
		}
		ImGui::End();
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