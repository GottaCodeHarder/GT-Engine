#include "Component.h"
#include "MathGeoLib/MathGeoLib.h"

class GameObject;
class cTransform : public Component
{
public:
	cTransform(GameObject* _gameObject);
	~cTransform();

	float3 positionLocal = {0.f,0.f,0.f};
	float3 scaleLocal = { 1.f,1.f,1.f };
	Quat rotationLocal = Quat::identity;

	void SetGlobalPos() {};

	const float3 GetGlobalPos();
	const float3 GetGlobalScale();
	const Quat GetGlobalRoatation();
	const float4x4 GetLocalMatrixTransf();
	const float4x4 GetGlobalMatrixTransf();

	void RealUpdate()
	{
		int i = 5;
	};

	void DrawUI();

private:

};