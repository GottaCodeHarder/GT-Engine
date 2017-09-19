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
	showIntersections = false;
	showFigures = false;

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

		if (showIntersections)
		{
			ImGui::SetNextWindowPos(ImVec2(560, 19));
			Intersections();
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
			showIntersections = !showIntersections;
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

	if (ImGui::Begin("Intersections##Window"),NULL,ImGuiWindowFlags_AlwaysAutoResize)
	{
		math::Sphere sphere;
		math::Capsule capsule;
		math::OBB obb;
		math::Frustum frustum;
		math::Plane plane;
	


		if (ImGui::Button("Add figures"))
		{
			showFigures = true;
		}

		if (showFigures)
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

			ImGui::Text("Sphere pos= 1,1,1 and radius=3");
			ImGui::Text("Capsule at 1,1,1");
			ImGui::Text("OBB at 10,10,10");
			ImGui::Text("Frustum don't know how to create it");
			ImGui::Text("Plane at 1,1,1");
			
		}
	
		ImGui::Separator();
		ImGui::Text("Create a box to intersect");
		char minPointName[50];
		int n = 0;
		float* minPoint = 0;
		sprintf(minPointName,"minPoint##%d", n);
		float a = 0.0f, b = 0.0f, c = 0.0f;
		static float vec3a[3] = { a,b,c };
		ImGui::InputFloat3(minPointName, vec3a);

		if (ImGui::Button("Add AABB at point 0,0,0"))
		{
			math::AABB aabb;
			aabb.minPoint = vec(vec3a);
			aabb.maxPoint = vec(vec3a[0]+1 , vec3a[1] + 1, vec3a[2] + 1);
			box_list.push_back(aabb);
			n++;
		}
		if (ImGui::Button("Add AABB at point 10,10,10"))
		{
			math::AABB aabb;
			aabb.minPoint = vec(vec3a[0] + 10, vec3a[1] + 10, vec3a[2] + 10);
			aabb.maxPoint = vec(vec3a[0] + 11, vec3a[1] + 11, vec3a[2] + 11);
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