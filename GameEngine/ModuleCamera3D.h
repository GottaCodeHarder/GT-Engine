#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/MathGeoLib.h"
#include "glmath.h"

class cCamera;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(vec3 Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	bool referenceDone = false;
	float* GetViewMatrixFloat();
	const float4x4 GetViewMatrix();
	const float4x4 GetViewMatrixInverse();
private:

	void CalculateViewMatrix();

public:

	vec3 X, Y, Z, Position, Reference, cameraSpeed, cameraDistance;

private:

	cCamera* defaultCamera;

	float4x4 ViewMatrix, ViewMatrixInverse;
	bool camera;
	
};