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
		gameObjects.push_back(gameObject);
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


			for (int i = 0; i < gameObjects.size(); i++)
			{
				AddGameObjectToChild(gameObjects[i]);
			}


			gameObjects.clear();
		}
	}
	else
	{
		AddGameObjectToChild(gameObject);
	}
}

void QuadNode::ObjectMoved(GameObject* gameObject)
{
	if (!quadBox.Contains(gameObject->aabbBox.CenterPoint()))
	{
		for (int i = 0; i < gameObjects.size(); i++)
		{
			if (gameObjects[i] == gameObject)
			{

			}
		}
	}

}

void QuadNode::AddGameObjectToChild(GameObject * gameObject)
{
	if (!sons.empty())
	{
		for (int i = 0; i < sons.size(); i++)
		{
			//if (sons[i]->quadBox.Contains(gameObject->aabbBox))
			//{
			//	sons[i]->AddGameObject(gameObject);
			//	break;
			//}
			if (sons[i]->quadBox.Contains(gameObject->aabbBox.CenterPoint()))
			{
				sons[i]->AddGameObject(gameObject);
				break;
			}
		}
	}

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
	root.Draw();
}
