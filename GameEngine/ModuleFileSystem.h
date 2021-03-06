#ifndef __j1FILESYSTEM_H__
#define __j1FILESYSTEM_H__

#include "Module.h"

struct SDL_RWops;

int close_sdl_rwops(SDL_RWops *rw);

class ModuleFileSystem : public Module
{
public:

	ModuleFileSystem(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleFileSystem();

	// Called before render is available
	bool Init();

	void AddImGui();

	// Called before quitting
	bool CleanUp();

	// Utility functions
	bool AddPath(const char* path_or_zip, const char* mount_point = NULL);
	bool Exists(const char* file) const;
	bool IsDirectory(const char* file) const;
	const char* GetSaveDirectory() const
	{
		return "save/";
	}

	const char* GetExecutableDirectory() const;

	std::vector<std::string> GetFolderContent(const char* path);
	std::vector<std::string> GetFolderContentRecursive(const char* path);

	// Open for Read/Write
	unsigned int Load(const char* file, char** buffer) const;
	SDL_RWops* Load(const char* file) const;

	unsigned int Save(const char* file, const char* buffer, unsigned int size) const;
	
private:

};

#endif // __j1FILESYSTEM_H__