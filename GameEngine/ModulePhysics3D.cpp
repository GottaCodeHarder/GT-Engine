#include "Globals.h"
#include "Application.h"
#include "ModulePhysics3D.h"
#include "ModuleInput.h"
#include "PhysBody3D.h"
#include "Primitive.h"
#include "ModuleRenderer3D.h"

#ifdef _DEBUG
	#pragma comment (lib, "Bullet/libx86/BulletDynamics_debug.lib")
	#pragma comment (lib, "Bullet/libx86/BulletCollision_debug.lib")
	#pragma comment (lib, "Bullet/libx86/LinearMath_debug.lib")
#else 
	#pragma comment (lib, "Bullet/libx86/BulletDynamics.lib")
	#pragma comment (lib, "Bullet/libx86/BulletCollision.lib")
	#pragma comment (lib, "Bullet/libx86/LinearMath.lib")
#endif

ModulePhysics3D::ModulePhysics3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	debug = true;

	collisionConf = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConf);
	broadPhase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	debugDraw = new DebugDrawer();
}

// Destructor
ModulePhysics3D::~ModulePhysics3D()
{
	delete debugDraw;
	delete solver;
	delete broadPhase;
	delete dispatcher;
	delete collisionConf;
}

// Render not available yet----------------------------------
bool ModulePhysics3D::Init()
{
	MYLOG("Creating 3D Physics simulation");
	bool ret = true;

	return ret;
}

