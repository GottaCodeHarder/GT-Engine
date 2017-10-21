#include "Globals.h"
#include "Application.h"

#include "ImGui/imgui_impl_sdl_gl3.h"
#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "GameObject.h"
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
	
	InitStyles();
	SetStyle(mintStyle);
	blueStyle.active = arcStyle.active = gteStyle.active = false;
	
	//Insert File Menu
	file_menu.insert(std::pair<std::string, bool>("Exit(esc)", false));
	file_menu.insert(std::pair<std::string, bool>("Load File", false));
	file_menu.insert(std::pair<std::string, bool>("Reset Scene", false));

	//Insert Tools Menu
	tools_menu.insert(std::pair<std::string, bool>("Style Editor", false));
	tools_menu.insert(std::pair<std::string, bool>("Random", false));
	tools_menu.insert(std::pair<std::string, bool>("Geometry", false));

	//Insert Window Menu

	window_menu.insert(std::pair<std::string, bool>("Configuration", true));
	window_menu.insert(std::pair<std::string, bool>("Console", true));
	window_menu.insert(std::pair<std::string, bool>("Heriarchy", true));
	window_menu.insert(std::pair<std::string, bool>("Properties", true));

	//Insert Menu About
	help_menu.insert(std::pair<std::string, bool>("About", false));
	help_menu.insert(std::pair<std::string, bool>("ImGui Example Window", false));
	help_menu.insert(std::pair<std::string, bool>("Documentation", false));
	help_menu.insert(std::pair<std::string, bool>("Report a Bug", false));
	help_menu.insert(std::pair<std::string, bool>("Latest Release", false));


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

			// WINDOW
			MenuWindow();

			// ABOUT
			MenuHelp();

			ImGui::EndMainMenuBar();
		}
	}

	//Show File Menu
	if (file_menu["Load File"])
	{
		LoadFile();
	}

	if (file_menu["Exit(esc)"])
	{
		return UPDATE_STOP;
	}

	//Show Tools Menu
	if (tools_menu["Style Editor"])
	{
		StyleEditor();
	}	
	if (tools_menu["Random"])
	{
		ToolRandom();
	}
	if (tools_menu["Geometry"])
	{
		ImGui::SetNextWindowPos(ImVec2(0, 19));
		ViewGeometry();
	}

	// Show Window Menu
	if (window_menu["Configuration"])
	{
		Configuration();
	}
	if (window_menu["Heriarchy"])
	{
		Heriarchy();
	}
	if (window_menu["Properties"])
	{
		Properties();
	}
	if (window_menu["Console"])
	{
		ImGui::SetNextWindowPos(ImVec2(0, 738));
		Console();
	}

	//Show Help Menu
	if (help_menu["ImGui Example Window"])
	{
		ImGui::ShowTestWindow();
	}
	if (help_menu["About"])
	{
		HelpAbout();
	}
	if (help_menu["Documentation"])
	{
		App->RequestBrowser("https://github.com/GottaCodeHarder/GT-Engine/wiki");
		std::map<std::string, bool>::iterator it = help_menu.find("Documentation");
		if (it->second == true)
		{
			it->second = false;
		}
	}
	if (help_menu["Latest Release"])
	{
		App->RequestBrowser("https://github.com/GottaCodeHarder/GT-Engine/releases");
		std::map<std::string, bool>::iterator it = help_menu.find("Latest Release");
		if (it->second == true)
		{
			it->second = false;
		}
	}
	if (help_menu["Report a Bug"])
	{
		App->RequestBrowser("https://github.com/GottaCodeHarder/GT-Engine/issues");
		std::map<std::string, bool>::iterator it = help_menu.find("Report a Bug");
		if (it->second == true)
		{
			it->second = false;
		}
	}


	return UPDATE_CONTINUE;
}

