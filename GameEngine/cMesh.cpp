#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "cMesh.h"
#include "ResourceMesh.h"
#include "cTransform.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "GameObject.h"

cMesh::cMesh(GameObject* _gameObject) : Component(MESH, _gameObject)
{
	type = MESH;
}

cMesh::~cMesh()
{

}

void cMesh::RealUpdate()
{

}

void cMesh::DrawUI()
{
	if (ImGui::CollapsingHeader("Mesh"))
	{
		if (ImGui::TreeNodeEx("Geometry", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Vertex Count: %i", resource->numVertex);
			ImGui::Text("Triangle Count: %i", resource->numIndex / 3);
			ImGui::TreePop();
		}
		ImGui::Spacing();

		if (ImGui::Checkbox("AABB box", &aabbActive)){}
		if (aabbActive)
		{
			DrawAABB(gameObject->aabbBox);
		}
		if (ImGui::TreeNodeEx("AABB information", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Center Point: %f, %f, %f", gameObject->aabbBox.CenterPoint().x, gameObject->aabbBox.CenterPoint().y, gameObject->aabbBox.CenterPoint().z);
			//ImGui::Text("Scale: %f, %f, %f", scaleLocal.x, scaleLocal.y, scaleLocal.z);
			//ImGui::Text("Rotation: %f, %f, %f, %f ", rotationLocal.x, rotationLocal.y, rotationLocal.z, rotationLocal.w);
			ImGui::TreePop();
		}
		ImGui::Spacing();

		//glColor3f(color.r, color.g, color.b);
	}
}

void cMesh::DrawAABB(AABB aabbBox)
{
	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.minPoint.x, aabbBox.minPoint.y, aabbBox.minPoint.z), btVector3(aabbBox.minPoint.x, aabbBox.maxPoint.y, aabbBox.minPoint.z), btVector3(0, 0.7f, 0));
	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.minPoint.x, aabbBox.minPoint.y, aabbBox.minPoint.z), btVector3(aabbBox.maxPoint.x, aabbBox.minPoint.y, aabbBox.minPoint.z), btVector3(0, 0.7f, 0));
	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.minPoint.x, aabbBox.minPoint.y, aabbBox.minPoint.z), btVector3(aabbBox.minPoint.x, aabbBox.minPoint.y, aabbBox.maxPoint.z), btVector3(0, 0.7f, 0));

	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.maxPoint.x, aabbBox.minPoint.y, aabbBox.maxPoint.z), btVector3(aabbBox.minPoint.x, aabbBox.minPoint.y, aabbBox.maxPoint.z), btVector3(0, 0.7f, 0));
	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.maxPoint.x, aabbBox.minPoint.y, aabbBox.maxPoint.z), btVector3(aabbBox.maxPoint.x, aabbBox.minPoint.y, aabbBox.minPoint.z), btVector3(0, 0.7f, 0));

	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.maxPoint.x, aabbBox.maxPoint.y, aabbBox.maxPoint.z), btVector3(aabbBox.maxPoint.x, aabbBox.minPoint.y, aabbBox.maxPoint.z), btVector3(0, 0.7f, 0));
	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.maxPoint.x, aabbBox.maxPoint.y, aabbBox.maxPoint.z), btVector3(aabbBox.maxPoint.x, aabbBox.maxPoint.y, aabbBox.minPoint.z), btVector3(0, 0.7f, 0));
	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.maxPoint.x, aabbBox.maxPoint.y, aabbBox.maxPoint.z), btVector3(aabbBox.minPoint.x, aabbBox.maxPoint.y, aabbBox.maxPoint.z), btVector3(0, 0.7f, 0));

	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.minPoint.x, aabbBox.maxPoint.y, aabbBox.maxPoint.z), btVector3(aabbBox.minPoint.x, aabbBox.minPoint.y, aabbBox.maxPoint.z), btVector3(0, 0.7f, 0));
	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.minPoint.x, aabbBox.maxPoint.y, aabbBox.maxPoint.z), btVector3(aabbBox.minPoint.x, aabbBox.maxPoint.y, aabbBox.minPoint.z), btVector3(0, 0.7f, 0));

	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.maxPoint.x, aabbBox.maxPoint.y, aabbBox.minPoint.z), btVector3(aabbBox.minPoint.x, aabbBox.maxPoint.y, aabbBox.minPoint.z), btVector3(0, 0.7f, 0));
	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.maxPoint.x, aabbBox.maxPoint.y, aabbBox.minPoint.z), btVector3(aabbBox.maxPoint.x, aabbBox.minPoint.y, aabbBox.minPoint.z), btVector3(0, 0.7f, 0));
}

uint cMesh::Serialize(char * buffer)
{
	uint length = 0;
	length += sizeof(int);
	length += sizeof(uint);
	length += resource->normals.size() * sizeof(float3);
	length += sizeof(uint);
	length += resource->vertex.size() * sizeof(float3);
	length += sizeof(uint);
	length += resource->index.size() * sizeof(uint);

	buffer = new char[length];
	char* it = buffer;

	memcpy(it, &length, sizeof(uint));
	it += sizeof(uint);
	
	memcpy(it, &type, sizeof(uint));
	it += sizeof(uint);

	int size = resource->normals.size();
	memcpy(it, &size, sizeof(uint));
	it += sizeof(uint);

	memcpy(it, resource->normals.data(), sizeof(float3) * resource->normals.size());
	it += resource->normals.size() * sizeof(float3);

	size = resource->vertex.size();
	memcpy(it, &size, sizeof(uint));
	it += sizeof(uint);

	memcpy(it, resource->vertex.data(), sizeof(float3) * resource->vertex.size());
	it += resource->vertex.size() * sizeof(float3);

	size = resource->index.size();
	memcpy(it, &size, sizeof(uint));
	it += sizeof(uint);

	memcpy(it, resource->index.data(), sizeof(uint) * resource->index.size());
	it += resource->index.size() * sizeof(uint);

	return uint();
}