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
	void DeleteGameObjects();

	template <typename Col>
	std::vector<GameObject*> Collide(const Col colision);


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

	QuadNode GetRoot();
private:
	QuadNode root;
};


template <typename Col>
inline std::vector<GameObject*> QuadNode::Collide(const Col colision)
{
	std::vector<GameObject*> ret;

	if (quadBox.Intersects(colision) || quadBox.Contains(colision))
	{
		for (auto itGameObjects : gameObjects)
		{
			if (colision.Intersects(itGameObjects->aabbBox))
			{
				ret.push_back(itGameObjects);
			}
		}
		for (auto itSons : sons)
		{
			itSons->Collide(colision);
		}
	}

	return ret;
}
#endif // !__QUADTREE__

//OBJECTES DINAMIS I ESTATICS, ELS DINAMICS NO S'AFEGEIXEN A L'ARBRE

//AFEGIR OBJECTE
//RETORNAR COLISIONS
//UN OBJECTE AL MIG DE VARIES AABB
//OBJECTE FORA QUADTREE
//MOC UN OBJECTE
//BORRAR UN OBJECTE

