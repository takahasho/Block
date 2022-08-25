#pragma once
#include "MoveComponent.h"
#include "../Bullet/Bullet.h"
class RigidBodyComponent :
	public MoveComponent
{
public:
	RigidBodyComponent(class Actor* owner);
	void AddRigidBody();				// RigidBodyの情報更新
private:
	void Update() override;
private:
	btRigidBody* mRigidBody;		// 物理エンジンから受け取る情報
	btVector3 mInitialPosition;
	btQuaternion mInitialRotation;
	float mMass;				// 質量
};