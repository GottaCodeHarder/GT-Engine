#ifndef __QUADTREE__
#define __QUADTREE__


#include "Globals.h"
#include "MathGeoLib/MathGeoLib.h"
#include "GameObject.h"

#define MAX_OBJECTS_NODE 4
#define MIN_WORLD_POINT float3(-20,-20,-20)
#define MAX_WORLD_POINT float3(40,40,40)


class QuadNode 
{
public:
	
	QuadNode(float3 minPoint,  float3 maxPoint);
	
	AABB quadBox;

	std::vector<GameObject*> gameObjects;
	std::vector<QuadNode*> sons;

	void Draw();
	void DrawAABB(AABB aabbBox);

	void AddGameObject(GameObject* gameObject);
	void ObjectMoved(GameObject* gameObject);
	void DeleteGameObjects();


private:
	void AddGameObjectToChild(GameObject* gameObject);
};


class myQuadTree 
{
public:
	myQuadTree();

	void AddGameObject(GameObject* gameObject);

	void Draw();

	void EmtpyQuad();
	void RecalculateQuad(GameObject* rootScene);

	bool draw = true;
private:
	QuadNode root;
};
#endif // !__QUADTREE__

//AFEGIR OBJECTE
//RETORNAR COLISIONS
//UN OBJECTE AL MIG DE VARIES AABB
//OBJECTE FORA QUADTREE
//MOC UN OBJECTE
//BORRAR UN OBJECTE