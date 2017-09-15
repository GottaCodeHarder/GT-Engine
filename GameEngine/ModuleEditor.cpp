#include "Globals.h"
#include "Application.h"
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_sdl_gl3.h"
#include "gl3w\gl3w.h"

ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleEditor::~ModuleEditor()
{

}

// Load assets
bool ModuleEditor::Start()
{
	LOG("Loading Editor");
	bool ret = true;

	show_test_window = true;
	done = false;

	gl3wInit();
	ImGui_ImplSdlGL3_Init(App->window->GetWindow());

	return ret;

}

// Load assets
bool ModuleEditor::CleanUp()
{
	LOG("Unloading Editor stuff");

	ImGui_ImplSdlGL3_Shutdown();

	return true;
}

// Update
update_status ModuleEditor::Update(float dt)
{
	ImGui_ImplSdlGL3_NewFrame(App->window->GetWindow());

	{
		ImGui::Begin("Window", &show_test_window, ImGuiWindowFlags_NoMove && ImGuiWindowFlags_NoResize);

		float f = 0.1f;
		ImGui::Text("Hello, world!");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		ImGui::ColorEdit3("clear color", (float*)&ImColor(114, 144, 154));
		if (ImGui::Button("Test Window")) show_test_window ^= 1;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
		ImGui::End();
	}

	ImGui::Render();

	return UPDATE_CONTINUE;
}