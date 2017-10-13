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

	void UpdateWindowSize();
	void SetTitle(const char* title);
	SDL_Window* GetWindow();

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screenSurface;

private:

	bool bFullscreen = WIN_FULLSCREEN;
	bool bFullscreenDesktop = WIN_FULLSCREEN_DESKTOP;
	bool bBorderless = WIN_BORDERLESS;
	bool bResizable = WIN_RESIZABLE;
	bool bMaximize = WIN_MAXIMIZED;
};

#endif // __ModuleWindow_H__