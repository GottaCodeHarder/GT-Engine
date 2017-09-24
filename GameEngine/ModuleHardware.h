#ifndef __MODULEHARDWARE_H__
#define __MODULEHARDWARE_H__

#include "Module.h"

class ModuleHardware : public Module
{
public:
	ModuleHardware(Application* app, bool start_enabled = true);
};

#endif 