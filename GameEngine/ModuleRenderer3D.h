#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include "Importer.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void AddImGui();

	void OnResize(int width, int height);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	void FillVertex();
	void DirectCube();
	std::vector<Mesh*> meshes;

private:
	bool bEnableDepthTest = true;
	bool bEnableCullFace = true;
	bool bEnableLighting = true;
	bool bEnableColorMaterial = true;
	bool bEnableGLTexture = true;
	bool bEnableWireframe = false;

	float vertices[108] = {};
	char * dropped_fbx_path = NULL;


	Importer importer;
	uint my_id = 0;

};