#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "cTransform.h"

#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

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

	root->AddComponent(new cTransform(root));
	return ret;
}

void ModuleScene::AddHeriarchyGui()
{
	for (auto sonsRoot : root->sons)
	{
		sonsRoot->DrawHeriarchy(sonsRoot);
	}

}

void ModuleScene::AddPropertiesGui()
{
	if (root->sons.empty())
	{
		ImGui::Text("You must first load a Scene!");
	}
	else
	{
		for (auto sonsRoot : root->sons)
		{
			sonsRoot->DrawProperties();
		}
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

update_status ModuleScene::Update(float dt)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	root->Update();
	glLoadIdentity();
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

	return UPDATE_CONTINUE;
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
		//root = childsTmp->parent;
		childsTmp->parent = root;
		root->sons.push_back(childsTmp);
	}

	App->camera->referenceDone = true;
	if (tmp != nullptr)
	{
		fbxMaxBoxes.push_back(importer.maxBox);
		App->camera->Position.x = importer.maxBox.maxPoint.x * 2;
		App->camera->Position.y = importer.maxBox.maxPoint.y * 2;
		App->camera->Position.z = importer.maxBox.maxPoint.z * 2;
		App->camera->LookAt(vec3(importer.maxBox.CenterPoint().x, importer.maxBox.CenterPoint().y, importer.maxBox.CenterPoint().z));
		tmp = nullptr;
		delete tmp;
	}
	
}
