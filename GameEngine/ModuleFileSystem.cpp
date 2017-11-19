#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "PhysFS/include/physfs.h"
#include "SDL/include/SDL.h"
#include "JSON/parson.h"
#include "Importer.h"

#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

ModuleFileSystem::ModuleFileSystem(Application* app, bool start_enabled) : Module(app, start_enabled)
{

	// need to be created before Awake so other modules can use it
	char* base_path = SDL_GetBasePath();
	PHYSFS_init(base_path);
	SDL_free(base_path);
}

// Destructor
ModuleFileSystem::~ModuleFileSystem()
{
	PHYSFS_deinit();
}

// Called before render is available
bool ModuleFileSystem::Init()
{
	MYLOG("Loading File System");
	bool ret = true;

	PHYSFS_setWriteDir(".");
	AddPath(".");

	return ret;
}

void ModuleFileSystem::AddImGui()
{

}

// Called before quitting
bool ModuleFileSystem::CleanUp()
{
	//LOG("Freeing File System subsystem");
	return true;
}

// Add a new zip file or folder
bool ModuleFileSystem::AddPath(const char* path_or_zip, const char* mount_point)
{
	bool ret = false;

	if (PHYSFS_mount(path_or_zip, mount_point, 1) == 0)
	{
		MYLOG("File System error while adding a path or zip(%s): %s\n", path_or_zip, PHYSFS_getLastError());
	}
	else
		ret = true;

	return ret;
}

// Check if a file exists
bool ModuleFileSystem::Exists(const char* file) const
{
	return PHYSFS_exists(file) != 0;
}

// Check if a file is a directory
bool ModuleFileSystem::IsDirectory(const char* file) const
{
	return PHYSFS_isDirectory(file) != 0;
}

const char * ModuleFileSystem::GetExecutableDirectory() const
{
	return PHYSFS_getBaseDir();
}

std::vector<std::string> ModuleFileSystem::GetFolderContent(const char * path)
{
	std::vector<std::string> ret;

	// Convert char** to vector<string>
	
	char **files = PHYSFS_enumerateFiles(path);
	char **tmp = files;

	if (*files == NULL)
		MYLOG("Failure. Reason: %s.\n", PHYSFS_getLastError());

	for (char* i = *tmp; i != NULL; i=*++tmp)
	{
		ret.push_back(i);
	}
	
	PHYSFS_freeList(files);

	return ret;
}

std::vector<std::string> ModuleFileSystem::GetFolderContentRecursive(const char * path)
{
	std::vector<std::string> files = GetFolderContent(path);
	
	std::vector<std::string> ret;

	for (std::vector<std::string>::iterator i = files.begin(); i != files.end(); i++)
	{
		FileExtensions type = Importer::GetExtension(i->c_str());

		std::string strPath = path;
		strPath += "/";
		strPath += i->c_str();

		if (type == FileExtensions::Folder)
		{
			std::vector<std::string> tmp = GetFolderContentRecursive(strPath.c_str());
			ret.insert(ret.end(), tmp.begin(), tmp.end());
		}
		else if (type != FileExtensions::Unsupported)
		{
			ret.push_back(strPath);
		}
	}

	return ret;
}

// Read a whole file and put it in a new buffer
unsigned int ModuleFileSystem::Load(const char* file, char** buffer) const
{
	unsigned int ret = 0;

	PHYSFS_file* fs_file = PHYSFS_openRead(file);

	if (fs_file != NULL)
	{
		PHYSFS_sint64 size = PHYSFS_fileLength(fs_file);

		if (size > 0)
		{
			*buffer = new char[(uint)size];
			PHYSFS_sint64 readed = PHYSFS_read(fs_file, *buffer, 1, (PHYSFS_sint32)size);
			if (readed != size)
			{
				MYLOG("File System error while reading from file %s: %s\n", file, PHYSFS_getLastError());
				delete (buffer);
			}
			else
				ret = (uint)readed;
		}

		if (PHYSFS_close(fs_file) == 0)
			MYLOG("File System error while closing file %s: %s\n", file, PHYSFS_getLastError());
	}
	else
		MYLOG("File System error while opening file %s: %s\n", file, PHYSFS_getLastError());

	return ret;
}

// Read a whole file and put it in a new buffer
SDL_RWops* ModuleFileSystem::Load(const char* file) const
{
	char* buffer;
	int size = Load(file, &buffer);

	if (size > 0)
	{
		SDL_RWops* r = SDL_RWFromConstMem(buffer, size);
		if (r != NULL)
			r->close = close_sdl_rwops;

		return r;
	}
	else
		return NULL;
}

int close_sdl_rwops(SDL_RWops *rw)
{
	delete (rw->hidden.mem.base);
	SDL_FreeRW(rw);
	return 0;
}

// Save a whole buffer to disk
unsigned int ModuleFileSystem::Save(const char* file, const char* buffer, unsigned int size) const
{
	unsigned int ret = 0;
	
	std::string tmp = GetExecutableDirectory();
	// If files are from Assets
	Importer::FindAndReplace(tmp, "\\", "/");
	tmp += file;

	/*int pos = tmp.find("Assets");
	if (pos != std::string::npos)
	{
	tmp = tmp.substr(pos + 7); // 7 being "Assets/"
	}

	pos = tmp.find("Library");
	if (pos != std::string::npos)
	{
	tmp = tmp.substr(pos + 8); // 7 being "Assets/"
	}*/

	PHYSFS_file* fs_file = PHYSFS_openWrite(file);

	if (fs_file != NULL)
	{
		PHYSFS_sint64 written = PHYSFS_write(fs_file, (const void*)buffer, 1, size);
		if (written != size)
		{
			MYLOG("File System error while writing to file %s: %s\n", file, PHYSFS_getLastError());
		}
		else
			ret = (uint)written;

		if (PHYSFS_close(fs_file) == 0)
			MYLOG("File System error while closing file %s: %s\n", file, PHYSFS_getLastError());
	}
	else
		MYLOG("File System error while opening file %s: %s\n", file, PHYSFS_getLastError());

	return ret;
}