#include "Component.h"
#include "MathGeoLib/MathGeoLib.h"

class ResourceMesh;
class GameObject;

class cMesh : public Component
{
public:
	cMesh(GameObject* _gameObject);
	~cMesh();

	ResourceMesh* resource;

	void RealUpdate();

	void DrawUI();
	void DrawAABB(AABB aabbBox);
	bool aabbTransform = false;

	uint Serialize(char* &buffer);
	void DeSerialize(char* &buffer, GameObject* parent);

private:
	bool aabbActive = false;


};