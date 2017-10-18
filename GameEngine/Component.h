#include "Globals.h"

enum componentType
{
	MESH = 0,
	TRANSFORM,
	MATERIAL,
	CAMERA
};

class Component
{
public:
	componentType type;
	bool active = true;
	bool uiOpen = false;

	virtual void Enable() {};

	void Update()
	{
		if (active)
		{
			RealUpdate();
		}
	}
	
	virtual void Disable() {};

	void DrawUI() {
		if (ImGui::CollapsingHeader("Window")) //AMB EL NOM QUE LI TOQUI
		{
			if (uiOpen)
			{
				RealDrawUI();
			}
		}
	}



private:

	virtual void RealUpdate() {};
	virtual void RealDrawUI() {};
};