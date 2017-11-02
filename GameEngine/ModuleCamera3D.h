#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/MathGeoLib.h"

class cCamera;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(vec Position, const vec &Reference, bool RotateAroundReference = false);
	void LookAt(const vec &Spot);
	void Move(const vec &Movement);
	bool referenceDone = false;
	float* GetViewMatrixFloat();
	const float4x4 GetViewMatrix();
	const float4x4 GetViewMatrixInverse();
	const cCamera* GetDefaultCamera();
private:

	void CalculateViewMatrix();

public:

	vec X, Y, Z, Position, Reference, cameraSpeed, cameraDistance;

private:

	cCamera* defaultCamera;

	float4x4 ViewMatrix, ViewMatrixInverse;
	bool camera;
	
};