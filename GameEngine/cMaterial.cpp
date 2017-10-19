#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "cMaterial.h"

cMaterial::cMaterial()
{
	type = MATERIAL;
}

cMaterial::~cMaterial()
{
	glDeleteBuffers(1, &buffNormals);
	glDeleteBuffers(1, &buffIndex);
	glDeleteBuffers(1, &buffVertex);
	glDeleteBuffers(1, &buffUv);

	glDeleteBuffers(1, &buffTexture);
}

void cMaterial::DrawUI()
{
	//	ImGui::Begin("Properties", 0, flag);
	//	{
	//		int a = 1;
	//		if (meshes.empty())
	//		{
	//			ImGui::Text("You must first load a Scene!");
	//		}
	//		else
	//		{
	//			for (std::vector<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); it++)
	//			{
	//				char title[30];
	//				sprintf_s(title, sizeof(title), "Mesh %i", a);
	//				if (ImGui::CollapsingHeader(title, 0))
	//				{
	//					if (ImGui::TreeNodeEx("Transformation"))
	//					{
	//						ImGui::Text("Unknown Position - Not supported");
	//						ImGui::Text("Unknown Rotation - Not supported");
	//						ImGui::Text("Unknown Scale    - Not supported");
	//						ImGui::TreePop();
	//					}
	//					ImGui::Spacing();
	//					if (ImGui::TreeNodeEx("Geometry", ImGuiTreeNodeFlags_DefaultOpen))
	//					{
	//						ImGui::Text("Triangle Count: %i", (*it)->numIndex / 3);
	//						ImGui::TreePop();
	//					}
	//					ImGui::Spacing();
	//					if (ImGui::TreeNodeEx("Texture", ImGuiTreeNodeFlags_DefaultOpen))
	//					{
	//						ImVec2 texScreenPos = ImGui::GetCursorScreenPos();
	//						float texWidth = 200.f;
	//						float texHeight = 200.f;
	//						ImTextureID textureID = (ImTextureID)(*it)->buffTexture;
	//
	//						ImGui::Text("Currently displaying resized texture, hover to zoom");
	//						ImGui::Text("Dimensions: %.0fx%.0f", importer.imageDimensions.x, importer.imageDimensions.y);
	//						ImGui::Image(textureID, ImVec2(texWidth, texHeight), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
	//
	//						if (ImGui::IsItemHovered())
	//						{
	//							ImGui::BeginTooltip();
	//							float focus_sz = 32.0f;
	//							float focusX = ImGui::GetMousePos().x - texScreenPos.x - focus_sz * 0.5f; if (focusX < 0.0f) focusX = 0.0f; else if (focusX > texWidth - focus_sz) focusX = texWidth - focus_sz;
	//							float focusY = ImGui::GetMousePos().y - texScreenPos.y - focus_sz * 0.5f; if (focusY < 0.0f) focusY = 0.0f; else if (focusY > texHeight - focus_sz) focusY = texHeight - focus_sz;
	//							ImGui::Text("Min: (%.2f, %.2f)", focusX, focusY);
	//							ImGui::Text("Max: (%.2f, %.2f)", focusX + focus_sz, focusY + focus_sz);
	//							ImVec2 uv0 = ImVec2((focusX) / texWidth, (focusY) / texHeight);
	//							ImVec2 uv1 = ImVec2((focusX + focus_sz) / texWidth, (focusY + focus_sz) / texHeight);
	//							ImGui::Image(textureID, ImVec2(128, 128), uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
	//							ImGui::EndTooltip();
	//						}
	//						ImGui::TreePop();
	//					}
	//				}
	//				a++;
	//			}
	//		}
	//	}
	//
	//	ImGui::End();
}