void ModuleEditor::AddImGui()
{
	if (ImGui::CollapsingHeader("Editor"))
	{
		if (ImGui::Checkbox("Set Blue Style", &blueStyle.active))
		{
			SetStyle(blueStyle);
			blueStyle.active = true;
			mintStyle.active = arcStyle.active = gteStyle.active = false;
		}

		if (ImGui::Checkbox("Set Forest Style", &mintStyle.active))
		{
			SetStyle(mintStyle);
			mintStyle.active = true;
			blueStyle.active = arcStyle.active = gteStyle.active = false;
		}

		if (ImGui::Checkbox("Set Arc Style", &arcStyle.active))
		{
			SetStyle(arcStyle);
			arcStyle.active = true;
			mintStyle.active = blueStyle.active = gteStyle.active = false;
		}

		/*if (ImGui::Checkbox("Set GT Engine Style", &gteStyle.active))
		{
			SetStyle(gteStyle);
			gteStyle.active = true;
			mintStyle.active = blueStyle.active = arcStyle.active = false;
		}*/
	}
}

void ModuleEditor::Draw()
{
	ImGui::Render();
}

void ModuleEditor::ConfigApplication(bool &bFreeze, Timer &startUp, float &capFramerate, float &millisec, float &fps)
{
	if (!bFreeze)
	{
		for (int i = 0; i <= 99; i++)
		{
			if (i == 99)
			{
				App->msArr[i] = (startUp.Read() - millisec);
				App->fpsArr[i] = capFramerate;
			}
			else
			{
				App->msArr[i] = App->msArr[i + 1];
				App->fpsArr[i] = App->fpsArr[i + 1];
			}
		}
	}

	if (ImGui::CollapsingHeader("Application"))
	{
		static char input[100];
		int size = sizeof(input) / sizeof(char);
		sprintf_s(input, size, "%s", App->name.c_str());

		if (ImGui::InputText("App Name", input, size))
		{
			App->name.assign(input);
			App->window->SetTitle(input);
		}

		sprintf_s(input, size, "%s", App->organization.c_str());

		if (ImGui::InputText("Organization", input, sizeof(input) / sizeof(char)))
		{
			App->organization.assign(input);
		}

		ImGui::PushItemWidth(250);
		ImGui::SliderFloat("Max FPS", &capFramerate, 20.0f, 144.0f, "%.1f");
		fps = (capFramerate > 0) ? 1000 / capFramerate : 0;

		millisec = startUp.Read();

		char title[25];
		sprintf_s(title, 25, "Framerate %.1f", App->fpsArr[99]);
		ImGui::PlotHistogram("##framerate", App->fpsArr, ((int)(sizeof(App->fpsArr) / sizeof(*App->fpsArr))), 0, title, 0.0f, 150.0f, ImVec2(310, 100));
		sprintf_s(title, 25, "Milliseconds %.1f", App->msArr[99]);
		ImGui::PlotHistogram("##milliseconds", App->msArr, ((int)(sizeof(App->msArr) / sizeof(*App->msArr))), 0, title, 0.0f, 40.0f, ImVec2(310, 100));

		ImGui::Checkbox("< Freeze Framerate Display", &bFreeze);
	}
}

// ---------------------------------------------< FILE
void ModuleEditor::MenuFile()
{
	if (ImGui::BeginMenu("File"))
	{
		std::map<std::string, bool>::iterator it = file_menu.begin();
		int n = 0;
		char check_box_n[50];

		for (; it != file_menu.end(); it++)
		{
			if (it != file_menu.begin())
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

			if (ImGui::MenuItem(it->first.c_str(), NULL, it->second))
			{
				it->second = !it->second;
			}
			n++;
		}

		ImGui::EndMenu();
	}
}

// ---------------------------------------------< TOOLS
void ModuleEditor::MenuTools()
{
	if (ImGui::BeginMenu("Tools"))
	{
		std::map<std::string, bool>::iterator it = tools_menu.begin();
		int n = 0;
		char check_box_n[50];

		for (; it != tools_menu.end(); it++)
		{
			if (it != tools_menu.begin())
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

			if (ImGui::MenuItem(it->first.c_str(), NULL, it->second))
			{
				it->second = !it->second;
			}
			n++;
		}

		ImGui::EndMenu();
	}
}

