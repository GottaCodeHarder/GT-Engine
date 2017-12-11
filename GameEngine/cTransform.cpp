#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GameObject.h"
#include "cMesh.h"
#include "ImGuizmo.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera3D.h"
#include "ModuleRenderer3D.h"
#include "ModuleEditor.h"
#include "ResourceMesh.h"

#include "cTransform.h"

cTransform::cTransform(GameObject* _gameObject) : Component(TRANSFORM, _gameObject)
{
}

cTransform::~cTransform()
{
	SetTransform();
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
	return positionLocal;
}

const float3 cTransform::GetGlobalScale()
{
	if (gameObject->parent != nullptr)
	{
		cTransform* transformParent = (cTransform*)gameObject->parent->FindComponent(TRANSFORM);
		if (transformParent != nullptr)
		{
			float3 parentScale = transformParent->GetGlobalScale();
			return float3(scaleLocal.x * parentScale.x , scaleLocal.y * parentScale.y , scaleLocal.z * parentScale.z);
		}
	}

	return scaleLocal;
}

const Quat cTransform::GetGlobalRoatation()
{
	if (gameObject->parent != nullptr)
	{
		cTransform* transformParent = (cTransform*)gameObject->parent->FindComponent(TRANSFORM);
		if (transformParent != nullptr)
		{
			return rotationLocal * transformParent->GetGlobalRoatation();
		}
	}
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

void cTransform::SetGlobalTransform(float4x4 matrix)
{
	float3 pos;
	float3 scale;
	Quat rot;
	matrix.Decompose(pos, rot, scale);
	SetGlobalPos(pos);
	SetGlobalRot(rot);
	SetGlobalScale(scale);
}

void cTransform::SetGlobalPos(float3 globalPos)
{
	if (gameObject->parent != nullptr)
	{
		positionLocal = globalPos - ((cTransform*)gameObject->parent->FindComponent(TRANSFORM))->GetGlobalPos();
	}
	else
	{
		positionLocal = globalPos;
	}
}

void cTransform::SetGlobalScale(float3 globalScale)
{
	if (gameObject->parent != nullptr)
	{
		float3 parentScale = ((cTransform*)gameObject->parent->FindComponent(TRANSFORM))->GetGlobalScale();
		scaleLocal = float3(globalScale.x / parentScale.x, globalScale.y / parentScale.y, globalScale.z / parentScale.z);
	}
	else
	{
		scaleLocal = globalScale;
	}
}

void cTransform::SetGlobalRot(Quat globalRot)
{
	if (gameObject->parent != nullptr)
	{
		math::Quat parentRot = ((cTransform*)gameObject->parent->FindComponent(TRANSFORM))->GetGlobalRoatation();
		rotationLocal = globalRot / parentRot;
	}
	else
	{
		rotationLocal = globalRot;
	}
}

void cTransform::SetRectTransform(uint _w, uint _h)
{
	w = _w;
	h = _h;
}

void cTransform::SetTransform()
{
	w = 0;
	h = 0;
}

void cTransform::DrawUI()
{
	transformChange = false;

	if (w != 0)
		DrawUIRectTransform();
	else
		DrawUITransform();

	//GUIZMO
	if (App->editor->selected == gameObject && !App->input->GetKey(SDL_SCANCODE_LALT) == KEY_DOWN)
	{
		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
		if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN)
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
			mCurrentGizmoOperation = ImGuizmo::ROTATE;

		if (ImGuizmo::IsUsing())
		{
			transformChange = true;
		}
		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		float4x4 matrix = GetGlobalMatrixTransf().Transposed();
		ImGuizmo::Manipulate(App->camera->GetViewMatrixFloat(), App->renderer3D->projectionMatrix.M, mCurrentGizmoOperation, mCurrentGizmoMode, matrix.ptr(), NULL);
		SetGlobalTransform(matrix.Transposed());
	}
}

