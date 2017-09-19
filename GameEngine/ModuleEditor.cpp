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
	MYLOG("Loading Editor");
	bool ret = true;

	bShowExample = false;

	gl3wInit();
	ImGui_ImplSdlGL3_Init(App->window->GetWindow());
	
	return ret;

}

// Load assets
bool ModuleEditor::CleanUp()
{
	MYLOG("Unloading Editor stuff");

	ImGui_ImplSdlGL3_Shutdown();

	return true;
}

// Update
update_status ModuleEditor::Update(float dt)
{
	ImGui_ImplSdlGL3_NewFrame(App->window->GetWindow());

	{
		if (ImGui::BeginMainMenuBar())
		{
			// FILE
			if(MenuFile())
			{
				return UPDATE_STOP;
			}

			// TOOLS
			MenuTools();

			// VIEW
			MenuView();

			// ABOUT
			MenuAbout();

			ImGui::EndMainMenuBar();
		}

		if (bShowExample)
		{
			ImGui::SetNextWindowPos(ImVec2(0, 19));
			ImGui::ShowTestWindow();
		}

	}

	ImGui::Render();

	return UPDATE_CONTINUE;
}

bool ModuleEditor::MenuFile()
{
	if (ImGui::BeginMenu("File"))
	{

		if (ImGui::MenuItem("Exit", "		Esc"))
		{
			return true;
		}
		ImGui::EndMenu();
	}
	return false;
}

void ModuleEditor::MenuTools()
{
	if (ImGui::BeginMenu("Tools"))
	{
		if (ImGui::MenuItem("Timer"))
		{

		}
		ImGui::EndMenu();
	}
}

void ModuleEditor::MenuView()
{
	if (ImGui::BeginMenu("View"))
	{
		ImGui::Checkbox("", &bShowExample);
		ImGui::SameLine();

		if (ImGui::MenuItem("Example Window"))
		{
			bShowExample = !bShowExample;
		}

		if (ImGui::MenuItem("Intersections"))
		{
			Intersections();
		}

		ImGui::EndMenu();
	}
}

void ModuleEditor::MenuAbout()
{
	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("Repository"))
		{
			ShellExecute(NULL, "open", "https://github.com/GottaCodeHarder/GT-Engine", NULL, NULL, SW_SHOWNORMAL);
		}

		if (ImGui::MenuItem("Readme"))
		{
			ShellExecute(NULL, "open", "https://github.com/GottaCodeHarder/GT-Engine/blob/master/README.md", NULL, NULL, SW_SHOWNORMAL);
		}

		if (ImGui::MenuItem("About"))
		{

		}

		ImGui::EndMenu();
	}
}

void ModuleEditor::Intersections()
{

}