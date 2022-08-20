#pragma once
#include "MoveComponent.h"

class RigidBodyComponent :
	public MoveComponent
{
public:
	RigidBodyComponent(class Actor* owner);
private:
	void Update() override;
	class ColliderComponent* UseCollider(class Actor* actor);       // actor���g���Ă���R���C�_�[
	void PhysicalCalculus(ColliderComponent* collider);             // �ǂ���x�N�^�[�v�Z
private:
	float mGravity;				// �d��
	float mMass;				// ����
	VECTOR mPrevVector;         // �O�t���[���̐i�s�x�N�g��
	float mFreeFallTime;		// ���R��������
};