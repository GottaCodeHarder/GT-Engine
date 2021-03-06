#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

#include "GTInterface/GTInterface.h"

ModuleWindow::ModuleWindow(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	window = NULL;
	screenSurface = NULL;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	MYLOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		MYLOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		//int width = SCREEN_WIDTH * SCREEN_SIZE;
		//int height = SCREEN_HEIGHT * SCREEN_SIZE;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		
		if (WIN_MAXIMIZED == true)
		{
			flags |= SDL_WINDOW_MAXIMIZED;
		}

		if(WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(App->name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * SCREEN_SIZE, SCREEN_HEIGHT * SCREEN_SIZE, flags);

		if(window == NULL)
		{
			MYLOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screenSurface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}

void ModuleWindow::AddImGui()
{
	if (ImGui::CollapsingHeader("Window"))
	{
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		static int width = screenSurface->w;

		ImGui::VSliderInt("##Height", ImVec2(22, 140), &screenSurface->h, 400, 980);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Doesn't apply if Maximized!\nDoesn't apply if Fullscreen!");
		if (ImGui::IsItemActive())
		{
			if (!bMaximize)
				SDL_SetWindowSize(window, screenSurface->w, screenSurface->h);
		}
		ImGui::SameLine();
		
		ImGui::Text("\n\n\n\n\n         %i x %i", width, screenSurface->h);

		ImGui::Spacing(); ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine();
		ImGui::SliderInt("##Width", &width, 600, 1920);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Doesn't apply if Maximized!\nDoesn't apply if Fullscreen!");
		if (ImGui::IsItemActive())
		{
		}
		else
		{
			if (!bMaximize)
			{
				screenSurface->w = width;
				SDL_SetWindowSize(window, screenSurface->w, screenSurface->h);
			}
		}

		if (ImGui::Checkbox("FullScreen", &bFullscreen))
		{
			if (bFullscreen)
			{
				flags |= SDL_WINDOW_FULLSCREEN;
				SDL_SetWindowFullscreen(window, flags);
			}
			else
			{
				flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
				SDL_SetWindowFullscreen(window, flags);
				bFullscreenDesktop = bMaximize = false;
			}
		}

		ImGui::SameLine();
		if (ImGui::Checkbox("FullScreen Desktop", &bFullscreenDesktop))
		{
			if (bFullscreenDesktop) {
				flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
				SDL_SetWindowFullscreen(window, flags);
				bFullscreen = true;
			}
			else
			{
				flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
				SDL_SetWindowFullscreen(window, flags);
				bFullscreen = bMaximize = false;
			}
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Maximize", &bMaximize))
		{
			if (bMaximize) {
				SDL_MaximizeWindow(window);
			}
			else
			{
				SDL_RestoreWindow(window);
			}
		}

		if (ImGui::Checkbox("Borderless", &bBorderless))
		{
			if (bBorderless)
			{
				SDL_SetWindowBordered(window, SDL_FALSE);
			}
			else
			{
				SDL_SetWindowBordered(window, SDL_TRUE);
			}
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Manually Resizable", &bResizable))
		{
			if (bResizable)
			{
				flags |= SDL_WINDOW_RESIZABLE;
			}
			else
			{
				SDL_RestoreWindow(window);
			}
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Not yet available");
	}
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	MYLOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::UpdateWindowSize()
{
	SDL_GetWindowSize(window, &screenSurface->w, &screenSurface->h);
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

SDL_Window* ModuleWindow::GetWindow()
{
	return window;
}