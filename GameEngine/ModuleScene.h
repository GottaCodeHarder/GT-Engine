#ifndef __ModuleScene_H__
#define __ModuleScene_H__

#include "Module.h"
#include "Importer.h"
#include "SDL/include/SDL.h"
#include "QuadTree.h"

class Application;
class GameObject;
//class myQuadTree;
//class Importer;

struct RayCastHit
{
	GameObject* gameObject = nullptr;
	float3 position = float3::zero;
	float3 normal = float3::zero;
	float distance = 0.f;
};

class ModuleScene : public Module
{
public:

	ModuleScene(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleScene();

	bool Init();
	void AddHeriarchyGui();
	bool CleanUp();
	update_status PreUpdate(float dt);
	update_status Update(float dt);

	GameObject* CreateGameObject(std::string name = "GameObject", bool active = true, GameObject* parent = nullptr);
	void CreateFbx(char* path);
	void CreateGte(char* path);
	void ResetScene();
	RayCastHit RayCast(const float3& position, const float3& direction);
	std::vector<GameObject*> GetDynamicGO(const GameObject* GO , std::vector<GameObject*>& posibleCol);

	myQuadTree quad;
	bool rayCast = false;

public:
	GameObject* root;
	Importer importer;
	std::vector<AABB> fbxMaxBoxes;

private:

};

#endif // __ModuleScene_H__