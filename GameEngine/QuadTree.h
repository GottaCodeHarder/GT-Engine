#include "Globals.h"
#include "MathGeoLib/MathGeoLib.h"
#include "GameObject.h"

#define MAX_OBJECTS_NODE 4


class QuadNode 
{
public:
	AABB quadBox;

	std::vector<GameObject*> gameObjects;
	std::vector<QuadNode*> sons;

	void AddGameObject(GameObject* gameObject) {};
};


class Quadtree 
{

public:
	QuadNode root;

};