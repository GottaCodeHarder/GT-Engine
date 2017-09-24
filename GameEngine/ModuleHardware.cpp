#include "Globals.h"
#include "ModuleHardware.h"
#include "SDL/include/SDL.h"

ModuleHardware::ModuleHardware(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

// Called before render is available
bool ModuleHardware::Init()
{
	MYLOG("Loading Hardware Specs");
	bool ret = true;

	RefreshSpecs();

	return ret;
}

void ModuleHardware::AddImGui()
{
	if (ImGui::CollapsingHeader("Hardware"))
	{
		char label[20];

		ImGui::Text("SDL Version: ");
		ImGui::SameLine();
		ImGui::Text(sdlVersion);

		ImGui::Separator();

		sprintf_s(label, 20, "%i (Cache: %ikb)", cpuCount, cpuCache);
		ImGui::Text("CPUs: ");
		ImGui::SameLine();
		ImGui::Text(label);

		sprintf_s(label, 20, "%.1fGb", ramGb);
		ImGui::Text("System RAM: ");
		ImGui::SameLine();
		ImGui::Text(label);

		ImGui::Text("Caps: ");
		if (rdtsc)
		{
			ImGui::SameLine(); ImGui::Text("RDTSC, ");
		}
		if (mmx)
		{
			ImGui::SameLine(); ImGui::Text("MMX, ");
		}
		if (sse)
		{
			ImGui::SameLine(); ImGui::Text("SSE,");
		}
		if (sse2)
		{
			ImGui::SameLine(); ImGui::Text("SSE2, ");
		}
		if (sse3)
		{
			ImGui::SameLine(); ImGui::Text("SSE3, ");
		}
		if (sse41)
		{
			ImGui::SameLine(); ImGui::Text("SSE41, ");
		}
		if (sse42)
		{
			ImGui::SameLine(); ImGui::Text("SSE42, ");
		}
		if (avx)
		{
			ImGui::SameLine(); ImGui::Text("AVX, ");
		}
		if (altivec)
		{
			ImGui::SameLine(); ImGui::Text("AltiVec, ");
		}
		if (dddnow)
		{
			ImGui::SameLine(); ImGui::Text("3DNow, ");
		}
	}
}

void ModuleHardware::RefreshSpecs()
{
	SDL_version version;
	SDL_GetVersion(&version);

	sprintf_s(sdlVersion, 10, "%i.%i.%i", version.major, version.minor, version.patch);
	cpuCount = SDL_GetCPUCount();
	cpuCache = SDL_GetCPUCacheLineSize();
	ramGb = float(SDL_GetSystemRAM()) / 1024.f;
	dddnow = SDL_Has3DNow() == SDL_TRUE;
	avx = SDL_HasAVX() == SDL_TRUE;
	altivec = SDL_HasAltiVec() == SDL_TRUE;
	mmx = SDL_HasMMX() == SDL_TRUE;
	rdtsc = SDL_HasRDTSC() == SDL_TRUE;
	sse = SDL_HasSSE() == SDL_TRUE;
	sse2 = SDL_HasSSE2() == SDL_TRUE;
	sse3 = SDL_HasSSE3() == SDL_TRUE;
	sse41 = SDL_HasSSE41() == SDL_TRUE;
	sse42 = SDL_HasSSE42() == SDL_TRUE;
}