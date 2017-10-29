#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include "Primitive.h"
#include "Bullet/src/btBulletDynamicsCommon.h"

#define MAX_LIGHTS 8
class DebugDrawer;
class GameObject;

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

	void OnResize(int width, int height);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 normalMatrix;
	mat4x4 modelMatrix, viewMatrix, projectionMatrix;
	void FillVertex();
	void DirectCube();
	void DrawGameObject(GameObject* go);
	void DrawFrustum(const math::Frustum);
	bool bEnableWireframe = false;
	DebugDrawer* debugDraw = nullptr;

private:
	bool bEnableDepthTest = true;
	bool bEnableCullFace = true;
	bool bEnableLighting = true;
	bool bEnableColorMaterial = true;
	bool bEnableGLTexture = true;

	float vertices[108] = {};

	
	uint myId = 0;

};

class DebugDrawer : public btIDebugDraw
{
public:
	DebugDrawer() : line(0, 0, 0)
	{}

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);
	void reportErrorWarning(const char* warningString);
	void draw3dText(const btVector3& location, const char* textString);
	void setDebugMode(int debugMode);
	int	 getDebugMode() const;

	DebugDrawModes mode;
	pbLine line;
	Primitive point;
};