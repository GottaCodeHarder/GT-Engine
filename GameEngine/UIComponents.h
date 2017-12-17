#include "Component.h"
#include "Random.h"
#include "GTInterface\GTInterface.h"


class GameObject;

class cUI : public Component
{
public:
	cUI(GameObject* _gameObject) : Component(UI, _gameObject) { }
	virtual ~cUI(){}

	virtual void Save(JSON_Object &object) const {};
	virtual void Load(const JSON_Object &object) {};

	std::vector<std::string> GetFunctionsName() 
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
	
	void SetFunctions(std::string target, GTI::UIElement* element)
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

	GTI::RectTransform* getTransform() const
	{
		return GetUI() != nullptr ? GetUI()->transform : nullptr;
	}
	virtual GTI::UIElement* GetUI() const { return nullptr; }
};

class cCanvas : public cUI
{
public:
	cCanvas(GameObject* _gameObject) : cUI(_gameObject)
	{
		canvas = (GTI::Canvas*)GTI::GTInterface.GetRoot();

		_gameObject->AddComponent(this);
		//if (!canvas) LOG(GTI::GetLastError().c_str());
	}
	~cCanvas() {};

	GTI::UIElement* GetUI() const { return canvas; }

private:
	GTI::Canvas* canvas;
};

class cImage : public cUI
{
public:
	int unique;

	cImage(GameObject* _gameObject, char* path = nullptr, GTI::UIElement* parent = nullptr) : cUI(_gameObject)
	{
		image = GTI::GTInterface.CreateImage(parent, path);

		_gameObject->AddComponent(this);
		// Load Default Texture & set Transform's width and height
		//_gameObject->SetRectTransform(1, 1);
	}
	~cImage()
	{
		delete image;
	}

	GTI::Image* GetUI() const { return image; }

	void DrawUI()
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
				// TODO
				/*char path[1024];
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
				}*/
			}
			ImGui::Image((ImTextureID)image->buffTexture, ImVec2(150, 150), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));

			//Color RGB
		}
	}

private:
	GTI::Image* image;
};


class cLabel: public cUI
{
public:
	cLabel(GameObject* _gameObject, const char* _text, const char* _font, uint size = 14, SDL_Color color = { 255, 255, 255 }, GTI::UIElement* parent = nullptr) : cUI(_gameObject)
	{
		std::string text = (_text == nullptr) ? "" : _text;
		std::string font = (_font == nullptr) ? "" : _font;
		label = GTI::GTInterface.CreateLabel(text, font, size, color, parent);

		_gameObject->AddComponent(this);
	}

	GTI::UIElement* GetUI() const { return label; }

	void DrawUI()
	{
		//Source
		//Color RGB
		//Alpha
	}

private:
	GTI::Label* label;
};


class cButton : public cUI
{
public:
	cButton(GameObject* _gameObject, GTI::UIElement* parent = nullptr) : cUI(_gameObject)
	{
		button = GTI::GTInterface.CreateButton(parent);
		_gameObject->AddComponent(this);
	}
	~cButton()
	{
		delete button;
	}
	GTI::Button* GetUI() const { return button; }
	GTI::Image* GetButtonImages() const { return imageA; }

	void DrawUI()
	{
		if (ImGui::CollapsingHeader("Button", nullptr, 0, true))
		{
			if (ImGui::Button("Set Function"))
				ImGui::OpenPopup("selectFunction");
			ImGui::SameLine();
			if (ImGui::BeginPopup("selectFunction"))
			{
				for (auto it : GetFunctionsName())
				{
					if (ImGui::Selectable(it.c_str()))
						SetFunctions(it, button);
				}
				ImGui::EndPopup();
			}
		}
		/*
		GTI::Image* image = imageA;

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
				// TODO
				/*char path[1024];
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
		*/
	}

private:
	GTI::Button* button;

	GTI::Image* imageA;
};

class cCheckbox : public cUI
{
public:
	cCheckbox(GameObject* _gameObject, GTI::UIElement* parent = nullptr) : cUI(_gameObject) { _gameObject->AddComponent(this);  }
	~cCheckbox()
	{
		delete checkbox;
	}
	GTI::UIElement* GetUI() const { return checkbox; }

	void DrawUI()
	{
		//Source
	}

private:
	GTI::Checkbox* checkbox;
};

class cInput : public cUI
{
public:
	cInput(GameObject* _gameObject) : cUI(_gameObject)
	{
		_gameObject->AddComponent(this);
		//GetUI()->stringEmitter.Register<GTI::Label>(GetUI(), &GTI::Label::SetText);
	}
	~cInput()
	{
		delete input;
	}
	GTI::UIElement* GetUI() const { return input; }

	void DrawUI()
	{

	}

private:
	GTI::Input* input;
};

