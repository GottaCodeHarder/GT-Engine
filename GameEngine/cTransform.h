#include "Component.h"
#include "MathGeoLib/MathGeoLib.h"

class GameObject;
class cUI;

enum TransformType
{
	NONE = 0,
	NORMAL,
	RECT_TRANS
};

class cTransform : public Component
{
public:
	cTransform(GameObject* _gameObject, TransformType t = NONE) :
		Component(TRANSFORM, _gameObject), type(t) {}
	virtual ~cTransform() {}

	TransformType GetType() const { return type; }
	virtual void SetRectSource(cUI* ui) {}

	virtual void SetGlobalPos() {}

	virtual const float3 GetGlobalPos() { return float3(0.f); }
	virtual const float3 GetGlobalScale() { return float3(1.f); }
	virtual const Quat GetGlobalRoatation() { return Quat::identity; }
	virtual const float4x4 GetLocalMatrixTransf() { return float4x4(Quat::identity); }
	virtual const float4x4 GetGlobalMatrixTransf() { return float4x4(Quat::identity); }
	virtual void SetGlobalTransform(float4x4 matrix) {}
	virtual void SetGlobalPos(float3 globalPos) {}
	virtual void SetGlobalScale(float3 globalScale) {}
	virtual void SetGlobalRot(Quat globalRot) {}

	virtual const float3 GetLocalPos() { return float3(0.f); }
	virtual const float3 GetLocalScale() { return float3(0.f); }
	virtual const Quat GetLocalRot() { return Quat::identity; }
	virtual void SetLocalPos(float3 pos) {}
	virtual void SetLocalScale(float3 scale) {}
	virtual void SetLocalRot(Quat rot) {}

	virtual void DrawUI() {}

	virtual uint Serialize(char* &buffer) { return 0; }
	virtual uint DeSerialize(char* &buffer, GameObject* parent) { return 0; }

public:
	bool transformChange = false;
	TransformType type;
};


class Transform : public cTransform
{
public:
	Transform(GameObject* _gameObject);
	~Transform();

	void SetGlobalPos() {}

	const float3 GetGlobalPos();
	const float3 GetGlobalScale();
	const Quat GetGlobalRoatation();
	const float4x4 GetLocalMatrixTransf();
	const float4x4 GetGlobalMatrixTransf();
	void SetGlobalTransform(float4x4 matrix);
	void SetGlobalPos(float3 globalPos);
	void SetGlobalScale(float3 globalScale);
	void SetGlobalRot(Quat globalRot);

	const float3 GetLocalPos();
	const float3 GetLocalScale();
	const Quat GetLocalRot();
	void SetLocalPos(float3 pos);
	void SetLocalScale(float3 scale);
	void SetLocalRot(Quat rot);

	void DrawUI();

	uint Serialize(char* &buffer);
	uint DeSerialize(char* &buffer, GameObject* parent);

private:
	float3 positionLocal;
	float3 scaleLocal;
	Quat rotationLocal;
};


class RectTransform : public cTransform
{
public:
	RectTransform(GameObject* _gameObject);
	~RectTransform();

	void SetRectSource(cUI* reference = nullptr);

	void SetGlobalPos() {}

	const float3 GetGlobalPos();
	const float3 GetGlobalScale();
	const Quat GetGlobalRoatation();
	const float4x4 GetLocalMatrixTransf();
	const float4x4 GetGlobalMatrixTransf();
	void SetGlobalTransform(float4x4 matrix);
	void SetGlobalPos(float3 globalPos);
	void SetGlobalScale(float3 globalScale);
	void SetGlobalRot(Quat globalRot);

	const float3 GetLocalPos();
	const float3 GetLocalScale();
	const Quat GetLocalRot();
	void SetLocalPos(float3 pos);
	void SetLocalScale(float3 scale);
	void SetLocalRot(Quat rot);

	void DrawUI();

	uint Serialize(char* &buffer);
	uint DeSerialize(char* &buffer, GameObject* parent);

private:
	bool HasUIRect() const;

	float fRotationX = 0.0f;
	float fRotationY = 0.0f;
	float fRotationZ = 0.0f;

private:
	cUI* ui;
};