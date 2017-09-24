#include "Globals.h"
#include "Application.h"
#include "ImGui\imgui_impl_sdl_gl3.h"
#include "gl3w\gl3w.h"
#include "Random.h"
#include "MathGeoLib\MathGeoLib.h"

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

	bShowRandom = false;
	bShowExample = false;
	bShowAbout = false;
	bGeometryFigures = false;

	bExit = false;

	active_menu.insert(std::pair<std::string, bool>("Geometry", false));
	active_menu.insert(std::pair<std::string, bool>("Configuration", false));
	active_menu.insert(std::pair<std::string, bool>("Console", false));
	

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
	
	SDL_Event sdlEvent;
	SDL_PollEvent(&sdlEvent);
	if (sdlEvent.type == SDL_KEYDOWN)
	{
		ImGui_ImplSdlGL3_ProcessEvent(&sdlEvent);
		
	}

	{
		if (ImGui::BeginMainMenuBar())
		{
			// FILE
			MenuFile();
			
			// TOOLS
			MenuTools();

			// VIEW
			MenuView();

			// ABOUT
			MenuAbout();

			ImGui::EndMainMenuBar();
		}

		if (bExit) return UPDATE_STOP;
	}

	// Show
	if (active_menu["Geometry"])
	{
		ImGui::SetNextWindowPos(ImVec2(560, 19));
		ViewGeometry();
	}
	if (bShowExample)
	{
		ImGui::ShowTestWindow();
	}
	if (bShowRandom)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 19));
		ToolRandom();
	}
	if (bShowAbout)
	{
		HelpAbout();
	}
	if (active_menu["Configuration"])
	{
		ImGui::SetNextWindowPos(ImVec2(560, 19));
		Configuration();
	}
	if (active_menu["Console"])
	{
		Console();
	}

	ImGui::Render();

	return UPDATE_CONTINUE;
}

// ---------------------------------------------< FILE
void ModuleEditor::MenuFile()
{
	if (ImGui::BeginMenu("File"))
	{

		if (ImGui::MenuItem("Exit", "		Esc"))
		{
			bExit = true;
		}
		ImGui::EndMenu();
	}
}

// ---------------------------------------------< TOOLS
void ModuleEditor::MenuTools()
{
	if (ImGui::BeginMenu("Tools"))
	{
		if (ImGui::SmallButton(" ##Random"))
		{
			bShowRandom = !bShowRandom;
		}
		ImGui::SameLine();

		if (ImGui::MenuItem("Random"))
		{
			bShowRandom = !bShowRandom;
		}

		ImGui::EndMenu();
	}
}

// ---------------------------------------------< VIEW
void ModuleEditor::MenuView()
{
	if (ImGui::BeginMenu("View"))
	{
		std::map<std::string, bool>::iterator it = active_menu.begin();
		int n = 0;
		char check_box_n[50];
		
		for (; it != active_menu.end(); it++)
		{
			if (it != active_menu.begin())
			{
				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();
			}

			sprintf(check_box_n, " ##%d", n);
			if (ImGui::SmallButton(check_box_n))
			{
				it->second = !it->second;
			}
			ImGui::SameLine();

			if (ImGui::MenuItem(it->first.c_str()))
			{
				it->second = !it->second;
			}
			n++;
		}

		ImGui::EndMenu();
	}
}

// ---------------------------------------------< ABOUT
void ModuleEditor::MenuAbout()
{
	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::SmallButton(" ##Example"))
		{
			bShowExample = !bShowExample;
		}
		ImGui::SameLine();

		if (ImGui::MenuItem("Example Window"))
			bShowExample = true;

		ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

		if (ImGui::MenuItem("Documentation"))
			App->RequestBrowser("https://github.com/GottaCodeHarder/GT-Engine/wiki");

		ImGui::Spacing();

		if (ImGui::MenuItem("Download latest"))
			App->RequestBrowser("https://github.com/GottaCodeHarder/GT-Engine/releases");

		ImGui::Spacing();

		if (ImGui::MenuItem("Report a bug"))
			App->RequestBrowser("https://github.com/GottaCodeHarder/GT-Engine/issues");

		ImGui::Spacing();

		if (ImGui::MenuItem("About"))
		{
			bShowAbout = !bShowAbout;
		}

		ImGui::EndMenu();
	}
}

