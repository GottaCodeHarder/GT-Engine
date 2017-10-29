#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "cMesh.h"
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
	glDeleteBuffers(1, &buffNormals);
	glDeleteBuffers(1, &buffIndex);
	glDeleteBuffers(1, &buffVertex);
	glDeleteBuffers(1, &buffUv);
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
			ImGui::Text("Vertex Count: %i", this->numVertex);
			ImGui::Text("Triangle Count: %i", this->numIndex / 3);
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

