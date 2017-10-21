#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "cMesh.h"

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

void cMesh::DrawUI()
{
	if (ImGui::CollapsingHeader("Mesh")) //AMB EL NOM QUE LI TOQUI
	{
		if (ImGui::TreeNodeEx("Geometry", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Triangle Count: %i", this->numIndex / 3);
			ImGui::TreePop();
		}
		ImGui::Spacing();
	}
}

