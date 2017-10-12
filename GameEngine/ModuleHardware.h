#ifndef __MODULEHARDWARE_H__
#define __MODULEHARDWARE_H__

#include "Module.h"

class ModuleHardware : public Module
{
public:
	ModuleHardware(Application* app, bool start_enabled = true);

	bool Init();

	void AddImGui();

	void RefreshSpecs(bool all);

public:

	std::string a;

	char sdlVersion[10] = "";
	int cpuCount = 0;
	int cpuCache = 0;
	float ramGb = 0.f;
	bool dddnow = false;
	bool avx = false;
	bool avx2 = false;
	bool altivec = false;
	bool mmx = false;
	bool rdtsc = false;
	bool sse = false;
	bool sse2 = false;
	bool sse3 = false;
	bool sse41 = false;
	bool sse42 = false;
};

#endif 