// ---------------------------------------------< WINDOW
void ModuleEditor::MenuWindow()
{
	if (ImGui::BeginMenu("Window"))
	{
		std::map<std::string, bool>::iterator it = window_menu.begin();
		int n = 0;
		char check_box_n[50];
		
		for (; it != window_menu.end(); it++)
		{
			if (it != window_menu.begin())
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

			if (ImGui::MenuItem(it->first.c_str(), NULL, it->second))
			{
				it->second = !it->second;
			}
			n++;
		}

		ImGui::EndMenu();
	}
}

// ---------------------------------------------< ABOUT
void ModuleEditor::MenuHelp()
{
	if (ImGui::BeginMenu("Help"))
	{
		std::map<std::string, bool>::iterator it = help_menu.begin();
		int n = 0;
		char check_box_n[50];

		for (; it != help_menu.end(); it++)
		{
			if (it != help_menu.begin())
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

			if (ImGui::MenuItem(it->first.c_str(), NULL, it->second))
			{
				it->second = !it->second;
			}
			n++;
		}

		ImGui::EndMenu();
	}
}

void ModuleEditor::Console()
{
	(ImGui::Begin("Console", 0, ImGuiWindowFlags_NoFocusOnAppearing));
	{
		ImGui::SetWindowSize(ImVec2(1230.0f, 220.0f));
		ImGui::Text(console_buffer.c_str());		
	}

	ImGui::End();
}

void ModuleEditor::Configuration()
{
	sizeX = 370.f;

	ImGui::SetNextWindowSize(ImVec2(sizeX, App->window->screenSurface->h - 560.f));
	ImGui::SetNextWindowPos(ImVec2(App->window->screenSurface->w - sizeX, 558.f));
	ImGuiWindowFlags flag = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_AlwaysAutoResize;
	ImGui::Begin("Configuration", 0, ImVec2(500, 1000), 0.8f, flag);
	{
		App->AddImGui();
	}

	ImGui::End();
}

void ModuleEditor::Heriarchy()
{
	sizeX = 370.f;

	ImGui::SetNextWindowSize(ImVec2(sizeX, App->window->screenSurface->h - 243.f));
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGuiWindowFlags flag = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_AlwaysAutoResize;
	ImGui::Begin("Heriarchy", 0, ImVec2(500, 1000), 0.8f, flag);
	{
		App->scene->AddHeriarchyGui();
	}

	ImGui::End();
}

void ModuleEditor::Properties()
{
	sizeX = 370.f;

	ImGui::SetNextWindowSize(ImVec2(sizeX, App->window->screenSurface->h - 423.f));
	ImGui::SetNextWindowPos(ImVec2(App->window->screenSurface->w - sizeX, 20));
	ImGuiWindowFlags flag = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_AlwaysAutoResize;
	ImGui::Begin("Properties", 0, ImVec2(500, 1000), 0.8f, flag);
	{
		if (selected == nullptr)
		{
			ImGui::Text("You must first load fbx!");
		}
		else
		{
			ImGui::Text("Properties for the Game Object: ");
			ImGui::Text(selected->name.c_str());

			selected->DrawProperties();
		}
	}

	ImGui::End();
}

void ModuleEditor::LoadFile()
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
		App->scene->CreateFbx(fileName);
	}

	file_menu["Load File"] = false;

	//for (auto itFile : file_menu) 
	//{
	//	if (file_menu["Load File"])
	//	{
	//
	//	}
	//}
}

