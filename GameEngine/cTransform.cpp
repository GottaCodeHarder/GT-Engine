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
#include "GTInterface\GTInterface.h"
#include "UIComponents.h"

#include "cTransform.h"

Transform::Transform(GameObject* _gameObject) : cTransform(_gameObject, NORMAL)
{
	positionLocal = float3(0.0f);
	scaleLocal = float3(1.0f);
	rotationLocal = Quat::identity;
}

Transform::~Transform()
{}

const float3 Transform::GetGlobalPos()
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

const float3 Transform::GetGlobalScale()
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

const Quat Transform::GetGlobalRoatation()
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

const float4x4 Transform::GetLocalMatrixTransf()
{
	float3 pos = positionLocal;
	float3 scale = float3(1.0f);
	Quat rot = rotationLocal;

	float4x4 ret = float4x4::FromTRS(pos, rot, scale);

	return ret;
}

const float4x4 Transform::GetGlobalMatrixTransf()
{
	float3 scale = float3(1.0f);
	return float4x4::FromTRS(GetGlobalPos(), GetGlobalRoatation(), scale);
}

void Transform::SetGlobalTransform(float4x4 matrix)
{
	float3 pos;
	float3 scale;
	Quat rot;
	matrix.Decompose(pos, rot, scale);
	SetGlobalPos(pos);
	SetGlobalRot(rot);
	SetGlobalScale(scale);
}

void Transform::SetGlobalPos(float3 globalPos)
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

void Transform::SetGlobalScale(float3 globalScale)
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

void Transform::SetGlobalRot(Quat globalRot)
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

const float3 Transform::GetLocalPos()
{
	return positionLocal;
}

const float3 Transform::GetLocalScale()
{
	return scaleLocal;
}

const Quat Transform::GetLocalRot()
{
	return rotationLocal;
}

void Transform::SetLocalPos(float3 pos)
{
	positionLocal = pos;
}

void Transform::SetLocalScale(float3 scale)
{
	scaleLocal = scale;
}

void Transform::SetLocalRot(Quat rot)
{
	rotationLocal = rot;
}

