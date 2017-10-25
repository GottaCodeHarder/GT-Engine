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

	void RealUpdate()
	{
		int i = 5;
	};

	void DrawUI();

private:
};