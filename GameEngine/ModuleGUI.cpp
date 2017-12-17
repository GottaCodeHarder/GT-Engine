#include "ModuleGUI.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
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
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		AddUIImage(nullptr);

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

bool ModuleGUI::AddUIImage(char* path)
{
	if (!canvas)
		CreateCanvas();

	char name[64] = "";
	sprintf_s(name, sizeof(name), "Image\#\#%i", rand.RndInt(0, 100000));

	GameObject* image_go = App->scene->CreateGameObject(name, true, canvas->gameObject, true);
	cImage* image = new cImage(image_go, path);
	((cTransform*)image_go->FindComponent(TRANSFORM))->SetRectSource(image);

	sprintf_s(name, sizeof(name), "Active %s", name);
	GTI::GTInterface.boolFunctions.AddFunction<GTI::UIElement>(name, image->GetUI(), &GTI::UIElement::SetActive);
	sprintf_s(name, sizeof(name), "Fade %s", name);
	GTI::GTInterface.floatFunctions.AddFunction<GTI::UIElement>(name, image->GetUI(), &GTI::UIElement::StartFade);

	return image->GetUI()->buffTexture != 0;
}

bool ModuleGUI::AddUIButton(char* path)
{
	if (!canvas)
		CreateCanvas();

	char name[64] = "";
	int id = rand.RndInt(0, 100000);
	sprintf_s(name, sizeof(name), "Button\#\#%i", id);

	GameObject* button_go = App->scene->CreateGameObject(name, true, canvas->gameObject, true);
	cButton* button = new cButton(button_go);
	((cTransform*)button_go->FindComponent(TRANSFORM))->SetRectSource(button);

	button->GetUI()->buttonImageBuffers.push_back(button->GetButtonImages());

	sprintf_s(name, sizeof(name), "Active Button#%i", id);
	GTI::GTInterface.boolFunctions.AddFunction<GTI::UIElement>(name, button->GetButtonImages(), &GTI::UIElement::SetActive);

	return button->GetButtonImages() != 0;
}

bool ModuleGUI::AddUIFont(char * path)
{
	// TODO - Ventana contextual pidiendo el nombre
	// El nombre del font es el nombre del archivo .ttf
	// se saca del path y te lo devuelve la funcion LoadFont(...)
	// si la fuente se carga correctamente

	if (!canvas)
		CreateCanvas();

	std::string font = GTI::GTInterface.LoadFont(path, 24);

	char name[64] = "";
	sprintf_s(name, sizeof(name), "Label\#\#%i", rand.RndInt(0, 100000));

	GameObject* label_go = App->scene->CreateGameObject(name, true, canvas->gameObject, true);
	cLabel* label = new cLabel(label_go, "Sample Text", font.c_str(), 24);
	((cTransform*)label_go->FindComponent(TRANSFORM))->SetRectSource(label);
	return label->GetUI()->buffTexture != 0;
}

uint ModuleGUI::LoadUIImage(char * path)
{
	return GTI::LoadTexture(path, nullptr); 
}