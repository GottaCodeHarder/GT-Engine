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

	void DesactivateCameraAttached(GameObject* GO);
	void AttachCamera();
	void TransformCamera();
	bool transformFrustum = false;
	void FrustumCulling(GameObject* gameObject);


	bool cameraAttached = false;

	uint Serialize(char* &buffer);
	uint DeSerialize(char* &buffer, GameObject* parent);

private:

	void dynamicFrustum(GameObject* gameObject);

	bool bCulling = true;
	bool drawFrustum = true;
	bool activeCamera = true;

	float horizontalFOV = 1;
	float verticalFOV = 1;
	float nearPlane = 0.5;
	float farPlane = 200;
};