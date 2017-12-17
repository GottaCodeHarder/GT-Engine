#include "ModuleGUI.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "ModuleFileSystem.h"
#include "cTransform.h"
#include "UIComponents.h"

#include "Random.h"

#include "GTInterface/GTInterface.h"

ModuleGUI::ModuleGUI(Application * app, bool start_enabled) : Module(app, start_enabled)
{
	uiComponents.clear();
	canvas = nullptr;
}

bool ModuleGUI::Init()
{
	return true;
}
bool ModuleGUI::Start()
{
	GTI::GTInterface.boolFunctions.AddFunction<Application>("Set VSync", App, &Application::SetVSync);
	return true;
}
update_status ModuleGUI::Update(float dt)
{
	if (justOnce)
	{
		/*GameObject* background_go = App->scene->CreateGameObject("Background Image", true, canvas->gameObject, true);
		cImage* image = new cImage(background_go);
		image->GetUI()->blendType = GTI::TransparencyType::BLEND;
		std::string name = "background.png";
		std::string path = App->fileSystem->GetExecutableDirectory() + name;
		image->GetUI()->buffTexture = GTI::LoadTexture(path.c_str(), image->GetUI()->transform);
		image->GetUI()->transform->scaleLocal = float3(3.0f, 3.0f, 1.0f);
		((cTransform*)background_go->FindComponent(TRANSFORM))->SetRectSource(image);*/

		// Button
		GameObject* button_go = App->scene->CreateGameObject("Start Button", true, background_go, true);
		cButton* button = new cButton(button_go, image->GetUI());
		button->GetUI()->draggable = true;
		button->GetUI()->blendType = GTI::TransparencyType::BLEND;

		name = "start.png";
		path = App->fileSystem->GetExecutableDirectory() + name;
		button->GetUI()->buffTexture = GTI::LoadTexture(path.c_str(), button->GetUI()->transform);
		button->GetUI()->valueFloat = 3.0f;
		button->GetUI()->valueBool = 3.0f;
		button->GetUI()->transform->scaleLocal = float3(0.4f, 0.3f, 1.0f);
		button->GetUI()->transform->positionLocal = float3(0.0f, 0.0f, 5.0f);
		button->GetUI()->transform->rotationLocal = Quat::identity;
		((cTransform*)button_go->FindComponent(TRANSFORM))->SetRectSource(button);

		GTI::GTInterface.floatFunctions.AddFunction<GTI::UIElement>("Fade Background Image", image->GetUI(), &GTI::UIElement::StartFade);
		GTI::GTInterface.floatFunctions.AddFunction<GTI::UIElement>("Fade Start Button", button->GetUI(), &GTI::UIElement::StartFade);
		button->SetFunctions("Fade Background Image", button->GetUI());
		button->SetFunctions("Fade Start Button", button->GetUI());

		GameObject* F1_go = App->scene->CreateGameObject("Background F1", true, canvas->gameObject, true);
		cImage* imageA = new cImage(F1_go);
		imageA->GetUI()->blendType = GTI::TransparencyType::BLEND;
		name = "background.png";
		path = App->fileSystem->GetExecutableDirectory() + name;
		//imageA->GetUI()->SetActive(false);
		imageA->GetUI()->buffTexture = GTI::LoadTexture(path.c_str(), imageA->GetUI()->transform);
		imageA->GetUI()->transform->scaleLocal = float3(3.0f, 3.0f, 1.0f);
		((cTransform*)F1_go->FindComponent(TRANSFORM))->SetRectSource(image);

		justOnce = false;
	}
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		if (!canvas)
			CreateCanvas();

		AddUILabel(nullptr, "Sample Text");
		AddUICheckBox();
	}

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{

	}

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		GTI::UIElement* tmp = canvas->GetUI();
		GTI::RectTransform* rect = tmp->transform;
		rect->scaleLocal.x *= 0.5;
	}
	if (App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN)
	{
		GTI::UIElement* tmp = canvas->GetUI();
		GTI::RectTransform* rect = tmp->transform;
		rect->scaleLocal.y *= 0.5;
	}
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		GTI::UIElement* tmp = canvas->GetUI();
		GTI::RectTransform* rect = tmp->transform;
		rect->scaleLocal.x *= 1.5;
	}
	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
	{
		GTI::UIElement* tmp = canvas->GetUI();
		GTI::RectTransform* rect = tmp->transform;
		rect->scaleLocal.y *= 1.5;
	}

	return UPDATE_CONTINUE;
}

bool ModuleGUI::CleanUp()
{
	return true;
}

void ModuleGUI::AddImGui()
{

}

void ModuleGUI::CreateCanvas()
{
	GameObject* canvas_go = App->scene->CreateGameObject("Canvas", true, nullptr, true);
	canvas = new cCanvas(canvas_go);
	((cTransform*)canvas_go->FindComponent(TRANSFORM))->SetRectSource(canvas);
}

