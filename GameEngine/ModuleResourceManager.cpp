#include "Globals.h"
#include "Application.h"
#include "Importer.h"
#include "ModuleResourceManager.h"
#include "Resources.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"
#include "ModuleScene.h"
#include "Importer.h"
#include "cMaterial.h"

ResourceManager::ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

ResourceManager::~ResourceManager()
{
}

bool ResourceManager::Init()
{
	return false;
}

void ResourceManager::AddImGui()
{
}

bool ResourceManager::CleanUp()
{
	return false;
}

ResourceMesh * ResourceManager::LoadResourceMesh(const aiScene* scene, int meshIndex , std::string name)
{
	ResourceMesh* ret;

	std::map<std::string, Resources*>::iterator it =  resources.find(name);
	if (it != resources.end())
	{
		it->second->nReferences++;
		return (ResourceMesh*)it->second;
	}

	ret = App->scene->importer.LoadMesh(scene, meshIndex);
	ret->name = name;
	ret->nReferences++;
	if (ret != nullptr)
		resources.insert(std::pair<std::string, Resources*>(name, ret));

	return ret;
}

ResourceTexture * ResourceManager::LoadResourceTexture(const aiScene * scene, int textIndex, std::string path, std::string name , cMaterial* componentMaterial)
{
	ResourceTexture* ret;

	std::map<std::string, Resources*>::iterator it = resources.find(name);
	if (it != resources.end())
	{
		it->second->nReferences++;
		return (ResourceTexture*)it->second;
	}

	ret = App->scene->importer.LoadTexture(scene, textIndex, path.c_str(), componentMaterial);
	ret->name = name;
	ret->nReferences++;
	if (ret != nullptr)
		resources.insert(std::pair<std::string, Resources*>(name, ret));

	return ret;
}

ResourceTexture * ResourceManager::LoadResourceTexture(std::string name, cMaterial * componentMaterial)
{
	ResourceTexture* ret;
	std::map<std::string, Resources*>::iterator it = resources.find(name);
	if (it != resources.end())
	{
		it->second->nReferences++;
		return (ResourceTexture*)it->second;
	}
	ret = componentMaterial->resource;
	if (ret != nullptr)
	{
		ret->buffTexture = App->scene->importer.LoadImageFile(name.c_str(), componentMaterial);
		ret->name = name;
		ret->nReferences++;
		if (ret != nullptr)
			resources.insert(std::pair<std::string, Resources*>(name, ret));
	}

	return ret;
}



void ResourceManager::UnLoadResource(Resources* resourceUnLoad)
{
	std::map<std::string, Resources*>::iterator it = resources.find(resourceUnLoad->name);
	if (it != resources.end())
	{
		it->second->nReferences--;
		if (it->second->nReferences == 0)
		{
			resources.erase(it);
			delete it->second;
		}
	}
}