void ModuleEditor::StyleEditor()
{
	ImGui::Begin("Style Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing);
	{
		ImGui::ShowStyleEditor();
	}
	ImGui::End();

	//ImGui::Begin("Heriarchy", 0, ImVec2(500, 1000), 0.8f, flag);
	//{
	//	App->scene->AddHeriarchyGui();
	//}

}


void ModuleEditor::ToolRandom()
{
	ImGui::Begin("Random Generator", 0, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse |
	ImGuiWindowFlags_NoResize);
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

	ImGui::Begin("Geometry##Window", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing);
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
		ImGui::Text("Using SDL (2.0.3), ImGui (1.50), PhysFS, Bullet (2.84), MathGeoLib (1.5), gl3w and parson (latest 2017)");
		ImGui::Spacing();
		ImGui::Text("MIT License. Copyright (c) 2017");
	}

	ImGui::End();
}

void ModuleEditor::AddTextConsole(char* text)
{
	console_buffer.append(text);
}

void ModuleEditor::SetStyle(Style style)
{
	ImGuiStyle &current = ImGui::GetStyle();

	current.AntiAliasedLines = true;
	current.AntiAliasedShapes = true;

	current.WindowTitleAlign = ImGuiAlign_VCenter;
	current.WindowPadding = ImVec2(10, 10);
	current.WindowRounding = 6;
	current.FramePadding = ImVec2(3, 3);
	current.FrameRounding = 2.6f;
	current.ItemSpacing = ImVec2(12, 4);
	current.ItemInnerSpacing = ImVec2(5, 5);
	
	current.ScrollbarSize = 24;
	current.ScrollbarRounding = 4;
	current.WindowRounding = 5.3f;

	current.Colors[ImGuiCol_Text] = ImVec4(style.cText.x, style.cText.y, style.cText.z, 1.00f);
	current.Colors[ImGuiCol_TextDisabled] = ImVec4(style.cText.x, style.cText.y, style.cText.z, 0.58f);
	current.Colors[ImGuiCol_WindowBg] = ImVec4(style.cBody.x, style.cBody.y, style.cBody.z, 0.95f);
	current.Colors[ImGuiCol_ChildWindowBg] = ImVec4(style.cArea.x, style.cArea.y, style.cArea.z, 0.58f);
	current.Colors[ImGuiCol_Border] = ImVec4(style.cBody.x, style.cBody.y, style.cBody.z, 0.00f);
	current.Colors[ImGuiCol_BorderShadow] = ImVec4(style.cBody.x, style.cBody.y, style.cBody.z, 0.00f);
	current.Colors[ImGuiCol_FrameBg] = ImVec4(style.cArea.x, style.cArea.y, style.cArea.z, 1.00f);
	current.Colors[ImGuiCol_FrameBgHovered] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 0.78f);
	current.Colors[ImGuiCol_FrameBgActive] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 1.00f);
	current.Colors[ImGuiCol_TitleBg] = ImVec4(style.cArea.x, style.cArea.y, style.cArea.z, 1.00f);
	current.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(style.cArea.x, style.cArea.y, style.cArea.z, 0.75f);
	current.Colors[ImGuiCol_TitleBgActive] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 1.00f);
	current.Colors[ImGuiCol_MenuBarBg] = ImVec4(style.cArea.x, style.cArea.y, style.cArea.z, 0.47f);
	current.Colors[ImGuiCol_ScrollbarBg] = ImVec4(style.cArea.x, style.cArea.y, style.cArea.z, 1.00f);
	current.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 0.21f);
	current.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 0.78f);
	current.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 1.00f);
	current.Colors[ImGuiCol_ComboBg] = ImVec4(style.cArea.x, style.cArea.y, style.cArea.z, 1.00f);
	current.Colors[ImGuiCol_CheckMark] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 0.80f);
	current.Colors[ImGuiCol_SliderGrab] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 0.50f);
	current.Colors[ImGuiCol_SliderGrabActive] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 1.00f);
	current.Colors[ImGuiCol_Button] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 0.50f);
	current.Colors[ImGuiCol_ButtonHovered] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 0.86f);
	current.Colors[ImGuiCol_ButtonActive] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 1.00f);
	current.Colors[ImGuiCol_Header] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 0.76f);
	current.Colors[ImGuiCol_HeaderHovered] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 0.86f);
	current.Colors[ImGuiCol_HeaderActive] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 1.00f);
	current.Colors[ImGuiCol_Column] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 0.32f);
	current.Colors[ImGuiCol_ColumnHovered] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 0.78f);
	current.Colors[ImGuiCol_ColumnActive] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 1.00f);
	current.Colors[ImGuiCol_ResizeGrip] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 0.15f);
	current.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 0.78f);
	current.Colors[ImGuiCol_ResizeGripActive] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 1.00f);
	current.Colors[ImGuiCol_CloseButton] = ImVec4(style.cText.x, style.cText.y, style.cText.z, 0.16f);
	current.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(style.cText.x, style.cText.y, style.cText.z, 0.39f);
	current.Colors[ImGuiCol_CloseButtonActive] = ImVec4(style.cText.x, style.cText.y, style.cText.z, 1.00f);
	current.Colors[ImGuiCol_PlotLines] = ImVec4(style.cText.x, style.cText.y, style.cText.z, 0.63f);
	current.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 1.00f);
	current.Colors[ImGuiCol_PlotHistogram] = ImVec4(style.cText.x, style.cText.y, style.cText.z, 0.63f);
	current.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 1.00f);
	current.Colors[ImGuiCol_TextSelectedBg] = ImVec4(style.cHead.x, style.cHead.y, style.cHead.z, 0.43f);
	current.Colors[ImGuiCol_PopupBg] = ImVec4(style.cPops.x, style.cPops.y, style.cPops.y, 0.92f);
	current.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(style.cArea.x, style.cArea.y, style.cArea.z, 0.73f);
}

