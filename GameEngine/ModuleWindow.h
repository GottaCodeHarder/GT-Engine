#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	void AddImGui();
	bool CleanUp();

	void SetTitle(const char* title);
	SDL_Window* GetWindow();

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

private:
	int width = SCREEN_WIDTH * SCREEN_SIZE;
	int	height = SCREEN_HEIGHT * SCREEN_SIZE;
	bool fullscreen = WIN_FULLSCREEN;
	bool fullscreen_desktop = WIN_FULLSCREEN_DESKTOP;
	bool borderless = WIN_BORDERLESS;
	bool resizable = WIN_RESIZABLE;
};

#endif // __ModuleWindow_H__