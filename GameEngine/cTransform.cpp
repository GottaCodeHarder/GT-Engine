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

const float3 cTransform::GetGlobalPos()
{

	if (gameObject->parent != nullptr)
	{
		cTransform* transformParent = (cTransform*)gameObject->parent->FindComponent(TRANSFORM);
		if (transformParent != nullptr)
		{
			return positionLocal + transformParent->GetGlobalPos();
		}
	}
	//rotationLocal = rotationLocal * Quat::RotateX(1.0f);
	return float3(0.f, 0.f, 0.f);
}

const float3 cTransform::GetGlobalScale()
{
	if (gameObject->parent != nullptr)
	{
		cTransform* transformParent = (cTransform*)gameObject->parent->FindComponent(TRANSFORM);
		if (transformParent != nullptr)
		{
			return scaleLocal + transformParent->GetGlobalScale();
		}
	}
	//rotationLocal = rotationLocal * Quat::RotateX(1.0f);
	return float3(0.f,0.f,0.f);
}

const Quat cTransform::GetGlobalRoatation()
{
	if (gameObject->parent != nullptr)
	{
		cTransform* transformParent = (cTransform*)gameObject->parent->FindComponent(TRANSFORM);
		if (transformParent != nullptr)
		{
			//return rotationLocal + transformParent->GetRoatation();
		}
	}
	//rotationLocal = rotationLocal * Quat::RotateX(1.0f);
	return rotationLocal;
}

const float4x4 cTransform::GetLocalMatrixTransf()
{
	return float4x4::FromTRS(positionLocal, rotationLocal, scaleLocal);
}

const float4x4 cTransform::GetGlobalMatrixTransf()
{
	return float4x4::FromTRS(GetGlobalPos(), GetGlobalRoatation(), scaleLocal);
}

void cTransform::DrawUI()
{
	if (ImGui::CollapsingHeader("Transform"))
	{
		if (ImGui::TreeNodeEx("Local Information", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Position: %f, %f, %f", positionLocal.x, positionLocal.y, positionLocal.z);
			ImGui::Text("Scale: %f, %f, %f", scaleLocal.x, scaleLocal.y, scaleLocal.z);
			ImGui::Text("Rotation: %f, %f, %f, %f ", rotationLocal.x, rotationLocal.y, rotationLocal.z, rotationLocal.w);
			ImGui::TreePop();
		}
		ImGui::Spacing();
		if (ImGui::TreeNodeEx("Global Information", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Position: %f, %f, %f", GetGlobalPos().x, GetGlobalPos().y, GetGlobalPos().z);
			ImGui::Text("Scale: %f, %f, %f", scaleLocal.x, scaleLocal.y, scaleLocal.z);
			ImGui::Text("Rotation: %f, %f, %f, %f ", rotationLocal.x, rotationLocal.y, rotationLocal.z, rotationLocal.w);
			ImGui::TreePop();
		}
		ImGui::Spacing();
	}
}

