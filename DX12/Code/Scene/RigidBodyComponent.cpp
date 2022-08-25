#include "RigidBodyComponent.h"
#include "Actor.h"
#include "Game.h"
#include "BoxComponent.h"
#include "CapsuleComponent.h"
#include "ModelMeshComponent.h"
#include "../VECTOR.h"

RigidBodyComponent::RigidBodyComponent(Actor* owner)
	:MoveComponent(owner), mMass(2), mRigidBody()
{
	btVector3 pos = btVector3(btScalar(mOwner->GetPosition().x), btScalar(mOwner->GetPosition().y), btScalar(mOwner->GetPosition().z));
	mInitialPosition = pos;
	btQuaternion qrot;
	qrot.setEulerZYX(XMConvertToRadians(mOwner->GetRotation().z), XMConvertToRadians(mOwner->GetRotation().y), XMConvertToRadians(mOwner->GetRotation().x));
	mInitialRotation = qrot;
}

void RigidBodyComponent::Update()
{
	if (mOwner->GetGame()->GetGameState() == GameState::EPlay)
	{
		auto p = mOwner->GetPosition();
		btTransform trans;
		if (mRigidBody && mRigidBody->getMotionState())
		{
			mRigidBody->getMotionState()->getWorldTransform(trans);
		}
		else
		{
			trans = mRigidBody->getWorldTransform();
		}
		VECTOR pos = VECTOR(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
		mOwner->SetPosition(pos);
		btScalar x;
		btScalar y;
		btScalar z;
		trans.getRotation().getEulerZYX(z, y, x);
		VECTOR rotation = VECTOR(XMConvertToDegrees(x), XMConvertToDegrees(y), XMConvertToDegrees(z));
		mOwner->SetRotation(rotation);
	}
	else if (mOwner->GetGame()->GetGameState() == GameState::ECreative)
	{
		btVector3 pos = btVector3(btScalar(mOwner->GetPosition().x), btScalar(mOwner->GetPosition().y), btScalar(mOwner->GetPosition().z));
		mInitialPosition = pos;
		btQuaternion qrot;
		qrot.setEulerZYX(XMConvertToRadians(mOwner->GetRotation().z), XMConvertToRadians(mOwner->GetRotation().y), XMConvertToRadians(mOwner->GetRotation().x));
		mInitialRotation = qrot;
	}
}

void RigidBodyComponent::AddRigidBody()
{
	btCollisionShape* shape = {};
	if (mOwner->GetBoxComponent())
	{
		shape = new btBoxShape(btVector3(btScalar(mOwner->GetScale().x / 2), btScalar(mOwner->GetScale().y / 2), btScalar(mOwner->GetScale().z / 2)));
	}
	else if (mOwner->GetCylinderComponent())
	{
		shape = new btCylinderShapeZ(btVector3(btScalar(mOwner->GetScale().x / 2), btScalar(mOwner->GetScale().y / 2), btScalar(mOwner->GetScale().z / 2)));
	}
	else if (mOwner->GetCapsuleComponent())
	{
		shape = new btCapsuleShape(btScalar(mOwner->GetCapsuleComponent()->GetRadius() / 2), btScalar(mOwner->GetCapsuleComponent()->GetHeight() / 2));
	}
	else if (mOwner->GetModelMeshComponent())
	{
		std::vector<VECTOR> vertices = mOwner->GetModelMeshComponent()->UpdateVertex();
		btConvexHullShape* convexHullShape = new btConvexHullShape();
		btScalar scaling(1);

		convexHullShape->setLocalScaling(btVector3(scaling, scaling, scaling));

		for (VECTOR vertex:vertices)
		{
			btVector3 vtx(vertex.x, vertex.y, vertex.z);			
			convexHullShape->addPoint(vtx * btScalar(1. / scaling));
		}

		convexHullShape->initializePolyhedralFeatures();

		float mass = 1.f;
		btVector3 localInertia(0, 0, 0);
		convexHullShape->calculateLocalInertia(mass, localInertia);

		shape = convexHullShape;
	}
	if (mOwner->GetPosition().y == -0.5f)
	{
		mRigidBody = mOwner->GetGame()->GetBullet()->CreateShape(shape, mInitialPosition, mInitialRotation, btScalar(0.0f), btScalar(1));
	}
	else
	{
		mRigidBody = mOwner->GetGame()->GetBullet()->CreateShape(shape, mInitialPosition, mInitialRotation, btScalar(0.03f), btScalar(0.5f));
	}
}