void Transform::DrawUI()
{
	transformChange = false;

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

uint Transform::Serialize(char * &buffer)
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

uint Transform::DeSerialize(char *& buffer, GameObject * parent)
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





RectTransform::RectTransform(GameObject* _gameObject) :
	cTransform(_gameObject, RECT_TRANS), ui(nullptr)
{}

RectTransform::~RectTransform()
{}

void RectTransform::SetRectSource(cUI* reference)
{
	ui = reference;
}

void SetGlobalPos()
{}

const float3 RectTransform::GetGlobalPos()
{
	if (!HasUIRect())
		return float3(0.f);

	GTI::RectTransform* rect = ui->getTransform();

	if (gameObject->parent != nullptr)
	{
		cTransform* transformParent = (cTransform*)gameObject->parent->FindComponent(TRANSFORM);
		if (transformParent != nullptr)
		{
			return  rect->positionLocal + transformParent->GetGlobalPos();
		}
	}

	return rect->positionLocal;
}

const float3 RectTransform::GetGlobalScale()
{
	if (!HasUIRect())
		return float3(0.f);

	GTI::RectTransform* rect = ui->getTransform();

	if (gameObject->parent != nullptr)
	{
		cTransform* transformParent = (cTransform*)gameObject->parent->FindComponent(TRANSFORM);
		if (transformParent != nullptr)
		{
			float3 parentScale = transformParent->GetGlobalScale();
			return float3(rect->scaleLocal.x * parentScale.x, rect->scaleLocal.y * parentScale.y, rect->scaleLocal.z * parentScale.z);
		}
	}

	return rect->scaleLocal;
}

const Quat RectTransform::GetGlobalRoatation()
{
	if (!HasUIRect())
		return Quat::identity;

	GTI::RectTransform* rect = ui->getTransform();

	if (gameObject->parent != nullptr)
	{
		cTransform* transformParent = (cTransform*)gameObject->parent->FindComponent(TRANSFORM);
		if (transformParent != nullptr)
		{
			return rect->rotationLocal * transformParent->GetGlobalRoatation();
		}
	}
	return rect->rotationLocal;
}

const float4x4 RectTransform::GetLocalMatrixTransf()
{
	if (!HasUIRect())
		return float4x4(Quat::identity);

	GTI::RectTransform* rect = ui->getTransform();

	return float4x4::FromTRS(rect->positionLocal, rect->rotationLocal, rect->scaleLocal);
}

const float4x4 RectTransform::GetGlobalMatrixTransf() 
{
	if (!HasUIRect())
		return float4x4(Quat::identity);

	GTI::RectTransform* rect = ui->getTransform();

	return float4x4::FromTRS(GetGlobalPos(), GetGlobalRoatation(), rect->scaleLocal);
}

void RectTransform::SetGlobalTransform(float4x4 matrix)
{
	if (!HasUIRect())
		return;

	float3 pos;
	float3 scale;
	Quat rot;
	matrix.Decompose(pos, rot, scale);
	SetGlobalPos(pos);
	SetGlobalRot(rot);
	SetGlobalScale(scale);
}

void RectTransform::SetGlobalPos(float3 globalPos) 
{
	if (!HasUIRect())
		return;

	GTI::RectTransform* rect = ui->getTransform();

	rect->positionLocal = globalPos;

	if (gameObject->parent != nullptr)
		rect->positionLocal -= ((cTransform*)gameObject->parent->FindComponent(TRANSFORM))->GetGlobalPos();
}

void RectTransform::SetGlobalScale(float3 globalScale)
{
	if (!HasUIRect())
		return;

	GTI::RectTransform* rect = ui->getTransform();

	if (gameObject->parent != nullptr)
	{
		float3 parentScale = ((cTransform*)gameObject->parent->FindComponent(TRANSFORM))->GetGlobalScale();
		rect->scaleLocal = float3(globalScale.x / parentScale.x, globalScale.y / parentScale.y, globalScale.z / parentScale.z);
	}
	else
	{
		rect->scaleLocal = globalScale;
	}
}
void RectTransform::SetGlobalRot(Quat globalRot)
{
	if (!HasUIRect())
		return;

	GTI::RectTransform* rect = ui->getTransform();

	if (gameObject->parent != nullptr)
		rect->rotationLocal = globalRot / ((cTransform*)gameObject->parent->FindComponent(TRANSFORM))->GetGlobalRoatation();
	else
		rect->rotationLocal = globalRot;
}

const float3 RectTransform::GetLocalPos()
{
	if (!HasUIRect())
		return float3(0.f);

	return ui->getTransform()->positionLocal;
}

const float3 RectTransform::GetLocalScale()
{
	if (!HasUIRect())
		return float3(0.f);

	return ui->getTransform()->scaleLocal;
}

const Quat RectTransform::GetLocalRot()
{
	if (!HasUIRect())
		return Quat::identity;

	return ui->getTransform()->rotationLocal;
}

void RectTransform::SetLocalPos(float3 pos)
{
	if (!HasUIRect())
		return;

	GTI::RectTransform* rect = ui->getTransform();

	rect->positionLocal = pos;
}

void RectTransform::SetLocalScale(float3 scale)
{
	if (!HasUIRect())
		return;

	GTI::RectTransform* rect = ui->getTransform();

	rect->scaleLocal = scale;
}

void RectTransform::SetLocalRot(Quat rot)
{
	if (!HasUIRect())
		return;

	GTI::RectTransform* rect = ui->getTransform();

	rect->rotationLocal = rot;
}

uint RectTransform::Serialize(char* &buffer)
{
	return 0;
}

uint RectTransform::DeSerialize(char* &buffer, GameObject* parent)
{
	return 0;
}

bool RectTransform::HasUIRect() const
{
	bool ret = false;

	if (ui != nullptr)
		ret = ui->getTransform() != nullptr;

	return ret;
}

void RectTransform::DrawUI()
{
	if (!HasUIRect())
		return;

	GTI::RectTransform* rect = ui->getTransform();

	if (ImGui::CollapsingHeader("Rect Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::TreeNodeEx("Local Information", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Columns(4, "localColumns", false);
			ImGui::NextColumn();
			ImGui::Text("Pos X");
			ImGui::DragFloat("##PosDragX", &rect->positionLocal.x, 0.25f, -0.0f, 0.0f, "%g");
			ImGui::Text("Width");
			ImGui::Text("%.1f", rect->w);
			ImGui::NextColumn();
			ImGui::Text("Pos Y");
			ImGui::DragFloat("##PosDragY", &rect->positionLocal.y, 0.25f, -0.0f, 0.0f, "%g");
			ImGui::Text("Height");
			ImGui::Text("%.1f", rect->h);
			ImGui::NextColumn();
			ImGui::Text("Pos Z");
			ImGui::DragFloat("##PosDragZ", &rect->positionLocal.z, 0.25f, -0.0f, 0.0f, "%g");
			ImGui::NextColumn();

			ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

			ImGui::Columns(4, "scaleRotationColumns", false);
			ImGui::Text("Rotation");
			ImGui::Text("Scale");
			ImGui::NextColumn();
			ImGui::Text("X");	ImGui::SameLine();
			float rotation = fRotationX;
			if (ImGui::DragFloat("##RotDragX", &rotation, 0.1f, 0.0f, 0.0f, "%g"))
			{
				if (rotation != fRotationX)
				{
					rect->rotationLocal = rect->rotationLocal * Quat::RotateX(rotation - fRotationX);
					fRotationX = rotation;
				}
			}
			ImGui::Text("X");	ImGui::SameLine();
			ImGui::DragFloat("##ScaDragX", &rect->scaleLocal.x, 0.1f, 0.1f, 1000000.0f, "%g");
			ImGui::NextColumn();
			ImGui::Text("Y");	ImGui::SameLine();
			rotation = fRotationY;
			if (ImGui::DragFloat("##RotDragY", &rotation, 0.1f, 0.0f, 0.0f, "%g"))
			{
				if (rotation != fRotationY)
				{
					rect->rotationLocal = rect->rotationLocal * Quat::RotateY(rotation - fRotationY);
					fRotationY = rotation;
				}
			}
			ImGui::Text("Y");	ImGui::SameLine();
			ImGui::DragFloat("##ScaDragY", &rect->scaleLocal.y, 0.1f, 0.1f, 1000000.0f, "%g");
			ImGui::NextColumn();
			ImGui::Text("Z");	ImGui::SameLine();
			rotation = fRotationZ;
			if (ImGui::DragFloat("##RotDragZ", &rotation, 0.1f, 0.0f, 0.0f, "%g"))
			{
				if (rotation != fRotationZ)
				{
					rect->rotationLocal = rect->rotationLocal * Quat::RotateZ(rotation - fRotationZ);
					fRotationZ = rotation;
				}
			}
			ImGui::Text("Z");	ImGui::SameLine();
			ImGui::DragFloat("##ScaDragZ", &rect->scaleLocal.z, 0.1f, 0.1f, 1000000.0f, "%g");
			ImGui::NextColumn();

			ImGui::Columns(1);

			ImGui::TreePop();
			ImGui::Spacing();
		}
		ImGui::Spacing();
		if (ImGui::TreeNodeEx("Global Information"))
		{
			float3 globalPos = GetGlobalPos();
			float3 globalScale = GetGlobalScale();
			Quat globalRot = GetGlobalRoatation();
			ImGui::Text("Position:	%g, %g, %g", globalPos.x, globalPos.y, globalPos.z);
			ImGui::Text("Scale:		%g, %g, %g", globalScale.x, globalScale.y, globalScale.z);
			ImGui::Text("Rotation:	%g, %g, %g, %g ", globalRot.x, globalRot.y, globalRot.z, globalRot.w);
			ImGui::TreePop();
		}
		ImGui::Spacing();
	}

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