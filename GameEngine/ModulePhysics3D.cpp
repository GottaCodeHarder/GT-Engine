#include "Globals.h"
#include "Application.h"
#include "ModulePhysics3D.h"
#include "PhysBody3D.h"
#include "PhysVehicle3D.h"
#include "Primitive.h"

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

	collision_conf = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_conf);
	broad_phase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	debug_draw = new DebugDrawer();
}

// Destructor
ModulePhysics3D::~ModulePhysics3D()
{
	delete debug_draw;
	delete solver;
	delete broad_phase;
	delete dispatcher;
	delete collision_conf;
}

// Render not available yet----------------------------------
bool ModulePhysics3D::Init()
{
	LOG("Creating 3D Physics simulation");
	bool ret = true;

	return ret;
}

// ---------------------------------------------------------
bool ModulePhysics3D::Start()
{
	LOG("Creating Physics environment");

	world = new btDiscreteDynamicsWorld(dispatcher, broad_phase, solver, collision_conf);
	world->setDebugDrawer(debug_draw);
	world->setGravity(GRAVITY);
	vehicle_raycaster = new btDefaultVehicleRaycaster(world);

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
				p2List_item<Module*>* item = pbodyA->collision_listeners.getFirst();
				while(item)
				{
					item->data->OnCollision(pbodyA, pbodyB);
					item = item->next;
				}

				item = pbodyB->collision_listeners.getFirst();
				while(item)
				{
					item->data->OnCollision(pbodyB, pbodyA);
					item = item->next;
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

	if(debug == true)
	{
		world->debugDrawWorld();

		// Render vehicles
		p2List_item<PhysVehicle3D*>* item = vehicles.getFirst();
		while(item)
		{
			item->data->Render();
			item = item->next;
		}
	}

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
	LOG("Destroying 3D Physics simulation");

	// Remove from the world all collision bodies
	for(int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		world->removeCollisionObject(obj);
	}

	for(p2List_item<btTypedConstraint*>* item = constraints.getFirst(); item; item = item->next)
	{
		world->removeConstraint(item->data);
		delete item->data;
	}
	
	constraints.clear();

	for(p2List_item<btDefaultMotionState*>* item = motions.getFirst(); item; item = item->next)
		delete item->data;

	motions.clear();

	for(p2List_item<btCollisionShape*>* item = shapes.getFirst(); item; item = item->next)
		delete item->data;

	shapes.clear();

	for(p2List_item<PhysBody3D*>* item = bodies.getFirst(); item; item = item->next)
		delete item->data;

	bodies.clear();

	for(p2List_item<PhysVehicle3D*>* item = vehicles.getFirst(); item; item = item->next)
		delete item->data;

	vehicles.clear();

	delete vehicle_raycaster;
	delete world;

	return true;
}

// ---------------------------------------------------------
PhysBody3D* ModulePhysics3D::AddBody(const Sphere& sphere, float mass)
{
	btCollisionShape* colShape = new btSphereShape(sphere.radius);
	shapes.add(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&sphere.transform);

	btVector3 localInertia(0, 0, 0);
	if(mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.add(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody3D* pbody = new PhysBody3D(body);

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	bodies.add(pbody);

	return pbody;
}


// ---------------------------------------------------------
PhysBody3D* ModulePhysics3D::AddBody(const Cube& cube, float mass)
{
	btCollisionShape* colShape = new btBoxShape(btVector3(cube.size.x*0.5f, cube.size.y*0.5f, cube.size.z*0.5f));
	shapes.add(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&cube.transform);

	btVector3 localInertia(0, 0, 0);
	if(mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.add(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody3D* pbody = new PhysBody3D(body);

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	bodies.add(pbody);

	return pbody;
}

// ---------------------------------------------------------
PhysBody3D* ModulePhysics3D::AddBody(const Cylinder& cylinder, float mass)
{
	btCollisionShape* colShape = new btCylinderShapeX(btVector3(cylinder.height*0.5f, cylinder.radius, 0.0f));
	shapes.add(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&cylinder.transform);

	btVector3 localInertia(0, 0, 0);
	if(mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.add(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody3D* pbody = new PhysBody3D(body);

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	bodies.add(pbody);

	return pbody;
}

// ---------------------------------------------------------
PhysVehicle3D* ModulePhysics3D::AddVehicle(const VehicleInfo& info)
{
	float large = info.chassis_size.z;
	float ample = info.chassis_size.x;
	float gros = info.chassis_size.y;
	float PI = 3.141592;

	btCompoundShape* comShape = new btCompoundShape();
	shapes.add(comShape);

	btTransform trans;
	trans.setIdentity();

	btCollisionShape* base = new btBoxShape(btVector3(ample, gros, large));
	shapes.add(base);

	btCollisionShape* front = new btBoxShape(btVector3(ample, large/6, gros));
	shapes.add(front);

	btCollisionShape* cyl_base_lad = new btCylinderShape(btVector3(gros, large, 0));
	shapes.add(cyl_base_lad);

	btCollisionShape* cyl_arr_lad = new btCylinderShape(btVector3(gros, large*0.17, 0));
	shapes.add(cyl_arr_lad);

	btCollisionShape* cyl_del = new btCylinderShape(btVector3(gros, large/6, 0));
	shapes.add(cyl_del);

	btCollisionShape* cyl_tras = new btCylinderShape(btVector3(gros, 0.625*large, 0));
	shapes.add(cyl_tras);

	btCollisionShape* cyl_tras_rue = new btCylinderShape(btVector3(gros, large/7, 0));
	shapes.add(cyl_tras_rue);

	btCollisionShape* cyl_cab_tras = new btCylinderShape(btVector3(gros, 0.6*large, 0));
	shapes.add(cyl_cab_tras);

	btCollisionShape* bar_lat = new btCylinderShape(btVector3(gros, 0.35*large, 0));
	shapes.add(bar_lat);

	btCollisionShape* cyl_base_del = new btCylinderShape(btVector3(gros, ample, 0));
	shapes.add(cyl_base_del);

	btCollisionShape* cyl_base_tra = new btCylinderShape(btVector3(gros, ample*0.35, 0));
	shapes.add(cyl_base_tra);

	btCollisionShape* cyl_sup_base_tra = new btCylinderShape(btVector3(gros, ample*0.3, 0));
	shapes.add(cyl_sup_base_tra);

	btCollisionShape* lateral = new btCylinderShape(btVector3(gros, large, 0));
	shapes.add(lateral);

	btCollisionShape* barra_techo = new btCylinderShape(btVector3(gros, 2 * large / 3, 0));
	shapes.add(barra_techo);

	btCollisionShape* barra = new btCylinderShape(btVector3(gros, large/3, 0));
	shapes.add(barra);

	btCollisionShape* esquina = new btSphereShape(gros);
	shapes.add(esquina);

	btCollisionShape* barra_del = new btCylinderShape(btVector3(gros, large - 0.08, 0));
	shapes.add(barra_del);

	//---------------------------------------------------CUBOS-----------------------------------------------------------------------------------------------

	trans.setOrigin(btVector3(0, large / 6 + info.chassis_offset.y , large));
	comShape->addChildShape(trans, front);

	trans.setRotation(btQuaternion(btVector3(1, 0, 0), 5 * PI / 180));

	trans.setOrigin(btVector3(0, 2 * large / 4.75 + info.chassis_offset.y, 0));
	comShape->addChildShape(trans, base);

	//---------------------------------------------------elevators-----------------------------------------------------------------------------------------------
	trans.setRotation(btQuaternion(btVector3(1, 0, 0), PI / 2));

	btCollisionShape* cuerno = new btBoxShape(btVector3(ample*0.2f, gros / 4, large + 1.0f));
	shapes.add(cuerno);
	btCollisionShape* lim = new btBoxShape(btVector3(ample*0.2f, gros*2, gros));
	shapes.add(lim);

	trans.setOrigin(btVector3(ample - 0.2f, info.chassis_offset.y*3 + large, large*2 -1.8f));
	comShape->addChildShape(trans, cuerno);

	trans.setOrigin(btVector3(ample - 0.2f, -info.chassis_offset.y*6, large * 2 - 1.8f));
	comShape->addChildShape(trans, lim);

	trans.setOrigin(btVector3(ample - 0.2f, 2*large, large * 2 - 1.8));
	comShape->addChildShape(trans, lim);


	trans.setOrigin(btVector3(-ample + 0.2f, info.chassis_offset.y * 3 + large, large*2 -1.8f));
	comShape->addChildShape(trans, cuerno);

	trans.setOrigin(btVector3(-ample + 0.2f, -info.chassis_offset.y*6, large * 2 - 1.8f));
	comShape->addChildShape(trans, lim);

	trans.setOrigin(btVector3(-ample + 0.2f, 2*large, large * 2 - 1.8f));
	comShape->addChildShape(trans, lim);
	
	//---------------------------------------------------CILINDROS Z-------------------------------------------------------------------------------------------
	trans.setRotation(btQuaternion(btVector3(1, 0, 0), PI / 2));

	trans.setOrigin(btVector3(ample, 0 + info.chassis_offset.y, 0));
	comShape->addChildShape(trans, cyl_base_lad);

	trans.setOrigin(btVector3(-ample, 0 + info.chassis_offset.y, 0));
	comShape->addChildShape(trans, cyl_base_lad);
	
	trans.setOrigin(btVector3(ample, 2 * 0.625f*large + info.chassis_offset.y, -large + large*0.17f));
	comShape->addChildShape(trans, cyl_arr_lad);
	
	trans.setOrigin(btVector3(-ample, 2 * 0.625f*large + info.chassis_offset.y, -large + large*0.17f));
	comShape->addChildShape(trans, cyl_arr_lad);

//---------------------------------------------------CILINDROS Y-------------------------------------------------------------------------------------------
	trans.setRotation(btQuaternion(btVector3(0, 1, 0), PI/2));

	trans.setOrigin(btVector3(ample, large / 6 + info.chassis_offset.y, large));
	comShape->addChildShape(trans, cyl_del);

	trans.setOrigin(btVector3(-ample, large / 6 + info.chassis_offset.y, large));
	comShape->addChildShape(trans, cyl_del);

	trans.setOrigin(btVector3(ample, 0.625f*large + info.chassis_offset.y, -large));
	comShape->addChildShape(trans, cyl_tras);

	trans.setOrigin(btVector3(-ample, 0.625f*large + info.chassis_offset.y, -large));
	comShape->addChildShape(trans, cyl_tras);

	trans.setOrigin(btVector3(ample * 0.3f, large / 7 + info.chassis_offset.y, -large));
	comShape->addChildShape(trans, cyl_tras_rue);

	trans.setOrigin(btVector3(-ample * 0.3f, large / 7 + info.chassis_offset.y, -large));
	comShape->addChildShape(trans, cyl_tras_rue);

	trans.setOrigin(btVector3(ample, 1.8f * large + info.chassis_offset.y, -large + 2 * large*0.17f));
	comShape->addChildShape(trans, cyl_cab_tras);

	trans.setOrigin(btVector3(-ample, 1.8f * large + info.chassis_offset.y, -large + 2 * large*0.17f));
	comShape->addChildShape(trans, cyl_cab_tras);

	trans.setOrigin(btVector3(ample, 2 * large / 2.55f + info.chassis_offset.y, 0));
	comShape->addChildShape(trans, bar_lat);

	trans.setOrigin(btVector3(-ample, 2 * large / 2.55f + info.chassis_offset.y, 0));
	comShape->addChildShape(trans, bar_lat);

	//---------------------------------------------------CILINDROS X-------------------------------------------------------------------------------------------
	trans.setRotation(btQuaternion(btVector3(0, 0, 1), PI / 2));

	trans.setOrigin(btVector3(0, 0 + info.chassis_offset.y, large));
	comShape->addChildShape(trans, cyl_base_del);

	trans.setOrigin(btVector3(0, 2 * 0.625f*large + info.chassis_offset.y, -large));
	comShape->addChildShape(trans, cyl_base_del);

	trans.setOrigin(btVector3(0, 2 * 0.625f*large + info.chassis_offset.y, -large + 2 * large*0.17f));
	comShape->addChildShape(trans, cyl_base_del);

	trans.setOrigin(btVector3(0, 2.4f * large + info.chassis_offset.y, -large + 2 * large*0.17f));
	comShape->addChildShape(trans, cyl_base_del);

	trans.setOrigin(btVector3(ample - ample*0.35f, 0 + info.chassis_offset.y, -large));
	comShape->addChildShape(trans, cyl_base_tra);

	trans.setOrigin(btVector3(-ample + ample*0.35f, 0 + info.chassis_offset.y, -large));
	comShape->addChildShape(trans, cyl_base_tra);

	trans.setOrigin(btVector3(0, 2 * large / 7 + info.chassis_offset.y, -large));
	comShape->addChildShape(trans, cyl_sup_base_tra);

	trans.setOrigin(btVector3(0, 2 * large / 6 + info.chassis_offset.y, large));
	comShape->addChildShape(trans, cyl_base_del);

	trans.setOrigin(btVector3(0, 2 * large / 0.877f + info.chassis_offset.y, large - large / 3));
	comShape->addChildShape(trans, cyl_base_del);

	//---------------------------------------------------CILINDROS ROTADOS-------------------------------------------------------------------------------------------
	trans.setRotation(btQuaternion(btVector3(1, 0, 0), -85*PI/180));

	trans.setOrigin(btVector3(ample, 2 * large / 4.75f + info.chassis_offset.y, 0));
	comShape->addChildShape(trans, lateral);

	trans.setOrigin(btVector3(-ample, 2 * large / 4.75f + info.chassis_offset.y, 0));
	comShape->addChildShape(trans, lateral);

	trans.setOrigin(btVector3(ample, 2 * large / 0.855f + info.chassis_offset.y, 0));
	comShape->addChildShape(trans, barra_techo);

	trans.setOrigin(btVector3(-ample, 2 * large / 0.855f + info.chassis_offset.y, 0));
	comShape->addChildShape(trans, barra_techo);

	trans.setRotation(btQuaternion(btVector3(1, 0, 0), -80 * PI / 180));

	trans.setOrigin(btVector3(ample, 2 * large / 1.68f + info.chassis_offset.y, -large / 3));
	comShape->addChildShape(trans, barra);

	trans.setOrigin(btVector3(-ample, 2 * large / 1.68f + info.chassis_offset.y, -large / 3));
	comShape->addChildShape(trans, barra);

	trans.setRotation(btQuaternion(btVector3(1, 0, 0), -9.8 * PI / 180));

	trans.setOrigin(btVector3(ample, 2 * large / 1.52f + info.chassis_offset.y, large - large / 6));
	comShape->addChildShape(trans, barra_del);
	
	trans.setOrigin(btVector3(-ample, 2 * large / 1.52f + info.chassis_offset.y, large - large / 6));
	comShape->addChildShape(trans, barra_del);
	
	//---------------------------------------------------ESQUINAS-------------------------------------------------------------------------------------------
	trans.setRotation(btQuaternion(btVector3(1, 0, 0),PI));

	trans.setOrigin(btVector3(ample, 0 + info.chassis_offset.y, large));
	comShape->addChildShape(trans, esquina);

	trans.setOrigin(btVector3(-ample, 0 + info.chassis_offset.y, large));
	comShape->addChildShape(trans, esquina);

	trans.setOrigin(btVector3(ample, 0 + info.chassis_offset.y, -large));
	comShape->addChildShape(trans, esquina);

	trans.setOrigin(btVector3(-ample, 0 + info.chassis_offset.y, -large));
	comShape->addChildShape(trans, esquina);

	trans.setOrigin(btVector3(-ample * 0.3, 0 + info.chassis_offset.y, -large));
	comShape->addChildShape(trans, esquina);

	trans.setOrigin(btVector3(ample * 0.3, 0 + info.chassis_offset.y, -large));
	comShape->addChildShape(trans, esquina);

	trans.setOrigin(btVector3(ample, 2 * large / 6 + info.chassis_offset.y, large));
	comShape->addChildShape(trans, esquina);

	trans.setOrigin(btVector3(-ample, 2 * large / 6 + info.chassis_offset.y, large));
	comShape->addChildShape(trans, esquina);

	trans.setOrigin(btVector3(ample, 2 * 0.625f*large + info.chassis_offset.y, -large));
	comShape->addChildShape(trans, esquina);

	trans.setOrigin(btVector3(-ample, 2 * 0.625f*large + info.chassis_offset.y, -large));
	comShape->addChildShape(trans, esquina);

	trans.setOrigin(btVector3(ample, 2.4*large + info.chassis_offset.y, -large + 2 * large*0.17f));
	comShape->addChildShape(trans, esquina);

	trans.setOrigin(btVector3(-ample, 2.4*large + info.chassis_offset.y, -large + 2 * large*0.17f));
	comShape->addChildShape(trans, esquina);

	trans.setOrigin(btVector3(ample, 2 * large / 1.765f + info.chassis_offset.y, 0));
	comShape->addChildShape(trans, esquina);

	trans.setOrigin(btVector3(-ample, 2 * large / 1.765f + info.chassis_offset.y, 0));
	comShape->addChildShape(trans, esquina);

	trans.setOrigin(btVector3(ample, 2 * large / 0.877f + info.chassis_offset.y, large - large / 3));
	comShape->addChildShape(trans, esquina);

	trans.setOrigin(btVector3(-ample, 2 * large / 0.877f + info.chassis_offset.y, large - large / 3));
	comShape->addChildShape(trans, esquina);

	btTransform startTransform;
	startTransform.setIdentity();

	btVector3 localInertia(0, 0, 0);
	comShape->calculateLocalInertia(info.mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(info.mass, myMotionState, comShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	body->setContactProcessingThreshold(BT_LARGE_FLOAT);
	body->setActivationState(DISABLE_DEACTIVATION);

	world->addRigidBody(body);

	btRaycastVehicle::btVehicleTuning tuning;
	tuning.m_frictionSlip = info.frictionSlip;
	tuning.m_maxSuspensionForce = info.maxSuspensionForce;
	tuning.m_maxSuspensionTravelCm = info.maxSuspensionTravelCm;
	tuning.m_suspensionCompression = info.suspensionCompression;
	tuning.m_suspensionDamping = info.suspensionDamping;
	tuning.m_suspensionStiffness = info.suspensionStiffness;

	btRaycastVehicle* vehicle = new btRaycastVehicle(tuning, body, vehicle_raycaster);

	vehicle->setCoordinateSystem(0, 1, 2);

	for(int i = 0; i < info.num_wheels; ++i)
	{
		btVector3 conn(info.wheels[i].connection.x, info.wheels[i].connection.y, info.wheels[i].connection.z);
		btVector3 dir(info.wheels[i].direction.x, info.wheels[i].direction.y, info.wheels[i].direction.z);
		btVector3 axis(info.wheels[i].axis.x, info.wheels[i].axis.y, info.wheels[i].axis.z);

		vehicle->addWheel(conn, dir, axis, info.wheels[i].suspensionRestLength, info.wheels[i].radius, tuning, info.wheels[i].front);
	}
	// ---------------------

	PhysVehicle3D* pvehicle = new PhysVehicle3D(body, vehicle, info);
	world->addVehicle(vehicle);
	vehicles.add(pvehicle);

	return pvehicle;
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
	constraints.add(p2p);
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
	constraints.add(hinge);
	hinge->setDbgDrawSize(2.0f);
}

void ModulePhysics3D::AddConstraintSlider(PhysBody3D& bodyA, PhysBody3D& bodyB, const btTransform& A, const btTransform& B, bool useLinearReferenceFrameA)
{
	btSliderConstraint* slider = new btSliderConstraint(*(bodyA.body), *(bodyB.body), A, B, useLinearReferenceFrameA);

	world->addConstraint(slider, useLinearReferenceFrameA);
	constraints.add(slider);
	slider->setDbgDrawSize(2.0f);
}



// =============================================
void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	line.origin.Set(from.getX(), from.getY(), from.getZ());
	line.destination.Set(to.getX(), to.getY(), to.getZ());
	line.color.Set(color.getX(), color.getY(), color.getZ());
	line.Render();
}

void DebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	point.transform.translate(PointOnB.getX(), PointOnB.getY(), PointOnB.getZ());
	point.color.Set(color.getX(), color.getY(), color.getZ());
	point.Render();
}

void DebugDrawer::reportErrorWarning(const char* warningString)
{
	LOG("Bullet warning: %s", warningString);
}

void DebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
	LOG("Bullet draw text: %s", textString);
}

void DebugDrawer::setDebugMode(int debugMode)
{
	mode = (DebugDrawModes) debugMode;
}

int	 DebugDrawer::getDebugMode() const
{
	return mode;
}
