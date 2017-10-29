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

	frustum.SetViewPlaneDistances(1, 100);
	frustum.SetWorldMatrix(((cTransform*)gameObject->FindComponent(TRANSFORM))->GetGlobalMatrixTransf().Float3x4Part());
	frustum.SetPerspective(2, 1);//horizontalFOV, verticalFOV);
	
	//frustum.SetOrthographic(10, 10);

}

cCamera::~cCamera()
{

}

void cCamera::RealUpdate()
{
	App->renderer3D->DrawFrustum(frustum);
}

void cCamera::DrawUI()
{
	if (ImGui::CollapsingHeader("Camera"))
	{

	}
}