void cTransform::DrawUITransform()
{
	if (ImGui::CollapsingHeader("Transform"))
	{
		if (ImGui::TreeNodeEx("Local Information", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Position: %g, %g, %g", positionLocal.x, positionLocal.y, positionLocal.z);
			ImGui::Text("Scale: %g, %g, %g", scaleLocal.x, scaleLocal.y, scaleLocal.z);
			ImGui::Text("Rotation: %g, %g, %g, %g ", rotationLocal.x, rotationLocal.y, rotationLocal.z, rotationLocal.w);
			ImGui::TreePop();
		}
		ImGui::Spacing();
		if (ImGui::TreeNodeEx("Global Information", ImGuiTreeNodeFlags_DefaultOpen))
		{
			float3 globalPos = GetGlobalPos();
			float3 globalScale = GetGlobalScale();
			Quat globalRot = GetGlobalRoatation();
			ImGui::Text("Position: %g, %g, %g", globalPos.x, globalPos.y, globalPos.z);
			ImGui::Text("Scale: %g, %g, %g", globalScale.x, globalScale.y, globalScale.z);
			ImGui::Text("Rotation: %g, %g, %g, %g ", globalRot.x, globalRot.y, globalRot.z, globalRot.w);
			ImGui::TreePop();
		}
		ImGui::Spacing();
		if (gameObject->name != "DefaultCamera")
		{
			if (!gameObject->statiC)
			{
				if (ImGui::TreeNodeEx("Modify Local Position"))
				{
					if (ImGui::DragFloat("x", &positionLocal.x, 0.5f))
					{
						transformChange = true;
					}
					if (ImGui::DragFloat("y", &positionLocal.y, 0.5f)) { transformChange = true; }
					if (ImGui::DragFloat("z", &positionLocal.z, 0.5f)) { transformChange = true; }

					ImGui::TreePop();
				}
				if (ImGui::TreeNodeEx("Modify Local Scale"))
				{
					if (ImGui::DragFloat("x", &scaleLocal.x, 0.5f)) { transformChange = true; }
					if (ImGui::DragFloat("y", &scaleLocal.y, 0.5f)) { transformChange = true; }
					if (ImGui::DragFloat("z", &scaleLocal.z, 0.5f)) { transformChange = true; }
					ImGui::TreePop();
				}
				if (ImGui::TreeNodeEx("Modify Local Rotation"))
				{
					float x = 0.f;
					if (ImGui::DragFloat("x", &x, 0.00005f))
					{
						rotationLocal = rotationLocal * Quat::RotateX(x);
						transformChange = true;
					}
					float y = 0.f;
					if (ImGui::DragFloat("y", &y, 0.00005f))
					{
						rotationLocal = rotationLocal * Quat::RotateY(y);
						transformChange = true;
					}
					float z = 0.f;
					if (ImGui::DragFloat("z", &z, 0.00005f))
					{
						rotationLocal = rotationLocal * Quat::RotateZ(z);
						transformChange = true;
					}
					ImGui::TreePop();
				}
			}
		}
	}
}

void cTransform::DrawUIRectTransform()
{
	if (ImGui::CollapsingHeader("Rect Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::TreeNodeEx("Local Information", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Columns(4, "localColumns", false);
			ImGui::NextColumn();
			ImGui::Text("Pos X");
			if (ImGui::DragFloat("##plx", &positionLocal.x, 0.2f, 0.0f, 0.0f, "%g")) { transformChange = true; }
			ImGui::NextColumn();
			ImGui::Text("Pos Y");
			if (ImGui::DragFloat("##ply", &positionLocal.y, 0.2f, 0.0f, 0.0f, "%g")) { transformChange = true; }
			ImGui::NextColumn();
			ImGui::Text("Pos Z");
			if (ImGui::DragFloat("##plz", &positionLocal.z, 0.2f, 0.0f, 0.0f, "%g")) { transformChange = true; }
			ImGui::NextColumn();

			ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

			ImGui::Columns(4, "scaleRotationColumns", false);
			ImGui::Text("Rotation");
			ImGui::Text("Scale");
			ImGui::NextColumn();
			ImGui::Text("X"); ImGui::SameLine();
			ImGui::Text("%g", rotationLocal.x);
			ImGui::Text("X"); ImGui::SameLine();
			if (ImGui::DragFloat("##slx", &scaleLocal.x, 0.1f, 0.01f, 1000.0f, "%g")) { transformChange = true; }
			ImGui::NextColumn();
			ImGui::Text("Y"); ImGui::SameLine();
			ImGui::Text("%g", rotationLocal.y);
			ImGui::Text("Y"); ImGui::SameLine();
			if (ImGui::DragFloat("##sly", &scaleLocal.y, 0.1f, 0.01f, 1000.0f, "%g")) { transformChange = true; }
			ImGui::NextColumn();
			ImGui::Text("Z"); ImGui::SameLine();
			ImGui::Text("%g", rotationLocal.z);
			ImGui::Text("Z"); ImGui::SameLine();
			if (ImGui::DragFloat("##slz", &scaleLocal.z, 0.1f, 0.01f, 1000.0f, "%g")) { transformChange = true; }
			ImGui::NextColumn();

			ImGui::Columns(1);

			ImGui::TreePop();
		}
		ImGui::Spacing();
		if (ImGui::TreeNodeEx("Global Information"))
		{
			float3 globalPos = GetGlobalPos();
			float3 globalScale = GetGlobalScale();
			Quat globalRot = GetGlobalRoatation();
			ImGui::Text("Position: %g, %g, %g", globalPos.x, globalPos.y, globalPos.z);
			ImGui::Text("Scale: %g, %g, %g", globalScale.x, globalScale.y, globalScale.z);
			ImGui::Text("Rotation: %g, %g, %g, %g ", globalRot.x, globalRot.y, globalRot.z, globalRot.w);
			ImGui::TreePop();
		}
		ImGui::Spacing();
		if (gameObject->name != "DefaultCamera")
		{
			if (!gameObject->statiC)
			{
				if (ImGui::TreeNodeEx("Modify Local Rotation"))
				{
					float x = 0.f;
					if (ImGui::DragFloat("x", &x, 0.00005f))
					{
						rotationLocal = rotationLocal * Quat::RotateX(x);
						transformChange = true;
					}
					float y = 0.f;
					if (ImGui::DragFloat("y", &y, 0.00005f))
					{
						rotationLocal = rotationLocal * Quat::RotateY(y);
						transformChange = true;
					}
					float z = 0.f;
					if (ImGui::DragFloat("z", &z, 0.00005f))
					{
						rotationLocal = rotationLocal * Quat::RotateZ(z);
						transformChange = true;
					}
					ImGui::TreePop();
				}
			}
		}
	}
}

uint cTransform::Serialize(char * &buffer)
{
	uint length = 0;
	length += sizeof(int);
	length += sizeof(float3);
	length += sizeof(float3);
	length += sizeof(Quat);

	buffer = new char[length];
	char* it = buffer;

	int iType = (int)componentType::TRANSFORM;
	memcpy(it, &iType, sizeof(int));
	it += sizeof(int);

	memcpy(it, &positionLocal, sizeof(float3));
	it += sizeof(float3);

	memcpy(it, &scaleLocal, sizeof(float3));
	it += sizeof(float3);

	memcpy(it, &rotationLocal, sizeof(Quat));
	it += sizeof(Quat);

	return length;
}

uint cTransform::DeSerialize(char *& buffer, GameObject * parent)
{
	char* it = buffer;
	uint ret = 0;

	memcpy(&positionLocal, it, sizeof(float3));
	it += sizeof(float3);
	ret += sizeof(float3);

	memcpy(&scaleLocal, it, sizeof(float3));
	it += sizeof(float3);
	ret += sizeof(float3);

	memcpy(&rotationLocal, it, sizeof(Quat));
	it += sizeof(Quat);
	ret += sizeof(Quat);

	if (gameObject->FindComponent(MESH) != nullptr)
	{
		gameObject->aabbBox.SetNegativeInfinity();
		OBB obb = ((cMesh*)gameObject->FindComponent(MESH))->resource->aabbBox.Transform(GetGlobalMatrixTransf());
		
		gameObject->aabbBox.Enclose(obb);
	}

	return ret;
}

