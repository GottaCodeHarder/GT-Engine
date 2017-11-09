#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleScene.h"
#include "ImGui/imgui_impl_sdl_gl3.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouseButtons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
}

// Destructor
ModuleInput::~ModuleInput()
{
	delete[] keyboard;
}

// Called before render is available
bool ModuleInput::Init()
{
	MYLOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		MYLOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate(float dt)
{
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouseX, &mouseY);

	mouseX /= SCREEN_SIZE;
	mouseY /= SCREEN_SIZE;
	mouseZ = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if(mouseButtons[i] == KEY_IDLE)
				mouseButtons[i] = KEY_DOWN;
			else
				mouseButtons[i] = KEY_REPEAT;
		}
		else
		{
			if(mouseButtons[i] == KEY_REPEAT || mouseButtons[i] == KEY_DOWN)
				mouseButtons[i] = KEY_UP;
			else
				mouseButtons[i] = KEY_IDLE;
		}
	}

	if (GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && !ImGui::IsMouseHoveringAnyWindow())
	{
		if (!App->input->GetKey(SDL_SCANCODE_LALT) == KEY_DOWN)
		{
			App->scene->rayCast = true;
		}

	}

	mouseXMotion = mouseYMotion = 0;
	bool quit = false;
	static SDL_Event e;

	while(SDL_PollEvent(&e))
	{
		ImGui_ImplSdlGL3_ProcessEvent(&e);

		switch(e.type)
		{
			case SDL_MOUSEWHEEL:
			{
				mouseZ = e.wheel.y;
				break;
			}

			case SDL_MOUSEMOTION:
			{
				mouseX = e.motion.x / SCREEN_SIZE;
				mouseY = e.motion.y / SCREEN_SIZE;

				mouseXMotion = e.motion.xrel / SCREEN_SIZE;
				mouseYMotion = e.motion.yrel / SCREEN_SIZE;
				break;
			}

			case SDL_DROPFILE:
			{
				file_path = e.drop.file; //BE ES LA RUTA
				has_dropped = true;
				SDL_free(e.drop.file);
				break;
			}

			case SDL_QUIT:
			{
				quit = true;
				break;
			}

			case SDL_WINDOWEVENT:
			{
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					App->window->UpdateWindowSize();
					App->renderer3D->OnResize(e.window.data1, e.window.data2);
				}
			}
		}
	}

	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

void ModuleInput::AddImGui()
{
	static char* name = "";
	if (GetFileDropped() != nullptr)
	{
		name = (char*)GetFileDropped();
	}

	if (ImGui::CollapsingHeader("Input"))
	{
		if (ImGui::TreeNodeEx("Mouse", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::BeginGroup();
			{
				ImGui::Text("Mouse position:");
				ImGui::Text("Mouse motion:");
				ImGui::Text("Mouse wheel:");
				ImGui::EndGroup();
			}
			ImGui::SameLine();
			ImGui::BeginGroup();
			{
				ImGui::Text("%i, %i", GetMouseX(), GetMouseY());
				ImGui::Text("%i, %i", GetMouseXMotion(), GetMouseYMotion());
				ImGui::Text("%i", GetMouseZ());
				ImGui::EndGroup();
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Files", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::TextWrapped("Latest dropped file:\n%s", name);
			ImGui::TreePop();
		}
	}
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	MYLOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

const char * ModuleInput::GetFileDropped()
{
	if (has_dropped)
	{
		return file_path;
	}
	else return nullptr;
	return nullptr;
}
