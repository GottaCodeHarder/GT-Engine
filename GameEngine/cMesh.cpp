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
	delete[] resource;
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

uint cMesh::Serialize(char * &buffer)
{
	uint length = 0;
	length += sizeof(int);
	length += sizeof(uint);
	length += resource->numVertex * sizeof(float3);
	length += sizeof(uint);
	length += resource->numIndex * sizeof(uint);
	length += sizeof(uint);
	length += resource->numVertex * sizeof(float3);
	length += resource->numVertex * sizeof(float2);

	buffer = new char[length];
	char* it = buffer;
	
	int iType = (int)componentType::MESH;
	memcpy(it, &iType, sizeof(int));
	it += sizeof(int);

	// Normals
	uint size = resource->numVertex;
	memcpy(it, &size, sizeof(uint));
	it += sizeof(uint);

	memcpy(it, resource->normals.data(), sizeof(float3) * resource->numVertex);
	it += resource->numVertex * sizeof(float3);

	// Index
	size = resource->numIndex;
	memcpy(it, &size, sizeof(uint));
	it += sizeof(uint);

	memcpy(it, resource->index.data(), sizeof(uint) * resource->numIndex);
	it += resource->numIndex * sizeof(uint);

	// Vertex
	size = resource->numVertex;
	memcpy(it, &size, sizeof(uint));
	it += sizeof(uint);

	memcpy(it, resource->vertex.data(), sizeof(float3) * resource->numVertex);
	it += resource->numVertex * sizeof(float3);


	float2* uv = new float2[resource->numVertex];
	glGetBufferSubData(resource->buffUv, NULL, resource->vertex.size() * sizeof(float2), uv);
	memcpy(it, uv, resource->numVertex * sizeof(float2));
	it += resource->numVertex * sizeof(float2);

	delete[] uv;

	return length;
}

uint cMesh::DeSerialize(char * &buffer, GameObject * parent)
{
	char* it = buffer;
	uint ret = 0;

	resource = new ResourceMesh();

	// Normals Size
	uint size;
	memcpy(&size, it, sizeof(uint));
	resource->numVertex = size;
	it += sizeof(uint);
	ret += sizeof(uint);

	// Normals
	if (resource->numVertex > 0)
	{
		resource->normals.reserve(resource->numVertex);
		memcpy(resource->normals.data(), it, resource->numVertex * sizeof(float3));
	}
	it += resource->numVertex * sizeof(float3);
	ret += resource->numVertex * sizeof(float3);

	// Index Size
	memcpy(&size, it, sizeof(uint));
	resource->numIndex = size;
	it += sizeof(uint);
	ret += sizeof(uint);

	// Index
	if (resource->numIndex > 0)
	{
		resource->index.reserve(resource->numIndex);
		memcpy(resource->index.data(), it, resource->numIndex * sizeof(uint));
	}
	it += resource->numIndex * sizeof(uint);
	ret += resource->numIndex * sizeof(uint);

	// Vertex Size
	memcpy(&size, it, sizeof(uint));
	resource->numVertex = size;
	it += sizeof(uint);
	ret += sizeof(uint);

	// Vertex 
	if (resource->numVertex > 0)
	{
		resource->vertex.reserve(resource->numVertex);
		memcpy(resource->vertex.data(), it, resource->numVertex * sizeof(float3));
	}
	it += resource->numVertex * sizeof(float3);
	ret += resource->numVertex * sizeof(float3);

	// UVs
	float2* uv = new float2[resource->numVertex];
	memcpy(&uv, it, resource->numVertex * sizeof(float2));
	it += resource->numVertex * sizeof(float2);
	ret += resource->numVertex * sizeof(float2);

	///////////////////////
	// Vertex
	if (resource->vertex.empty() != false)
	{
		glGenBuffers(1, (GLuint*) &(resource->buffVertex));
		glBindBuffer(GL_ARRAY_BUFFER, resource->buffVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * resource->numVertex, resource->vertex.data(), GL_STATIC_DRAW);

		MYLOG("DeSerialize - Loading %i vertex succesful!", resource->numVertex);
	}
	else
	{
		MYLOG("WARNING, the .GTE scene has 0 vertex!");
	}
	// Normals
	if (resource->normals.empty() != false)
	{
		glGenBuffers(1, (GLuint*) &(resource->buffNormals));
		glBindBuffer(GL_ARRAY_BUFFER, resource->buffNormals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * resource->numVertex, resource->normals.data(), GL_STATIC_DRAW);
	}

	// UVs
	if (uv->Size > NULL)
	{
		glGenBuffers(1, (GLuint*) &(resource->buffUv));
		glBindBuffer(GL_ARRAY_BUFFER, resource->buffUv);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * resource->numVertex * 2, uv, GL_STATIC_DRAW);
	}

	// Index
	glGenBuffers(1, (GLuint*) &(resource->buffIndex));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resource->buffIndex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, resource->numIndex * 3, &resource->index, GL_STATIC_DRAW);
	MYLOG("Importer - Loading %i index succesful!", resource->numIndex * 3);

	return ret;
}