// ---------------------------------------------------------
bool ModulePhysics3D::Start()
{
	MYLOG("Creating Physics environment");

	world = new btDiscreteDynamicsWorld(dispatcher, broadPhase, solver, collisionConf);
	world->setDebugDrawer(debugDraw);
	world->setGravity(GRAVITY);
	vehicleRaycaster = new btDefaultVehicleRaycaster(world);

	// Big plane as ground
	{
		btCollisionShape* colShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);

		btDefaultMotionState* myMotionState = new btDefaultMotionState();
		btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, myMotionState, colShape);

		btRigidBody* body = new btRigidBody(rbInfo);
		world->addRigidBody(body);
	}

	return true;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PreUpdate(float dt)
{
	world->stepSimulation(dt, 15);

	int numManifolds = world->getDispatcher()->getNumManifolds();
	for(int i = 0; i<numManifolds; i++)
	{
		btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
		btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

		int numContacts = contactManifold->getNumContacts();
		if(numContacts > 0)
		{
			PhysBody3D* pbodyA = (PhysBody3D*)obA->getUserPointer();
			PhysBody3D* pbodyB = (PhysBody3D*)obB->getUserPointer();

			if(pbodyA && pbodyB)
			{
				std::list<Module*>::iterator item = pbodyA->collision_listeners.begin();
				
				for (; item != pbodyA->collision_listeners.end(); item++)
				{
					(*item)->OnCollision(pbodyA, pbodyB);
				}

				item = pbodyB->collision_listeners.begin();
				
				for (; item != pbodyB->collision_listeners.end(); item++)
				{
					(*item)->OnCollision(pbodyB, pbodyA);
				}
			}
		}
	}

	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::Update(float dt)
{
	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	world->debugDrawWorld();

	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModulePhysics3D::CleanUp()
{
	MYLOG("Destroying 3D Physics simulation");

	// Remove from the world all collision bodies
	for(int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		world->removeCollisionObject(obj);
	}

	for(std::list<btTypedConstraint*>::iterator it = constraints.begin(); it != constraints.end(); it++)
	{
		world->removeConstraint(*it);
		delete (*it);
	}
	
	constraints.clear();

	for (std::list<btDefaultMotionState*>::iterator it = motions.begin(); it != motions.end(); it++)
		delete (*it);

	motions.clear();

	for (std::list<btCollisionShape*>::iterator it = shapes.begin(); it != shapes.end(); it++)
		delete (*it);

	shapes.clear();

	for(std::list<PhysBody3D*>::iterator it = bodies.begin(); it != bodies.end(); it++)
		delete (*it);

	bodies.clear();
	
	delete vehicleRaycaster;
	delete world;

	return true;
}

// ---------------------------------------------------------
PhysBody3D* ModulePhysics3D::AddBody(const pbSphere& sphere, float mass)
{
	btCollisionShape* colShape = new btSphereShape(sphere.radius);
	shapes.push_back(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&sphere.transform);

	btVector3 localInertia(0, 0, 0);
	if(mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody3D* pbody = new PhysBody3D(body);

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	bodies.push_back(pbody);

	return pbody;
}


// ---------------------------------------------------------
PhysBody3D* ModulePhysics3D::AddBody(const Cube& cube, float mass)
{
	btCollisionShape* colShape = new btBoxShape(btVector3(cube.size.x*0.5f, cube.size.y*0.5f, cube.size.z*0.5f));
	shapes.push_back(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&cube.transform);

	btVector3 localInertia(0, 0, 0);
	if(mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody3D* pbody = new PhysBody3D(body);

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	bodies.push_back(pbody);

	return pbody;
}

// ---------------------------------------------------------
PhysBody3D* ModulePhysics3D::AddBody(const pbCylinder& cylinder, float mass)
{
	btCollisionShape* colShape = new btCylinderShapeX(btVector3(cylinder.height*0.5f, cylinder.radius, 0.0f));
	shapes.push_back(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&cylinder.transform);

	btVector3 localInertia(0, 0, 0);
	if(mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody3D* pbody = new PhysBody3D(body);

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	bodies.push_back(pbody);

	return pbody;
}

// ---------------------------------------------------------
void ModulePhysics3D::AddConstraintP2P(PhysBody3D& bodyA, PhysBody3D& bodyB, const vec3& anchorA, const vec3& anchorB)
{
	btTypedConstraint* p2p = new btPoint2PointConstraint(
		*(bodyA.body), 
		*(bodyB.body), 
		btVector3(anchorA.x, anchorA.y, anchorA.z), 
		btVector3(anchorB.x, anchorB.y, anchorB.z));
	world->addConstraint(p2p);
	constraints.push_back(p2p);
	p2p->setDbgDrawSize(2.0f);
}

void ModulePhysics3D::AddConstraintHinge(PhysBody3D& bodyA, PhysBody3D& bodyB, const vec3& anchorA, const vec3& anchorB, const vec3& axisA, const vec3& axisB, bool disable_collision)
{
	btHingeConstraint* hinge = new btHingeConstraint(
		*(bodyA.body), 
		*(bodyB.body), 
		btVector3(anchorA.x, anchorA.y, anchorA.z),
		btVector3(anchorB.x, anchorB.y, anchorB.z),
		btVector3(axisA.x, axisA.y, axisA.z), 
		btVector3(axisB.x, axisB.y, axisB.z));

	world->addConstraint(hinge, disable_collision);
	constraints.push_back(hinge);
	hinge->setDbgDrawSize(2.0f);
}

void ModulePhysics3D::AddConstraintSlider(PhysBody3D& bodyA, PhysBody3D& bodyB, const btTransform& A, const btTransform& B, bool useLinearReferenceFrameA)
{
	btSliderConstraint* slider = new btSliderConstraint(*(bodyA.body), *(bodyB.body), A, B, useLinearReferenceFrameA);

	world->addConstraint(slider, useLinearReferenceFrameA);
	constraints.push_back(slider);
	slider->setDbgDrawSize(2.0f);
}



//// =============================================
//void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
//{
//	line.origin.Set(from.getX(), from.getY(), from.getZ());
//	line.destination.Set(to.getX(), to.getY(), to.getZ());
//	line.color.Set(color.getX(), color.getY(), color.getZ());
//	line.Render();
//}
//
//void DebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
//{
//	point.transform.translate(PointOnB.getX(), PointOnB.getY(), PointOnB.getZ());
//	point.color.Set(color.getX(), color.getY(), color.getZ());
//	point.Render();
//}
//
//void DebugDrawer::reportErrorWarning(const char* warningString)
//{
//	MYLOG("Bullet warning: %s", warningString);
//}
//
//void DebugDrawer::draw3dText(const btVector3& location, const char* textString)
//{
//	MYLOG("Bullet draw text: %s", textString);
//}
//
//void DebugDrawer::setDebugMode(int debugMode)
//{
//	mode = (DebugDrawModes) debugMode;
//}
//
//int	 DebugDrawer::getDebugMode() const
//{
//	return mode;
//}