void ModuleEditor::Console()
{
	if (ImGui::Begin("Console"))
	{

		ImGui::Text(console_buffer.c_str());

		ImGui::End();
	}
}

void ModuleEditor::Configuration()
{
	if (ImGui::Begin("Configuration"))
	{

		App->AddImGui();

		ImGui::End();
	}
}

void ModuleEditor::ToolRandom()
{
	if (ImGui::Begin("Random Generator"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)
	{
		const char* RandType[] = { "Float", "Float (0~1)", "Int" };

		if (ImGui::Button("Select.."))
			ImGui::OpenPopup("select");
		ImGui::SameLine();
		ImGui::Text("%s Selected", RandType[iSelectedRand]);
		if (ImGui::BeginPopup("select"))
		{
			for (int i = 0; i < (int)(sizeof(RandType) / sizeof(*RandType)); i++)
				if (ImGui::Selectable(RandType[i]))
					iSelectedRand = i;
			ImGui::EndPopup();
		}

		switch (iSelectedRand)
		{
		case 0:
		{
			ImGui::DragFloat("Maximum", &fRandomMax, 0.1f, fRandomMin);
			ImGui::DragFloat("Minimum", &fRandomMin, 0.1f, NULL, fRandomMax);

			if (ImGui::Button("Generate"))
			{
				Random A = Random();
				if (fRandomMin <= fRandomMax)
					fResult = A.RndFloat(fRandomMin, fRandomMax);
			}
			ImGui::SameLine();
			ImGui::Text("Result: %.2f", fResult); break;
		}
		case 1:
		{
			ImGui::Spacing();
			if (ImGui::Button("Generate"))
			{
				Random A = Random();
				fResult = A.RndFloat();
			}
			ImGui::SameLine();
			ImGui::Text("Result: %.2f", fResult); break;
		}
		case 2:
		{
			ImGui::DragFloat("Maximum", &fRandomMax, 1.0f, fRandomMin, NULL, "%.0f");
			ImGui::DragFloat("Minimum", &fRandomMin, 1.0f, NULL, fRandomMax, "%.0f");

			if (ImGui::Button("Generate"))
			{
				Random A = Random();
				if (fRandomMin <= fRandomMax)
					fResult = A.RndInt(int(fRandomMin), int(fRandomMax));
			}
			ImGui::SameLine();
			ImGui::Text("Result: %i", int(fResult)); break;
		}
		}
	}
	ImGui::End();
}

void ModuleEditor::ViewGeometry()
{

	if (ImGui::Begin("Geometry##Window"), NULL, ImGuiWindowFlags_AlwaysAutoResize)
	{
		math::Sphere sphere;
		math::Capsule capsule;
		math::OBB obb;
		math::Frustum frustum;
		math::Plane plane;

		if (ImGui::Button("Add Figures"))
		{
			bGeometryFigures = true;
		}

		if (bGeometryFigures)
		{
			sphere.pos = vec(1, 1, 1);
			sphere.r = 3.0f;

			LineSegment capsuleSegment;
			capsuleSegment.a = vec(1, 1, 1);
			capsuleSegment.b = vec(1, 1, 1);
			capsule.l = capsuleSegment;
			capsule.r = 3.0f;

			obb.pos = vec(10, 10, 10);
			obb.axis[0] = vec(10, 10, 10);
			obb.axis[1] = vec(10, 10, 10);
			obb.axis[2] = vec(10, 10, 10);
			obb.r = vec(1, 1, 1);

			frustum.SetPos(vec(20, 20, 20));
			frustum.SetFront(vec(20, 20, 20));
			frustum.SetUp(vec(20, 20, 20));
			frustum.SetOrthographic(10, 10);

			plane.d = 4.0f;
			plane.normal = vec(1, 1, 1);
			plane.Translate(vec(1, 1, 1));

			ImGui::Text("Sphere at %.0f,%.0f,%.0f with radius 3", sphere.pos[0], sphere.pos[1], sphere.pos[2]);
			ImGui::Text("Capsule at %.0f,%.0f,%.0f", capsuleSegment.CenterPoint()[0], capsuleSegment.CenterPoint()[1], capsuleSegment.CenterPoint()[2]);
			ImGui::Text("OBB at %.0f,%.0f,%.0f", obb.pos[0], obb.pos[1], obb.pos[2]);
			ImGui::Text("Plane at %.0f,%.0f,%.0f", plane.normal[0], plane.normal[1], plane.normal[2]);
			ImGui::Text("Frustum none");
		}

		ImGui::Separator();	ImGui::Spacing();
		ImGui::Text("AABB Dimensions");
		char minPointName[50];
		char maxPointName[50];
		int n = 0;
		float* minPoint = 0;
		float* maxPoint = 0;
		sprintf(minPointName, "minPoint##%d", n);
		sprintf(maxPointName, "maxPoint##%d", n);

		static float vec3a[3] = { 0.0f, 0.0f, 0.0f };
		static float vec3b[3] = { 0.0f, 0.0f, 0.0f };
		ImGui::DragFloat3(minPointName, vec3a, 0.1f);
		ImGui::DragFloat3(maxPointName, vec3b, 0.1f);

		if (ImGui::Button("Create an AABB"))
		{
			math::AABB aabb;
			aabb.minPoint = vec(vec3a);
			aabb.maxPoint = vec(vec3a[0] + 1, vec3a[1] + 1, vec3a[2] + 1);
			box_list.push_back(aabb);
			n++;
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
		{
			vec3a[0] = 0.0f; vec3b[0] = 0.0f;
			vec3a[1] = 0.0f; vec3b[1] = 0.0f;
			vec3a[2] = 0.0f; vec3b[2] = 0.0f;
		}

		std::list<AABB>::iterator it = box_list.begin();

		for (; it != box_list.end(); it++)
		{
			ImGui::Text("New AABB created that:");
			if (it->Intersects(sphere))
			{
				ImGui::Text("Intersects with the Sphere");
			}
			else
			{
				ImGui::Text("Doesn't intersect with the Sphere");
			}

			if (it->Intersects(capsule))
			{
				ImGui::Text("Intersects with the Capsule");
			}
			else
			{
				ImGui::Text("Doesn't intersect with the Capsule");
			}
			if (it->Intersects(obb))
			{
				ImGui::Text("Intersects with the OBB");
			}
			else
			{
				ImGui::Text("Doesn't intersect with the OBB");
			}
			if (it->Intersects(plane))
			{
				ImGui::Text("Intersects with the Plane");
			}
			else
			{
				ImGui::Text("Doesn't intersect with the Plane");
			}
			ImGui::Separator();
		}
	}

	ImGui::End();
}

void ModuleEditor::HelpAbout()
{
	if (ImGui::Begin("About GT GameEngine"), nullptr, ImGuiWindowFlags_NoScrollbar)
	{
		ImGui::Text("GT ENGINE");
		ImGui::Spacing(); ImGui::Spacing();
		ImGui::Text("GT Engine is a Game Engine for educational purposes. Made by Pau Serra Colomer and Samuel Navarro Sato.");
		ImGui::Spacing();
		ImGui::Text("Using SDL (2.0.3), ImGui (1.50), PhysFS, Bullet (2.84), MathGeoLib (1.5), gl3w, glut (3.7) and parson (latest 2017)");
		ImGui::Spacing();
		ImGui::Text("MIT License. Copyright (c) 2017");
	}

	ImGui::End();
}

void ModuleEditor::AddTextConsole(char* text)
{
	console_buffer.append(text);
}