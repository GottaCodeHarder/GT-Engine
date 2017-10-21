#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GameObject.h"

#include "cTransform.h"

cTransform::cTransform(GameObject* _gameObject) : Component(TRANSFORM, _gameObject)
{
}

cTransform::~cTransform()
{

}

const float3 cTransform::getGlobalPos()
{
	if (gameObject->parent != nullptr)
	{
		cTransform* transformParent = (cTransform*)gameObject->parent->FindComponent(TRANSFORM);
		if (transformParent != nullptr)
		{
			return positionLocal + transformParent->getGlobalPos();
		}
	}
	//rotationLocal = rotationLocal * Quat::RotateX(1.0f);
	return positionLocal;
}

const float4x4 cTransform::GetMatrixTransf()
{
	return float4x4::FromTRS(positionLocal, rotationLocal, scaleLocal);
}

void cTransform::DrawUI()
{
	if (ImGui::CollapsingHeader("Transform")) //AMB EL NOM QUE LI TOQUI
	{

		if (ImGui::TreeNodeEx("Transformation"))
		{
			ImGui::Text("Unknown Position - Not supported");
			ImGui::Text("Unknown Rotation - Not supported");
			ImGui::Text("Unknown Scale    - Not supported");
			ImGui::TreePop();
		}
		ImGui::Spacing();
	}
}

