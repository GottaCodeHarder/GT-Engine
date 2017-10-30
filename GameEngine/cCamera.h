#include "Component.h"
#include "MathGeoLib/MathGeoLib.h"

class cTransform;
class cCamera : public Component
{
public:
	cCamera(GameObject* _gameObject);
	~cCamera();

	math::Frustum frustum;
	cTransform* transform;

	void RealUpdate();

	void DrawUI();

private:

	bool bCulling = true;

	int horizontalFOV = 87;
	int verticalFOV = 60;
};