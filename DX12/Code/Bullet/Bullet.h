#pragma once
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/BroadphaseCollision/btDbvtBroadphase.h"
#include "BulletCollision/CollisionDispatch/btSimulationIslandManager.h"

class Bullet sealed
{
public:
	Bullet();
	~Bullet();

	// çÑëÃê∂ê¨
	btRigidBody* CreateShape(btCollisionShape* shape, const btVector3& position, const btQuaternion& rotation, btScalar mass, btScalar restitution);

	btDiscreteDynamicsWorld* GetDynamicsWorld() { return dynamicsWorld; }
	void InitDynamicsWorld();
	void DleateDynamicsWorld();
private:
	btDefaultCollisionConfiguration* collisionConfiguration;
	
	btCollisionDispatcher* dispatcher;
	
	btBroadphaseInterface* overlappingPairCache;

	btSequentialImpulseConstraintSolver* solver;

	btDiscreteDynamicsWorld* dynamicsWorld;

	btAlignedObjectArray<btCollisionShape*> collisionShapes;
};