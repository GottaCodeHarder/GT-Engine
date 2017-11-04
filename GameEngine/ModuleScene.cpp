#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "GameObject.h"
//#include "Importer.h"
#include "cTransform.h"
#include "cCamera.h"
#include "cMesh.h"
#include "QuadTree.h"

#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	root = new GameObject("root", true, nullptr);

	root->AddComponent(new cTransform(root));
}

// Destructor
ModuleScene::~ModuleScene()
{
	delete root;
}

// Called before render is available
bool ModuleScene::Init()
{
	MYLOG("Init SDL window & surface");
	bool ret = true;


	GameObject* tmp = CreateGameObject("Camera");
	cCamera* c = new cCamera(tmp);
	tmp->AddComponent(c);

	return ret;
}

void ModuleScene::AddHeriarchyGui()
{
	if (ImGui::Button("Recalculate QuadTree"))
	{
		quad.EmtpyQuad();
		quad.RecalculateQuad(root);
	}
	for (auto sonsRoot : root->sons)
	{
		sonsRoot->DrawHeriarchy(sonsRoot);
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

update_status ModuleScene::PreUpdate(float dt)
{

	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt)
{
	glMatrixMode(GL_MODELVIEW);
	root->PreUpdate();
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

	if (rayCast)
	{
		const cCamera* cameraTMP = App->camera->GetDefaultCamera();
		float2 pointNearPlane = cameraTMP->frustum.ScreenToViewportSpace(App->input->GetMouseX(), App->input->GetMouseY(), SCREEN_WIDTH, SCREEN_HEIGHT);
		float3 dir = cameraTMP->frustum.UnProject(pointNearPlane).dir;
		float3 dir1 = cameraTMP->frustum.UnProjectFromNearPlane(pointNearPlane.x, pointNearPlane.y).dir;

		RayCastHit goSelected = RayCast(((cTransform*)cameraTMP->gameObject->FindComponent(TRANSFORM))->positionLocal, dir1);
		App->editor->selected = goSelected.gameObject;
		rayCast = false;
	}

	quad.Draw();
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
	cTransform* trs = new cTransform(ret);
	ret->AddComponent(trs);

	return ret;
}

void ModuleScene::CreateFbx(char* path)
{
	GameObject* tmp = importer.LegacyLoadFbx(path);
	App->editor->selected = *tmp->sons.begin();
	for (auto childsTmp : tmp->sons)
	{
		//App->editor->selected = childsTmp;
		childsTmp->parent = root;
		root->sons.push_back(childsTmp);
	}

	App->camera->referenceDone = true;
	if (tmp != nullptr)
	{
		cTransform* cameraTmp = ((cTransform*)App->camera->GetDefaultCamera()->gameObject->FindComponent(TRANSFORM));

		fbxMaxBoxes.push_back(importer.maxBox);
		App->camera->Position.x = importer.maxBox.maxPoint.x * 2;
		App->camera->Position.y = importer.maxBox.maxPoint.y * 2;
		App->camera->Position.z = importer.maxBox.maxPoint.z * 2;
		cameraTmp->positionLocal = { App->camera->Position.x, App->camera->Position.y, App->camera->Position.z };
		App->camera->LookAt(vec(importer.maxBox.CenterPoint().x, importer.maxBox.CenterPoint().y, importer.maxBox.CenterPoint().z));
		tmp = nullptr;
		delete tmp;
	}
	
}

void ModuleScene::ResetScene()
{
	for (auto itSons : root->sons)
	{
		delete itSons;
	}
	root = new GameObject("root", true, nullptr);

	root->AddComponent(new cTransform(root));
	App->editor->selected = nullptr;

}

RayCastHit ModuleScene::RayCast(const float3 & position, const float3 & direction)
{
	LineSegment ray(position , position+direction*500);
	std::map<float, GameObject*> gameObjectsOrdered;

	{
		std::vector<GameObject*> posibleColisions = quad.GetRoot().Collide(ray);
		//COMPROVAR TOTS ELS DINAMICS
		GetDynamicGO(root, posibleColisions);

		for (auto itGO : posibleColisions)
		{
			if (itGO->FindComponent(MESH) != nullptr)
			{
				float3 distance = position - itGO->aabbBox.CenterPoint();
				float distance1 = distance.LengthSq();
				gameObjectsOrdered.insert(std::pair<float, GameObject*>(distance1, itGO));
			}
		}
	}

	RayCastHit ret;

	for (auto itGO : gameObjectsOrdered)
	{
		cMesh* mesh = ((cMesh*)itGO.second->FindComponent(MESH));
		for (int i = 0; i <= mesh->numIndex-3 ; i += 3)
		{
			Triangle triangle(mesh->vertex[mesh->index[i]], mesh->vertex[mesh->index[i + 1]], mesh->vertex[mesh->index[i + 2]]);
			if (triangle.Intersects(ray))
			{
				float3 distanceTri = position - triangle.CenterPoint();
				if (ret.distance > distanceTri.Length() || ret.distance==0)
				{
					ret.distance = distanceTri.Length();
					ret.gameObject = itGO.second;
					ret.position = triangle.CenterPoint();
					ret.normal = triangle.NormalCCW();
				}
			}
		}
		if (ret.gameObject != nullptr)
		{
			break;
		}
	}
	return ret;
}

std::vector<GameObject*> ModuleScene::GetDynamicGO(const GameObject * GO, std::vector<GameObject*>& posibleCol)
{

	for (auto dynamicObj : GO->sons)
	{
		if (!dynamicObj->statiC)
		{
			posibleCol.push_back(dynamicObj);
		}
		GetDynamicGO(dynamicObj,posibleCol);
	}

	return posibleCol;
}
