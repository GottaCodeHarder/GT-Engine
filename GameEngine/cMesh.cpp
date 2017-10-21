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

void cMesh::RealUpdate()
{
}

void cMesh::DrawUI()
{
	if (ImGui::CollapsingHeader("Mesh"))
	{
		if (ImGui::TreeNodeEx("Geometry", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Triangle Count: %i", this->numIndex / 3);
			ImGui::TreePop();
		}
		ImGui::Spacing();

		if (ImGui::Checkbox("AABB box", &aabbActive))
		{

				// Draw Axis Grid
				glLineWidth(2.0f);

				glBegin(GL_LINES);

				glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

				glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
				glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
				glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

				glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

				glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
				glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
				glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
				glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

				glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

				glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
				glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
				glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
				glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

				glEnd();

				glLineWidth(1.0f);
			}

			//glColor3f(color.r, color.g, color.b);


			if (ImGui::Checkbox("Wireframe", &wireframe))
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
		}
	}

