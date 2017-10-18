#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	
}

// Destructor
ModuleScene::~ModuleScene()
{
}

// Called before render is available
bool ModuleScene::Init()
{
	MYLOG("Init SDL window & surface");
	bool ret = true;
	root = new GameObject("root", true, nullptr);
	
	return ret;
}

void ModuleScene::AddImGui()
{
	if (ImGui::CollapsingHeader("Window"))
	{
		
	}
}

// Called before quitting
bool ModuleScene::CleanUp()
{
	MYLOG("Destroying SDL window and quitting all SDL systems");

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleScene::Update()
{
	root->Update();

	if (App->input->has_dropped)
	{
		FileExtensions extension = importer.GetExtension(App->input->GetFileDropped());

		switch (extension)
		{
		case FileExtensions::Scene3D:
		{
			CreateFbx((char*)App->input->GetFileDropped());
			break;
		}
		case FileExtensions::Image:
		{
			//LoadImages((char*)App->input->GetFileDropped());
			break;
		}
		case FileExtensions::Unsupported:
		{
			MYLOG("File Type not supported by GT Engine");
			break;
		}
		}

		App->input->has_dropped = false;
	}
}

GameObject * ModuleScene::CreateGameObject(std::string name, bool active, GameObject * parent)
{
	GameObject* ret = nullptr;
	if (parent == nullptr)
	{
		ret = new GameObject(name, active, root);
		root->sons.push_back(ret);
	}
	else
	{
		ret = new GameObject(name, active, parent);
		parent->sons.push_back(ret);
	}

	return ret;
}

void ModuleScene::CreateFbx(char* path)
{
	GameObject* tmp = importer.LoadFbx(path);
	for (auto childsTmp : tmp->sons)
	{
		childsTmp->parent = root;
		root->sons.push_back(childsTmp);
	}

	App->camera->referenceDone = true;
	if (tmp != nullptr)
	{
		
		App->camera->Position.x = importer.maxBox.maxPoint.x * 2;
		App->camera->Position.y = importer.maxBox.maxPoint.y * 2;
		App->camera->Position.z = importer.maxBox.maxPoint.z * 2;
		App->camera->LookAt(vec3(importer.maxBox.CenterPoint().x, importer.maxBox.CenterPoint().y, importer.maxBox.CenterPoint().z));

		delete tmp;
	}
	
}
