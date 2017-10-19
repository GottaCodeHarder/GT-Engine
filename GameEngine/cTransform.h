#include "Component.h"
#include "MathGeoLib/MathGeoLib.h"

class GameObject;
class cTransform : public Component
{
public:
	cTransform(GameObject* _gameObject);
	~cTransform();

	float3 positionLocal = {0.f,0.f,0.f};
	float3 scaleLocal = float3::zero;
	Quat rotationLocal = Quat::identity;

	const float3 getGlobalPos();
	const float3 getGlobalScale() {};
	const Quat getRoatation() {};
	const float4x4 GetMatrixTransf();

	void RealUpdate()
	{
		int i = 5;
	};

	void DrawUI();

private:

};