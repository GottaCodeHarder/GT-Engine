#include "Globals.h"
#include "ModuleHardware.h"
#include "SDL/include/SDL.h"
#include "glew/include/glew.h"

ModuleHardware::ModuleHardware(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

// Called before render is available
bool ModuleHardware::Init()
{
	MYLOG("Loading Hardware Specs");
	bool ret = true;

	RefreshSpecs(true);

	return ret;
}

void ModuleHardware::AddImGui()
{
	if (ImGui::CollapsingHeader("Hardware"))
	{
		char label[20];
		if (ImGui::TreeNodeEx("GPU Related", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::BeginGroup();
			{
				ImGui::Text("Vendor: ");
				ImGui::Text("Renderer: ");
				ImGui::Text("GL version: ");
				ImGui::Text("GL shading ver: ");
				ImGui::EndGroup();
			}
			ImGui::SameLine();
			ImGui::BeginGroup();
			{
				ImGui::Text("%s", glGetString(GL_VENDOR));
				ImGui::Text("%s", glGetString(GL_RENDERER));
				ImGui::Text("%s", glGetString(GL_VERSION));
				ImGui::Text("%s", glGetString(GL_SHADING_LANGUAGE_VERSION));
				ImGui::EndGroup();
			}
			ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("CPU Related", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::BeginGroup();
			{
				ImGui::Text("SDL Version: ");
				ImGui::Text("CPUs: ");
				ImGui::Text("System RAM: ");
				ImGui::EndGroup();
			}
			ImGui::SameLine();
			ImGui::BeginGroup();
			{
				ImGui::Text(sdlVersion);
				sprintf_s(label, 20, "%i (Cache: %ikb)", cpuCount, cpuCache);
				ImGui::Text(label);
				sprintf_s(label, 20, "%.1fGb", ramGb);
				ImGui::Text(label);
				ImGui::EndGroup();
			}

			ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

			ImGui::Text("Capacities:");
			ImGui::BeginGroup();
			{
				if (ImGui::Checkbox("RDTSC", &rdtsc))
				{
					RefreshSpecs(false);
				}
				if (ImGui::Checkbox("MMX", &mmx))
				{
					RefreshSpecs(false);
				}
				if (ImGui::Checkbox("SSE", &sse))
				{
					RefreshSpecs(false);
				}
				ImGui::EndGroup();
			}
			ImGui::SameLine();
			ImGui::BeginGroup();
			{
				if (ImGui::Checkbox("SSE2", &sse2))
				{
					RefreshSpecs(false);
				}
				if (ImGui::Checkbox("SSE3", &sse3))
				{
					RefreshSpecs(false);
				}
				if (ImGui::Checkbox("SSE41", &sse41))
				{
					RefreshSpecs(false);
				}
				ImGui::EndGroup();
			}
			ImGui::SameLine();
			ImGui::BeginGroup();
			{
				if (ImGui::Checkbox("SSE42", &sse42))
				{
					RefreshSpecs(false);
				}
				if (ImGui::Checkbox("AVX", &avx))
				{
					RefreshSpecs(false);
				}
				if (ImGui::Checkbox("AltiVec", &altivec))
				{
					RefreshSpecs(false);
				}
				ImGui::EndGroup();
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("3DNow", &dddnow))
			{
				RefreshSpecs(false);
			}
			ImGui::TreePop();
		}
	}
}

void ModuleHardware::RefreshSpecs(bool all)
{
	if (all)
	{
		SDL_version version;
		SDL_GetVersion(&version);

		sprintf_s(sdlVersion, 10, "%i.%i.%i", version.major, version.minor, version.patch);
		cpuCount = SDL_GetCPUCount();
		cpuCache = SDL_GetCPUCacheLineSize();
		ramGb = float(SDL_GetSystemRAM()) / 1024.f;
	}

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