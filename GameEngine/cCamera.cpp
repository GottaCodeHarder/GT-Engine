#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleScene.h"
#include "ModuleCamera3D.h"
#include "GameObject.h"
#include "cCamera.h"
#include "cTransform.h"
#include "cMesh.h"

cCamera::cCamera(GameObject* _gameObject) : Component(CAMERA, _gameObject)
{
	transform = new cTransform(_gameObject);

	frustum.SetViewPlaneDistances(nearPlane, farPlane);
	frustum.SetWorldMatrix(((cTransform*)gameObject->FindComponent(TRANSFORM))->GetGlobalMatrixTransf().Float3x4Part());
	frustum.SetPerspective(horizontalFOV,verticalFOV);

}

cCamera::~cCamera()
{

}

void cCamera::RealUpdate()
{
	if(drawFrustum)
	App->renderer3D->DrawFrustum(frustum);

	frustum.SetPerspective(horizontalFOV, verticalFOV);
	frustum.SetViewPlaneDistances(nearPlane, farPlane);

	if (transformFrustum)
	{
		frustum.pos = ((cTransform*)gameObject->FindComponent(TRANSFORM))->positionLocal;
		frustum.front = ((cTransform*)gameObject->FindComponent(TRANSFORM))->GetLocalMatrixTransf().Row3(2);
		frustum.up = ((cTransform*)gameObject->FindComponent(TRANSFORM))->GetLocalMatrixTransf().Row3(1);
	}

	if (activeCamera)
	{
		FrustumCulling(App->scene->root);
	}

	if (cameraGlobal)
	{
		TransformCamera();
	}

}

void cCamera::DrawUI()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		if (ImGui::Button("Attach Camera"))
		{
			DesactivateCameraGlobal(App->scene->root);
			cameraGlobal = true;
			AttachCamera();
		}
		if (ImGui::Button("Detach Camera"))
		{
			cameraGlobal = false;
		}
		ImGui::Checkbox("Draw Frustum", &drawFrustum);
		ImGui::SameLine();
		ImGui::Checkbox("Active Camera", &activeCamera);

		if (ImGui::DragFloat("Horizontal FOV", &horizontalFOV, 0.05f)){}
		if (ImGui::DragFloat("Vertical FOV", &verticalFOV, 0.05f)){}
		if (ImGui::DragFloat("Near Plane", &nearPlane, 0.05f)){}
		if (ImGui::DragFloat("Far Plane", &farPlane, 0.05f)){}
	}
}

void cCamera::DesactivateCameraGlobal(GameObject * GO)
{
	for (auto sonsGO : GO->sons)
	{
		cCamera* cameraTMP = ((cCamera*)sonsGO->FindComponent(CAMERA));
		if (cameraTMP->cameraGlobal)
		{
			cameraTMP->cameraGlobal = false;
		}
	}
}

void cCamera::AttachCamera()
{
	cTransform* transformTMP = ((cTransform*)gameObject->FindComponent(TRANSFORM));
	App->camera->Position = vec3(transformTMP->GetGlobalPos().x, transformTMP->GetGlobalPos().y, transformTMP->GetGlobalPos().z);
	//App->camera->LookAt(frustum.front);

}

void cCamera::TransformCamera()
{	
	cTransform* transformTMP = ((cTransform*)gameObject->FindComponent(TRANSFORM));
	transformTMP->positionLocal = float3(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);
	frustum.SetPos(float3(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z));

	frustum.front = ((cTransform*)gameObject->FindComponent(TRANSFORM))->GetLocalMatrixTransf().Row3(2);
	frustum.up = ((cTransform*)gameObject->FindComponent(TRANSFORM))->GetLocalMatrixTransf().Row3(1);

	//frustum.Transform(App->camera->GetViewMatrix());
}

void cCamera::FrustumCulling(GameObject* gameObject)
{
	std::vector<GameObject*> gameObjectColisions = App->scene->quad.GetRoot().Collide<Frustum>(frustum);
	
	for (auto itGameObjects : gameObjectColisions)
	{
		itGameObjects->insideFrustum = true;
	}
	dynamicFrustum(App->scene->root);
}

void cCamera::dynamicFrustum(GameObject * gameObject)
{
	for (auto staticGO : gameObject->sons)
	{
		if (((cMesh*)staticGO->FindComponent(MESH)) != nullptr)
		{
			if (staticGO->aabbBox.Contains(frustum) || staticGO->aabbBox.Intersects(frustum))
			{
				staticGO->insideFrustum = true;
			}
		}
		dynamicFrustum(staticGO);
	}
}
