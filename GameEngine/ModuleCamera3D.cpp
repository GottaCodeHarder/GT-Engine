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
	CalculateViewMatrix();
	GameObject* tmp = App->scene->CreateGameObject("DefaultCamera");
	defaultCamera = new cCamera(tmp);
	tmp->AddComponent(defaultCamera);

	((cTransform*)defaultCamera->gameObject->FindComponent(TRANSFORM))->positionLocal = { 0.f,0.f,5.f };

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 0.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);
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

	if (camera){
		vec3 newPos(0, 0, 0);
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
						LookAt(vec3(App->editor->selected->aabbBox.CenterPoint().x, App->editor->selected->aabbBox.CenterPoint().y, App->editor->selected->aabbBox.CenterPoint().z));

					}
					else if (((cTransform*)App->editor->selected->FindComponent(TRANSFORM)) != nullptr)
					{
						Position.x = ((cTransform*)App->editor->selected->FindComponent(TRANSFORM))->GetGlobalPos().x + 2;
						Position.y = ((cTransform*)App->editor->selected->FindComponent(TRANSFORM))->GetGlobalPos().y + 2;
						Position.z = ((cTransform*)App->editor->selected->FindComponent(TRANSFORM))->GetGlobalPos().z + 2;
						LookAt(vec3(((cTransform*)App->editor->selected->FindComponent(TRANSFORM))->GetGlobalPos().x, ((cTransform*)App->editor->selected->FindComponent(TRANSFORM))->GetGlobalPos().y, ((cTransform*)App->editor->selected->FindComponent(TRANSFORM))->GetGlobalPos().z));

					}
				}
			}		
		}


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
				float Sensitivity = 0.25f;

				if (dx != 0)
				{
					float DeltaX = (float)dx * Sensitivity;
					
					X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
					Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
					Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
				}

				if (dy != 0)
				{
					float DeltaY = (float)dy * Sensitivity;

					Y = rotate(Y, DeltaY, X);
					Z = rotate(Z, DeltaY, X);

					if (Y.y < 0.0f)
					{
						Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
						Y = cross(Z, X);
					}
				}

				Reference = Position - Reference;
			}
			else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
			{
				float Sensitivity = 0.25f;

				if (App->editor->selected != nullptr)
				{
					if (((cTransform*)App->editor->selected->FindComponent(TRANSFORM)) != nullptr && referenceDone)
					{

						Reference = vec3(((cTransform*)App->editor->selected->FindComponent(TRANSFORM))->GetGlobalPos().x, ((cTransform*)App->editor->selected->FindComponent(TRANSFORM))->GetGlobalPos().y, ((cTransform*)App->editor->selected->FindComponent(TRANSFORM))->GetGlobalPos().z);
						if(((cMesh*)App->editor->selected->FindComponent(MESH)) != nullptr)
							Reference = vec3(App->editor->selected->aabbBox.CenterPoint().x, App->editor->selected->aabbBox.CenterPoint().y, App->editor->selected->aabbBox.CenterPoint().z);
						referenceDone = false;
					}
				}

				Position -= Reference;

				if (dx != 0)
				{
					float DeltaX = (float)dx * Sensitivity;

					X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
					Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
					Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
				}

				if (dy != 0)
				{
					float DeltaY = (float)dy * Sensitivity;

					Y = rotate(Y, DeltaY, X);
					Z = rotate(Z, DeltaY, X);

					if (Y.y < 0.0f)
					{
						Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
						Y = cross(Z, X);
					}
				}

				Position = Reference + Z * length(Position);
			}
		}
	}
	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(vec3 Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
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

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = float4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = ViewMatrix.Inverted();
}