#pragma once
#include "MoveComponent.h"
#include "../Bullet/Bullet.h"
class RigidBodyComponent :
	public MoveComponent
{
public:
	RigidBodyComponent(class Actor* owner);
	void AddRigidBody();				// RigidBody�̏��X�V
private:
	void Update() override;
private:
	btRigidBody* mRigidBody;		// �����G���W������󂯎����
	btVector3 mInitialPosition;
	btQuaternion mInitialRotation;
	float mMass;				// ����
};