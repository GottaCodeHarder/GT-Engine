#include "Globals.h"
#include "Application.h"
#include "PhysBody3D.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "cMesh.h"
#include "cCamera.h"
#include "cTransform.h"
#include "glmath.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	//CalculateViewMatrix();
	GameObject* tmp = App->scene->CreateGameObject("DefaultCamera");
	defaultCamera = new cCamera(tmp);
	tmp->AddComponent(defaultCamera);
	defaultCamera->frustum.SetViewPlaneDistances(1, 100);
	defaultCamera->frustum.SetWorldMatrix(((cTransform*)defaultCamera->gameObject->FindComponent(TRANSFORM))->GetGlobalMatrixTransf().Float3x4Part());
	defaultCamera->frustum.SetPerspective(1, 1);
	defaultCamera->frustum.ComputeViewProjMatrix();


	((cTransform*)defaultCamera->gameObject->FindComponent(TRANSFORM))->positionLocal = { 0.f,0.f,5.f };

	X = vec(1.0f, 0.0f, 0.0f);
	Y = vec(0.0f, 1.0f, 0.0f);
	Z = vec(0.0f, 0.0f, 1.0f);

	Position = vec(0.0f, 0.0f, 5.0f);
	Reference = vec(0.0f, 0.0f, 0.0f);
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	MYLOG("Setting up the camera");
	bool ret = true;

	camera = true;
	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	MYLOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	cameraDistance = { -20.0f, 10.0f, -20.0f };

	if (camera)
	{
		vec newPos(0, 0, 0);
		float speed = 3.0f * dt;
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			speed = 8.0f * dt;

		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) newPos.y -= speed * 2;
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) newPos.y += speed * 2;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed * 2;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed * 2;


		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed * 2;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed * 2;
		
		if (!ImGui::IsMouseHoveringAnyWindow())
		{
			if (App->input->GetMouseZ() == 1)
			{
				newPos -= Z * speed * 10;
			}

			if (App->input->GetMouseZ() == -1)
			{
				newPos += Z * speed * 10;
			}
		}


		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		{
			if (App->scene->root != nullptr)
			{
				if (App->editor->selected != nullptr)
				{
					if (((cMesh*)App->editor->selected->FindComponent(MESH)) != nullptr)
					{
						Position.x = App->editor->selected->aabbBox.maxPoint.x * 2;
						Position.y = App->editor->selected->aabbBox.maxPoint.y * 2;
						Position.z = App->editor->selected->aabbBox.maxPoint.z * 2;
						LookAt(vec(App->editor->selected->aabbBox.CenterPoint().x, App->editor->selected->aabbBox.CenterPoint().y, App->editor->selected->aabbBox.CenterPoint().z));
					}
					else if (((cTransform*)App->editor->selected->FindComponent(TRANSFORM)) != nullptr)
					{
						cTransform* tmp = ((cTransform*)App->editor->selected->FindComponent(TRANSFORM));
						cTransform* cameraTmp = ((cTransform*)defaultCamera->gameObject->FindComponent(TRANSFORM));
						Position.x = tmp->GetGlobalPos().x + 2;
						Position.y = tmp->GetGlobalPos().y + 2;
						Position.z = tmp->GetGlobalPos().z + 2;
						cameraTmp->positionLocal = { Position.x, Position.y, Position.z };
						LookAt(vec(tmp->GetGlobalPos().x, tmp->GetGlobalPos().y, tmp->GetGlobalPos().z));

					}
				}
			}		
		}


		//((cTransform*)defaultCamera->gameObject->FindComponent(TRANSFORM))->positionLocal += newPos;
		Position += newPos;
		Reference += newPos;

		// Mouse motion ----------------
			int dx = -App->input->GetMouseXMotion();
			int dy = -App->input->GetMouseYMotion();

		if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
		{
			referenceDone = true;
		}
		if (!ImGui::IsMouseHoveringAnyWindow())
		{
			if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
			{
				float Sensitivity = 0.005f;

				if (dx != 0)
				{
					float DeltaX = (float)dx * Sensitivity;
					float3x3 rotate = float3x3::RotateAxisAngle(vec(0.f, 1.f, 0.f), DeltaX);

					X = rotate * X;
					Y = rotate * Y;
					Z = rotate * Z;
				}

				if (dy != 0)
				{
					float DeltaY = (float)dy * Sensitivity;
					float3x3 rotate = float3x3::RotateAxisAngle(X, DeltaY);

					Y = rotate*Y;
					Z = rotate*Z;

					if (Y.y < 0.0f)
					{
						Z = vec(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
						Y = Z.Cross(X);
					}
				}

				Reference = Position - Reference;
			}
			else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
			{
				float Sensitivity = 0.005f;

				if (App->editor->selected != nullptr)
				{
					if (((cTransform*)App->editor->selected->FindComponent(TRANSFORM)) != nullptr && referenceDone)
					{

						Reference = vec(((cTransform*)App->editor->selected->FindComponent(TRANSFORM))->GetGlobalPos().x, ((cTransform*)App->editor->selected->FindComponent(TRANSFORM))->GetGlobalPos().y, ((cTransform*)App->editor->selected->FindComponent(TRANSFORM))->GetGlobalPos().z);
						if(((cMesh*)App->editor->selected->FindComponent(MESH)) != nullptr)
							Reference = vec(App->editor->selected->aabbBox.CenterPoint().x, App->editor->selected->aabbBox.CenterPoint().y, App->editor->selected->aabbBox.CenterPoint().z);
						referenceDone = false;
					}
				}

				Position -= Reference;

				if (dx != 0)
				{
					float DeltaX = (float)dx * Sensitivity;

					float3x3 rotate = float3x3::RotateAxisAngle(vec(0.f, 1.f, 0.f), DeltaX);

					X = rotate * X;
					Y = rotate * Y;
					Z = rotate * Z;
				}

				if (dy != 0)
				{
					float DeltaY = (float)dy * Sensitivity;
					float3x3 rotate = float3x3::RotateAxisAngle(X, DeltaY);

					Y = rotate * Y;
					Z = rotate * Z;

					if (Y.y < 0.0f)
					{
						Z = vec(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
						Y = Z.Cross(X);
					}
				}

				Position = Reference + Z * Position.Length();
			}
		}
	}

	defaultCamera->frustum.SetPos(Position);

	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(vec Position, const vec &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z =(Position - Reference);
	Z.Normalize();
	X = vec(0.0f, 1.0f, 0.0f).Cross(Z).Normalized();
	Y = Z.Cross(X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const vec &Spot)
{
	Reference = Spot;

	Z = (Position - Reference).Normalized();
	X = vec(0.0f, 1.0f, 0.0f).Cross(Z).Normalized();
	Y = Z.Cross(X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// ----------------------------------------------------------------

float * ModuleCamera3D::GetViewMatrixFloat()
{
	return (float*)ViewMatrix.v;
}

const float4x4 ModuleCamera3D::GetViewMatrix()
{
	return ViewMatrix;
}

const float4x4 ModuleCamera3D::GetViewMatrixInverse()
{
	return ViewMatrixInverse;
}

const cCamera * ModuleCamera3D::GetDefaultCamera()
{
	return defaultCamera;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = float4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -X.Dot(Position), -Y.Dot(Position), -Z.Dot(Position), 1.0f);
	ViewMatrixInverse = ViewMatrix.Inverted();
	defaultCamera->frustum.front = -ViewMatrix.Transposed().Row3(2);
	defaultCamera->frustum.up = ViewMatrix.Transposed().Row3(1);
}