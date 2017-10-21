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
	if (ImGui::CollapsingHeader("Transform"))
	{
		if (ImGui::TreeNodeEx("Transformation",ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Position: %d, %d, %d", positionLocal.x,positionLocal.y, positionLocal.z);
			ImGui::Text("Scale: %d, %d, %d", scaleLocal.x, scaleLocal.y, scaleLocal.z);
			ImGui::Text("Rotation: %d, %d, %d, %d ", rotationLocal.x, rotationLocal.y, rotationLocal.z, rotationLocal.w);
			ImGui::TreePop();
		}
		ImGui::Spacing();
	}
}

