#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Application.h"
#include "ModuleRenderer3D.h"
#include "GameObject.h"
#include "cCamera.h"
#include "cTransform.h"

cCamera::cCamera(GameObject* _gameObject) : Component(CAMERA, _gameObject)
{
	transform = new cTransform(_gameObject);

	frustum.SetViewPlaneDistances(nearPlane, farPlane);
	frustum.SetWorldMatrix(((cTransform*)gameObject->FindComponent(TRANSFORM))->GetGlobalMatrixTransf().Float3x4Part());
	frustum.SetPerspective(horizontalFOV,verticalFOV);//horizontalFOV, verticalFOV);
	
	//frustum.SetOrthographic(10, 10);

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

}

void cCamera::DrawUI()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::Checkbox("Draw Frustum", &drawFrustum);

		if (ImGui::DragFloat("Horizontal FOV", &horizontalFOV, 0.05f)){}
		if (ImGui::DragFloat("Vertical FOV", &verticalFOV, 0.05f)){}
		if (ImGui::DragFloat("Near Plane", &nearPlane, 0.05f)){}
		if (ImGui::DragFloat("Far Plane", &farPlane, 0.05f)){}

	}
}
