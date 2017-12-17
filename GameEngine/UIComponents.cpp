#include "UIComponents.h"
#include "Application.h"

#include "ModuleGUI.h"
#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"

#include <gl/GL.h>
#include <gl/GLU.h>
#include <Windows.h>



std::vector<std::string> cUI::GetFunctionsName()

{
	std::vector<std::string> ret;

	for (auto it : GTI::GTInterface.boolFunctions.FunctionsMap)
		ret.push_back(it.first);

	for (auto it : GTI::GTInterface.floatFunctions.FunctionsMap)
		ret.push_back(it.first);

	for (auto it : GTI::GTInterface.stringFunctions.FunctionsMap)
		ret.push_back(it.first);

	return ret;
}

void cUI::SetFunctions(std::string target, GTI::UIElement * element)
{

	{
		for (auto it : GTI::GTInterface.boolFunctions.FunctionsMap)
		{
			if (it.first == target)
			{
				element->floatEmitter.Register(it.second);
			}
		}

		for (auto it : GTI::GTInterface.floatFunctions.FunctionsMap)
		{
			if (it.first == target)
			{
				element->boolEmitter.Register(it.second);
			}
		}

		for (auto it : GTI::GTInterface.stringFunctions.FunctionsMap)
		{
			if (it.first == target)
			{
				element->stringEmitter.Register(it.second);
			}
		}
	}
}

GTI::RectTransform * cUI::getTransform() const
{
	return GetUI() != nullptr ? GetUI()->transform : nullptr;
}

void cImage::DrawUI()
{
	if (ImGui::CollapsingHeader("Image", nullptr, 0, true))
	{
		bool bAlpha = (image->blendType == GTI::TransparencyType::ALPHA_TEST);
		bool bBlend = (image->blendType == GTI::TransparencyType::BLEND);

		if (ImGui::Checkbox("ALPHA_TEST", &bAlpha))
			bBlend = false;
		if (ImGui::Checkbox("BLEND", &bBlend))
			bAlpha = false;

		if (bAlpha)
			image->blendType = GTI::TransparencyType::ALPHA_TEST;
		else if (bBlend)
			image->blendType = GTI::TransparencyType::BLEND;
		else
			image->blendType = GTI::TransparencyType::NONE;

		if (image->blendType == GTI::TransparencyType::ALPHA_TEST)
		{
			ImGui::Text("Alpha");
			ImGui::SameLine();
			ImGui::DragFloat("##dragFloat", &image->alpha, 0.05f, 0.0f, 1.0f, "%g");
		}

		if (image->blendType == GTI::TransparencyType::BLEND)
		{
			if (ImGui::Button("Select Blend Type"))
				ImGui::OpenPopup("selectBlend");
			ImGui::SameLine();
			ImGui::Text("Current: %i", image->blend);
			if (ImGui::BeginPopup("selectBlend"))
			{
				if (ImGui::Selectable("0 Zero##Blend"))
					image->blend = GL_ZERO;
				if (ImGui::Selectable("1 One##Blend"))
					image->blend = GL_ONE;
				if (ImGui::Selectable("768 Src_Color##Blend"))
					image->blend = GL_SRC_COLOR;
				if (ImGui::Selectable("769 One_Minus_Src_Color##Blend"))
					image->blend = GL_ONE_MINUS_SRC_COLOR;
				if (ImGui::Selectable("770 Src_Alpha##Blend"))
					image->blend = GL_SRC_ALPHA;
				if (ImGui::Selectable("771 One_Minus_Src_Alpha##Blend"))
					image->blend = GL_ONE_MINUS_SRC_ALPHA;
				if (ImGui::Selectable("772 Dst_Alpha##Blend"))
					image->blend = GL_DST_ALPHA;
				if (ImGui::Selectable("773 One_Minus_Dst_Alpha##Blend"))
					image->blend = GL_ONE_MINUS_DST_ALPHA;
				if (ImGui::Selectable("774 Dst_Color##Blend"))
					image->blend = GL_DST_COLOR;
				if (ImGui::Selectable("775 One_Minus_Dst_Color##Blend"))
					image->blend = GL_ONE_MINUS_DST_COLOR;
				if (ImGui::Selectable("32769 Constant_Color##Blend"))
					image->blend = GL_CONSTANT_COLOR;
				if (ImGui::Selectable("32770 One_Minus_Constant_Color##Blend"))
					image->blend = GL_ONE_MINUS_CONSTANT_COLOR;
				if (ImGui::Selectable("32771 Constant_Alpha##Blend"))
					image->blend = GL_CONSTANT_ALPHA;
				if (ImGui::Selectable("32772 One_Minus_Constant_Alpha##Blend"))
					image->blend = GL_ONE_MINUS_CONSTANT_ALPHA;
				ImGui::EndPopup();
			}
		}

		ImGui::Text("Currently using:");	ImGui::SameLine();
		if (ImGui::Button("Change"))
		{
			char path[1024];
			ZeroMemory(&path, sizeof(path));

			OPENFILENAME oFileName;
			ZeroMemory(&oFileName, sizeof(oFileName));
			oFileName.lStructSize = sizeof(oFileName);
			oFileName.lpstrFile = path;
			oFileName.nMaxFile = 1024;
			oFileName.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			oFileName.lpstrTitle = "Select file to import";

			if (GetOpenFileName(&oFileName) != 0)
			{
			image->buffTexture = App->userinterface->LoadUIImage(path);
			}
		}
		ImGui::Image((ImTextureID)image->buffTexture, ImVec2(150, 150), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));

		ImGui::Spacing();
		//Color RGB
	}
}

