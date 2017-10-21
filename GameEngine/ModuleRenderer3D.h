#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();
	void AddImGui();

	void ImporterUI();


	void LoadImages(char* path) {};

	void OnResize(int width, int height);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 normalMatrix;
	mat4x4 modelMatrix, viewMatrix, projectionMatrix;
	void FillVertex();
	void DirectCube();
	void DrawGameObject(GameObject* go);
	bool bEnableWireframe = false;

private:
	bool bEnableDepthTest = true;
	bool bEnableCullFace = true;
	bool bEnableLighting = true;
	bool bEnableColorMaterial = true;
	bool bEnableGLTexture = true;


	float vertices[108] = {};

	
	uint myId = 0;

};