#include "Bullet.h"

Bullet::Bullet()
{
	InitDynamicsWorld();
}

Bullet::~Bullet()
{
	DleateDynamicsWorld();
}

void Bullet::InitDynamicsWorld()
{
	if (!dynamicsWorld)
	{
		collisionConfiguration = new btDefaultCollisionConfiguration();

		dispatcher = new btCollisionDispatcher(collisionConfiguration);

		overlappingPairCache = new btDbvtBroadphase();

		solver = new btSequentialImpulseConstraintSolver;

		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

		dynamicsWorld->setGravity(btVector3(0, -10, 0));
	}
}

void Bullet::DleateDynamicsWorld()
{
	if (dynamicsWorld)
	{
		for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body && body->getMotionState())
			{
				delete body->getMotionState();
			}
			dynamicsWorld->removeCollisionObject(obj);
			delete obj;
		}

		for (int j = 0; j < collisionShapes.size(); j++)
		{
			btCollisionShape* shape = collisionShapes[j];
			collisionShapes[j] = 0;
			delete shape;
		}

		delete dynamicsWorld;

		dynamicsWorld = 0;

		delete solver;

		delete overlappingPairCache;

		delete dispatcher;

		delete collisionConfiguration;

		collisionShapes.clear();
	}
}

btRigidBody* Bullet::CreateShape(btCollisionShape* shape, const btVector3& position, const btQuaternion& rotation, btScalar mass, btScalar restitution)
{
	collisionShapes.push_back(shape);
	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(position);
	trans.setRotation(rotation);
	btVector3 localInertia(0, 0, 0);
	if (mass != 0) shape->calculateLocalInertia(mass, localInertia);
	btDefaultMotionState* motionState = new btDefaultMotionState(trans);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	body->setRestitution(restitution);
	dynamicsWorld->addRigidBody(body);
	return body;
}