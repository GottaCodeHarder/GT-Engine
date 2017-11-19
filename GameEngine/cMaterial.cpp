#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include <commdlg.h>
#include "Application.h"
#include "ModuleScene.h"
#include "ResourceTexture.h"
#include "ModuleResourceManager.h"

#include "cMaterial.h"
#include "GameObject.h"

cMaterial::cMaterial(GameObject* _gameObject) : Component(MATERIAL, _gameObject)
{
	resource = new ResourceTexture();
	resource->path.assign("No path");
}

cMaterial::~cMaterial()
{
}

void cMaterial::DrawUI()
{
	if (ImGui::CollapsingHeader("Material",nullptr , 0 ,true))
	{
		if (ImGui::Button("Load Texture"))
		{
			LoadTexture();
		}
		if (ImGui::TreeNodeEx("Texture", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImVec2 texScreenPos = ImGui::GetCursorScreenPos();
			float texWidth = 200.f;
			float texHeight = 200.f;
			ImTextureID textureID = (ImTextureID)resource->buffTexture;

			ImGui::Text("Currently displaying resized texture,");
			ImGui::Text("hover to zoom");
			ImGui::Text("Dimensions: %.0fx%.0f", resource->imageDimensions.x, resource->imageDimensions.y);
			ImGui::Image(textureID, ImVec2(texWidth, texHeight), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));

			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				float focus_sz = 32.0f;
				float focusX = ImGui::GetMousePos().x - texScreenPos.x - focus_sz * 0.5f; if (focusX < 0.0f) focusX = 0.0f; else if (focusX > texWidth - focus_sz) focusX = texWidth - focus_sz;
				float focusY = ImGui::GetMousePos().y - texScreenPos.y - focus_sz * 0.5f - 34; if (focusY < 0.0f) focusY = 0.0f; else if (focusY > texHeight - focus_sz) focusY = texHeight - focus_sz;
				ImGui::Text("Min: (%.2f, %.2f)", focusX, focusY);
				ImGui::Text("Max: (%.2f, %.2f)", focusX + focus_sz, focusY + focus_sz);
				ImVec2 uv0 = ImVec2((focusX) / texWidth, (focusY) / texHeight);
				ImVec2 uv1 = ImVec2((focusX + focus_sz) / texWidth, (focusY + focus_sz) / texHeight);
				ImGui::Image(textureID, ImVec2(128, 128), uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
				ImGui::EndTooltip();
			}
			ImGui::TreePop();
		}
	}
}

void cMaterial::LoadTexture()
{
	char fileName[1024];
	ZeroMemory(&fileName, sizeof(fileName));

	OPENFILENAME oFileName;
	ZeroMemory(&oFileName, sizeof(oFileName));
	oFileName.lStructSize = sizeof(oFileName);
	oFileName.lpstrFile = fileName;
	oFileName.nMaxFile = 1024;
	oFileName.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	oFileName.lpstrTitle = "Select file to import";
	if (GetOpenFileName(&oFileName) != 0)
	{
		if (resource->buffTexture > 0)
		{
			//glDeleteBuffers(1, &(*it)->buffTexture);
		}
		resource = (ResourceTexture*)App->resourceManager->LoadResourceTexture(fileName, this);
	}
}

uint cMaterial::Serialize(char * &buffer)
{
	uint length = 0;
	length += sizeof(int);
	length += sizeof(uint);
	length += sizeof(float2);
	length += sizeof(uint);
	length += resource->path.length();
	length += sizeof(float3);

	buffer = new char[length];
	char* it = buffer;

	int iType = (int)componentType::MATERIAL;
	memcpy(it, &iType, sizeof(int));
	it += sizeof(int);

	// Texture Buffer
	memcpy(it, &resource->buffTexture, sizeof(uint));
	it += sizeof(uint);

	// Dimensions
	memcpy(it, &resource->imageDimensions, sizeof(float2));
	it += sizeof(float2);

	// Path Length
	uint size = resource->path.length();
	memcpy(it, &size, sizeof(uint));
	it += sizeof(uint);

	// Path
	memcpy(it, resource->path.data(), size);
	it += size;

	// Color
	memcpy(it, &resource->color, sizeof(float3));
	it += sizeof(float3);

	return length;
}

uint cMaterial::DeSerialize(char *& buffer, GameObject * parent)
{
	char* it = buffer;
	uint ret = 0;

	// Texture Buffer
	memcpy(&resource->buffTexture, &it, sizeof(uint));
	it += sizeof(uint);
	ret += sizeof(uint);

	// Dimensions
	memcpy(&resource->imageDimensions, &it, sizeof(float2));
	it += sizeof(float2);
	ret += sizeof(float2);

	// Path Length
	uint size;
	memcpy(&size, &it, sizeof(uint));
	it += sizeof(uint);
	ret += sizeof(uint);

	// Path
	resource->path.assign(&it[0], size);
	it += size;
	ret += size;

	// Color
	memcpy(&resource->color, &it, sizeof(float3));
	it += sizeof(float3);
	ret += sizeof(float3);

	return ret;
}

