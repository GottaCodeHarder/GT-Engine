#include "QuadTree.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

QuadNode::QuadNode(float3 minPoint, float3 maxPoint)
{
	quadBox.minPoint = minPoint;
	quadBox.maxPoint = maxPoint;

}

void QuadNode::Draw()
{
	if (!sons.empty())
	{
		for (int i = 0; i < sons.size(); i++)
		{
			sons[i]->Draw();
		}
	}
	else
	{
		DrawAABB(quadBox);
	}
}

void QuadNode::DrawAABB(AABB aabbBox)
{
	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.minPoint.x, aabbBox.minPoint.y, aabbBox.minPoint.z), btVector3(aabbBox.minPoint.x, aabbBox.maxPoint.y, aabbBox.minPoint.z), btVector3(0, 0.7f, 0));
	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.minPoint.x, aabbBox.minPoint.y, aabbBox.minPoint.z), btVector3(aabbBox.maxPoint.x, aabbBox.minPoint.y, aabbBox.minPoint.z), btVector3(0, 0.7f, 0));
	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.minPoint.x, aabbBox.minPoint.y, aabbBox.minPoint.z), btVector3(aabbBox.minPoint.x, aabbBox.minPoint.y, aabbBox.maxPoint.z), btVector3(0, 0.7f, 0));

	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.maxPoint.x, aabbBox.minPoint.y, aabbBox.maxPoint.z), btVector3(aabbBox.minPoint.x, aabbBox.minPoint.y, aabbBox.maxPoint.z), btVector3(0, 0.7f, 0));
	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.maxPoint.x, aabbBox.minPoint.y, aabbBox.maxPoint.z), btVector3(aabbBox.maxPoint.x, aabbBox.minPoint.y, aabbBox.minPoint.z), btVector3(0, 0.7f, 0));

	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.maxPoint.x, aabbBox.maxPoint.y, aabbBox.maxPoint.z), btVector3(aabbBox.maxPoint.x, aabbBox.minPoint.y, aabbBox.maxPoint.z), btVector3(0, 0.7f, 0));
	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.maxPoint.x, aabbBox.maxPoint.y, aabbBox.maxPoint.z), btVector3(aabbBox.maxPoint.x, aabbBox.maxPoint.y, aabbBox.minPoint.z), btVector3(0, 0.7f, 0));
	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.maxPoint.x, aabbBox.maxPoint.y, aabbBox.maxPoint.z), btVector3(aabbBox.minPoint.x, aabbBox.maxPoint.y, aabbBox.maxPoint.z), btVector3(0, 0.7f, 0));

	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.minPoint.x, aabbBox.maxPoint.y, aabbBox.maxPoint.z), btVector3(aabbBox.minPoint.x, aabbBox.minPoint.y, aabbBox.maxPoint.z), btVector3(0, 0.7f, 0));
	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.minPoint.x, aabbBox.maxPoint.y, aabbBox.maxPoint.z), btVector3(aabbBox.minPoint.x, aabbBox.maxPoint.y, aabbBox.minPoint.z), btVector3(0, 0.7f, 0));

	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.maxPoint.x, aabbBox.maxPoint.y, aabbBox.minPoint.z), btVector3(aabbBox.minPoint.x, aabbBox.maxPoint.y, aabbBox.minPoint.z), btVector3(0, 0.7f, 0));
	App->renderer3D->debugDraw->drawLine(btVector3(aabbBox.maxPoint.x, aabbBox.maxPoint.y, aabbBox.minPoint.z), btVector3(aabbBox.maxPoint.x, aabbBox.minPoint.y, aabbBox.minPoint.z), btVector3(0, 0.7f, 0));

}

void QuadNode::AddGameObject(GameObject * gameObject)
{
	if (sons.empty())
	{

		if (quadBox.Contains(gameObject->aabbBox.CenterPoint()))
		{
			if (gameObject->statiC)
			{
				if (gameObject != nullptr)
				{
					gameObjects.push_back(gameObject);
				}

			}
		}

		if (gameObjects.size() > MAX_OBJECTS_NODE)
		{
			float3 centerPoint = quadBox.CenterPoint();

			QuadNode* node1 = new QuadNode(quadBox.minPoint, float3(centerPoint.x, quadBox.maxPoint.y, centerPoint.z));
			QuadNode* node2 = new QuadNode(float3(centerPoint.x, quadBox.minPoint.y, centerPoint.z), quadBox.maxPoint);
			QuadNode* node3 = new QuadNode(float3(quadBox.minPoint.x, quadBox.minPoint.y, centerPoint.z), float3(centerPoint.x, quadBox.maxPoint.y, quadBox.maxPoint.z));
			QuadNode* node4 = new QuadNode(float3(centerPoint.x, quadBox.minPoint.y, quadBox.minPoint.z), float3(quadBox.maxPoint.x, quadBox.maxPoint.y, centerPoint.z));

			sons.push_back(node1);
			sons.push_back(node2);
			sons.push_back(node3);
			sons.push_back(node4);

			std::vector<GameObject*> tmpGO;
			GameObject* tmp;
			for (int i = 0; i < gameObjects.size(); i++)
			{
				tmp=AddGameObjectToChild(gameObjects[i]);
				if (tmp != nullptr)
				{
					tmpGO.push_back(tmp);
				}
			}

			gameObjects.clear();
			for (auto tmpGO1 : tmpGO)
			{
				if (tmpGO1->statiC)
				{
					if (tmpGO1 != nullptr)
					{
						gameObjects.push_back(tmpGO1);
					}

				}
			}
			tmpGO.clear();

		}
	}
	else
	{
		if (gameObject->statiC)
		{
			if (gameObject != nullptr)
			{
				gameObjects.push_back(AddGameObjectToChild(gameObject));
			}

		}

	}
}

GameObject* QuadNode::AddGameObjectToChild(GameObject * gameObject)
{
	if (!sons.empty())
	{
		bool sonAssigned = false;
		for (int i = 0; i < sons.size(); i++)
		{
			if (sons[i]->quadBox.Contains(gameObject->aabbBox))
			{
				if (gameObject->statiC)
				{
					if (gameObject != nullptr)
					{
						sons[i]->AddGameObject(gameObject);
						sonAssigned = true;
						return nullptr;
					}
				}
			}
		}
		if (!sonAssigned)
		{
			return gameObject;
		}
	}
}

void QuadNode::DeleteGameObjects()
{
	if (!sons.empty())
	{
		for (auto mySons : sons)
		{
			mySons->DeleteGameObjects();
		}
	}
	gameObjects.clear();
	sons.clear();
}

myQuadTree::myQuadTree() : root(MIN_WORLD_POINT, MAX_WORLD_POINT)
{
}

void myQuadTree::AddGameObject(GameObject * gameObject)
{
	root.AddGameObject(gameObject);
}

void myQuadTree::Draw()
{
	if (draw)
	{
		root.Draw();
	}
}

void myQuadTree::EmtpyQuad()
{
	root.DeleteGameObjects();
}

void myQuadTree::RecalculateQuad(GameObject* rootScene)
{
	for (auto rootSons : rootScene->sons)
	{
		RecalculateQuad(rootSons);
	}
	AddGameObject(rootScene);
}

QuadNode myQuadTree::GetRoot()
{
	return root;
}