void cButton::DrawUI()

{
	if (ImGui::CollapsingHeader("Button", nullptr, 0, true))
	{
		if (ImGui::Button("Set Function"))
			ImGui::OpenPopup("selectFunction");
		if (ImGui::BeginPopup("selectFunction"))
		{
			for (auto it : GetFunctionsName())
			{
				if (ImGui::Selectable(it.c_str()))
					SetFunctions(it, button);
			}
			ImGui::EndPopup();
		}
		int bValue = button->valueBool;

		ImGui::Text("These parameters will be sent to all Functions\nselected above.");

		ImGui::Columns(3, "buttonValues", false);
		ImGui::Spacing();
		ImGui::Text("Float Value"); ImGui::Spacing();
		ImGui::Text("Bool Value");
		ImGui::NextColumn();
		ImGui::DragFloat("##dragFValue", &button->valueFloat, 1.0f, 0.0f, 0.0f, "%g");
		if(ImGui::DragInt("##dragBValue", &bValue, 1.0f, 0.0f, 1.0f, "%g"))
			button->valueBool = (bool)bValue;
		ImGui::NextColumn();
		ImGui::Columns(1);
	}
	ImGui::Spacing();
}

void cCheckbox::DrawUI()
{
	if (ImGui::CollapsingHeader("Checkbox", nullptr, 0, true))
	{
		if (ImGui::Button("Set Function"))
			ImGui::OpenPopup("selectFunction");
		if (ImGui::BeginPopup("selectFunction"))
		{
			for (auto it : GetFunctionsName())
			{
				if (ImGui::Selectable(it.c_str()))
					SetFunctions(it, checkbox);
			}
			ImGui::EndPopup();
		}
		int bValue = checkbox->value;

		ImGui::Text("This parameter will be sent to all Functions\nselected above.");

		ImGui::Columns(3, "buttonValues", false);
		ImGui::Spacing();
		ImGui::Text("Bool Value");
		ImGui::NextColumn();
		if (ImGui::DragInt("##dragValue", &bValue, 1.0f, 0.0f, 1.0f, "%g"))
			checkbox->value = (bool)bValue;
		ImGui::NextColumn();
		ImGui::Columns(1);
	}
}