void ModuleEditor::InitStyles()
{
	blueStyle.cText = { 236 / 255.f, 240 / 255.f, 241 / 255.f };
	blueStyle.cHead = { 41 / 255.f, 128 / 255.f, 185 / 255.f };
	blueStyle.cArea = { 57 / 255.f,  79 / 255.f, 105 / 255.f };
	blueStyle.cBody = { 44 / 255.f,  62 / 255.f,  80 / 255.f };
	blueStyle.cPops = { 33 / 255.f,  46 / 255.f,  60 / 255.f };

	mintStyle.cText = { 211 / 255.f, 211 / 255.f, 211 / 255.f };
	mintStyle.cHead = { 95 / 255.f, 142 / 255.f,  85 / 255.f };
	mintStyle.cArea = { 47 / 255.f,  47 / 255.f,  47 / 255.f };
	mintStyle.cBody = { 64 / 255.f,  64 / 255.f,  64 / 255.f };
	mintStyle.cPops = { 30 / 255.f,  30 / 255.f,  30 / 255.f };

	arcStyle.cText = { 211 / 255.f, 218 / 255.f, 227 / 255.f };
	arcStyle.cHead = { 64 / 255.f, 132 / 255.f, 214 / 255.f };
	arcStyle.cArea = { 47 / 255.f,  52 / 255.f,  63 / 255.f };
	arcStyle.cBody = { 56 / 255.f,  60 / 255.f,  74 / 255.f };
	arcStyle.cPops = { 28 / 255.f,  30 / 255.f,  37 / 255.f };

	gteStyle.cText = { 236 / 255.f, 240 / 255.f, 241 / 255.f };
	gteStyle.cHead = { 41 / 255.f, 128 / 255.f, 185 / 255.f };
	gteStyle.cArea = { 57 / 255.f,  79 / 255.f, 105 / 255.f };
	gteStyle.cBody = { 44 / 255.f,  62 / 255.f,  80 / 255.f };
	gteStyle.cPops = { 33 / 255.f,  46 / 255.f,  60 / 255.f };
}