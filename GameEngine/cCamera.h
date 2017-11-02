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

	bool transformFrustum = false;
	void FrustumCulling(GameObject* gameObject);

private:

	void dynamicFrustum(GameObject* gameObject);

	bool bCulling = true;
	bool drawFrustum = true;
	bool activeCamera = true;

	float horizontalFOV = 2;
	float verticalFOV = 1;
	float nearPlane = 1;
	float farPlane = 100;
};