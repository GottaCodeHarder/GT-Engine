#include "Globals.h"
#include "Application.h"
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_sdl_gl3.h"
#include "gl3w\gl3w.h"
#include "Random.h"

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
	bShowRandom = false;
	bShowGeometry = false;
	bGeometryFigures = false;

	bExit = false;

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
	if (bShowGeometry)
	{
		ImGui::SetNextWindowPos(ImVec2(560, 19));
		Intersections();
	}

	if (bShowExample)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 19));
		ImGui::ShowTestWindow();
	}

	if (bShowRandom)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 19));
		ToolRandom();
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
		ImGui::Checkbox("##0", &bShowRandom);
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
		ImGui::Checkbox("##0", &bShowGeometry);
		ImGui::SameLine();

		if (ImGui::MenuItem("Geometry"))
		{
			bShowGeometry = !bShowGeometry;
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Checkbox("##1", &bShowExample);
		ImGui::SameLine();

		if (ImGui::MenuItem("Example Window"))
		{
			bShowExample = !bShowExample;
		}

		ImGui::EndMenu();
	}
}

// ---------------------------------------------< ABOUT
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

void ModuleEditor::ToolRandom()
{
	if (ImGui::Begin("Random Generator"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)
	{
		static float fRandomMin = 0.0f;
		static float fRandomMax = 0.0f;
		static float fResult = 0.0f;
		static int iSelectedRand = 0;

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
			ImGui::DragFloat("Maximum", &fRandomMax, 0.1f);
			ImGui::DragFloat("Minimum", &fRandomMin, 0.1f);

			if (ImGui::Button("Generate"))
			{
				Random A = Random();
				fResult = A.RndFloat(fRandomMin, fRandomMax);
			}
			ImGui::SameLine();
			ImGui::Text("Result: %.2f", fResult); break;
		}
		case 1:
		{
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
			ImGui::DragFloat("Maximum", &fRandomMax, 1.0f);
			ImGui::DragFloat("Minimum", &fRandomMin, 1.0f);

			if (ImGui::Button("Generate"))
			{
				Random A = Random();
				fResult = A.RndInt(int(fRandomMin), int(fRandomMax));
			}
			ImGui::SameLine();
			ImGui::Text("Result: %i", int(fResult)); break;
		}
		}
	}
	ImGui::End();
}

void ModuleEditor::Intersections()
{

	if (ImGui::Begin("Geometry##Window"), NULL, ImGuiWindowFlags_AlwaysAutoResize)
	{
		math::Sphere sphere;
		math::Capsule capsule;
		math::OBB obb;
		math::Frustum frustum;
		math::Plane plane;
	
		/*static bool bCreate = false;

		ImGui::PushID(1);
		ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1 / 7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(1 / 7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(1 / 7.0f, 0.8f, 0.8f));
		if (ImGui::Button("CREATE"))
		{
			bCreate = true;
		}
		ImGui::PopStyleColor(3);
		ImGui::PopID();

		ImGui::SameLine();

		static int selected_figure = -1;
		const char* figures[] = { "Sphere", "Capsule", "OBB", "Frustum", "Plane" };

		if (ImGui::Button("Select.."))
			ImGui::OpenPopup("select");
		ImGui::SameLine();
		ImGui::Text(selected_figure == -1 ? "<None>" : figures[selected_figure]);
		if (ImGui::BeginPopup("select"))
		{
			for (int i = 0; i < (int)(sizeof(figures)/sizeof(*figures)); i++)
				if (ImGui::Selectable(figures[i]))
					selected_figure = i;
			ImGui::EndPopup();
		}*/

		if (ImGui::Button("Add figures"))
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
			obb.axis[1] = vec(10,10,10);
			obb.axis[2] = vec(10,10,10);
			obb.r = vec(1, 1, 1);

			frustum.SetPos(vec(20, 20, 20));
			frustum.SetFront(vec(20, 20, 20));
			frustum.SetUp(vec(20, 20, 20));
			frustum.SetOrthographic(10, 10);
		
			plane.d = 4.0f;
			plane.normal = vec(1, 1, 1);
			plane.Translate(vec(1, 1, 1));

			ImGui::Text("Sphere at 1,1,1 with radius 3");
			ImGui::Text("Capsule at 1,1,1");
			ImGui::Text("OBB at 10,10,10");
			ImGui::Text("Frustum none");
			ImGui::Text("Plane at 1,1,1");
			
		}
	
		ImGui::Separator();
		ImGui::Text("AABB Dimensions");
		char minPointName[50];
		int n = 0;
		float* minPoint = 0;
		sprintf(minPointName,"minPoint##%d", n);

		static float vec3a[3] = { 0.0f, 0.0f, 0.0f };
		ImGui::DragFloat3(minPointName, vec3a, 0.1f);

		if (ImGui::Button("Create an AABB"))
		{
			math::AABB aabb;
			aabb.minPoint = vec(vec3a);
			aabb.maxPoint = vec(vec3a[0]+1 , vec3a[1] + 1, vec3a[2] + 1);
			box_list.push_back(aabb);
			n++;
		}
		
		std::list<AABB>::iterator it = box_list.begin();

		for (; it != box_list.end(); it++)
		{
			ImGui::Text("This is a box");
			if (it->Intersects(sphere))
			{
				ImGui::Text("Intersects with the Sphere");
			}
			else
			{
				ImGui::Text("Doesn't intersects with the Sphere");
			}

			if (it->Intersects(capsule))
			{
				ImGui::Text("Intersects with the Capsule");
			}
			else
			{
				ImGui::Text("Doesn't intersects with the Capsule");
			}
			if (it->Intersects(obb))
			{
				ImGui::Text("Intersects with the OBB");
			}
			else
			{
				ImGui::Text("Doesn't intersects with the OBB");
			}
//			if (it->Intersects(frustum))
//			{
//				ImGui::Text("Intersects with the Frustum");
//			}
//			else
//			{
//				ImGui::Text("Doesn't intersects with the Frustum");
//			}
			if (it->Intersects(plane))
			{
				ImGui::Text("Intersects with the Plane");
			}
			else
			{
				ImGui::Text("Doesn't intersects with the Plane");
			}
			ImGui::Separator();
		}
	}



	ImGui::End();
}