cImage* ModuleGUI::AddUIImage(GameObject* parent_go, const char* path)
{
	if (!canvas)
		CreateCanvas();

	GameObject* image_go = nullptr;
	cImage* image = nullptr;

	char name[64] = "";
	int id = rand.RndInt(0, 100000);
	sprintf_s(name, sizeof(name), "Image\#\#%i", id);

	if (parent_go == nullptr)
	{
		image_go = App->scene->CreateGameObject(name, true, canvas->gameObject, true);
		image = new cImage(image_go, path);
	}
	else
	{
		cUI* parentUI = (cUI*)parent_go->FindComponent(UI);
		if (parentUI == nullptr)
		{
			image_go = App->scene->CreateGameObject(name, true, canvas->gameObject, true);
			image = new cImage(image_go, path);
		}
		else
		{
			image_go = App->scene->CreateGameObject(name, true, parent_go, true);
			image = new cImage(image_go, path, parentUI->GetUI());
		}
	}

	((cTransform*)image_go->FindComponent(TRANSFORM))->SetRectSource(image);

	sprintf_s(name, sizeof(name), "Active %s", name);
	GTI::GTInterface.boolFunctions.AddFunction<GTI::UIElement>(name, image->GetUI(), &GTI::UIElement::SetActive);
	sprintf_s(name, sizeof(name), "Fade %s", name);
	GTI::GTInterface.floatFunctions.AddFunction<GTI::UIElement>(name, image->GetUI(), &GTI::UIElement::StartFade);

	return image;
}

cButton* ModuleGUI::AddUIButton(GameObject* parent_go)
{
	if (!canvas)
		CreateCanvas();

	GameObject* button_go = nullptr;
	cButton* button = nullptr;

	char name[64] = "";
	int id = rand.RndInt(0, 100000);
	sprintf_s(name, sizeof(name), "Image\#\#%i", id);

	if (parent_go == nullptr)
	{
		button_go = App->scene->CreateGameObject(name, true, canvas->gameObject, true);
		button = new cButton(button_go);
	}
	else
	{
		cUI* parentUI = (cUI*)parent_go->FindComponent(UI);
		if (parentUI == nullptr)
		{
			button_go = App->scene->CreateGameObject(name, true, canvas->gameObject, true);
			button = new cButton(button_go);
		}
		else
		{
			button_go = App->scene->CreateGameObject(name, true, parent_go, true);
			button = new cButton(button_go, parentUI->GetUI());
		}
	}

	((cTransform*)button_go->FindComponent(TRANSFORM))->SetRectSource(button);

	sprintf_s(name, sizeof(name), "Active Button#%i", id);
	GTI::GTInterface.boolFunctions.AddFunction<GTI::UIElement>(name, button->GetUI(), &GTI::UIElement::SetActive);
	sprintf_s(name, sizeof(name), "Fade Button#%i", id);
	GTI::GTInterface.floatFunctions.AddFunction<GTI::UIElement>(name, button->GetUI(), &GTI::UIElement::StartFade);

	return button;
}

cLabel* ModuleGUI::AddUILabel(GameObject* parent_go, const char* text, uint size, const char* path)
{
	if (!canvas)
		CreateCanvas();

	GameObject* label_go = nullptr;
	cLabel* label = nullptr;

	char name[64] = "";
	int id = rand.RndInt(0, 100000);
	sprintf_s(name, sizeof(name), "Label\#\#%i", id);

	if (parent_go == nullptr)
	{
		label_go = App->scene->CreateGameObject(name, true, canvas->gameObject, true);
		label = new cLabel(label_go, text);
	}
	else
	{
		cUI* parentUI = (cUI*)parent_go->FindComponent(UI);
		if (parentUI == nullptr)
		{
			label_go = App->scene->CreateGameObject(name, true, canvas->gameObject, true);
			label = new cLabel(label_go, text);
		}
		else
		{
			label_go = App->scene->CreateGameObject(name, true, parent_go, true);
			label = new cLabel(label_go, text, parentUI->GetUI());
		}
	}

	((cTransform*)label_go->FindComponent(TRANSFORM))->SetRectSource(label);

	if (path != nullptr)
		label->SetFont(GTI::GTInterface.LoadFont(path, size), size);

	((cTransform*)label_go->FindComponent(TRANSFORM))->SetRectSource(label);
	return label;
}

cCheckbox* ModuleGUI::AddUICheckBox(GameObject* parent_go)
{
	if (!canvas)
		CreateCanvas();

	GameObject* checkbox_go = nullptr;
	cCheckbox* checkbox = nullptr;

	char name[64] = "";
	int id = rand.RndInt(0, 100000);
	sprintf_s(name, sizeof(name), "Checkbox\#\#%i", id);

	if (parent_go == nullptr)
	{
		checkbox_go = App->scene->CreateGameObject(name, true, canvas->gameObject, true);
		checkbox = new cCheckbox(checkbox_go, true);
	}
	else
	{
		cUI* parentUI = (cUI*)parent_go->FindComponent(UI);
		if (parentUI == nullptr)
		{
			checkbox_go = App->scene->CreateGameObject(name, true, canvas->gameObject, true);
			checkbox = new cCheckbox(checkbox_go, true);
		}
		else
		{
			checkbox_go = App->scene->CreateGameObject(name, true, parent_go, true);
			checkbox = new cCheckbox(checkbox_go, true, parentUI->GetUI());
		}
	}

	((cTransform*)checkbox_go->FindComponent(TRANSFORM))->SetRectSource(checkbox);

	sprintf_s(name, sizeof(name), "Active Button#%i", id);
	GTI::GTInterface.boolFunctions.AddFunction<GTI::UIElement>(name, checkbox->GetUI(), &GTI::UIElement::SetActive);

	return checkbox;
}

const char* ModuleGUI::AddUIFont(const char* path)
{
	return GTI::GTInterface.LoadFont(path, 24);
}

uint ModuleGUI::LoadUIImage(char * path, cUI* component)
{
	if (component != nullptr)
		return GTI::LoadTexture(path, component->getTransform());

	return  GTI::LoadTexture(path, nullptr);
}