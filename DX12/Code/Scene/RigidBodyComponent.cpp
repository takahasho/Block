#include "RigidBodyComponent.h"
#include "../Input/DInput.h"
#include "Actor.h"
#include "../VECTOR.h"
#include "Game.h"
#include "ColliderCalculation.h"
#include "BoxComponent.h"
#include "CapsuleComponent.h"
#include "ModelMeshComponent.h"
RigidBodyComponent::RigidBodyComponent(Actor* owner)
	:MoveComponent(owner), mGravity(0.098), mMass(2), mPrevVector(VECTOR(0, 0, 0)), mFreeFallTime(0)
{

}

void RigidBodyComponent::Update()
{
	if (mOwner->GetGame()->GetGameState() == GameState::EPlay)
	{
		ColliderComponent* collider = UseCollider(mOwner);
		if (collider)
		{
			PhysicalCalculus(collider);
		}
	}
	// �^�C�g���A�N���G�C�e�B�u���[�h�ŏ�����
	else if (mOwner->GetGame()->GetGameState() == GameState::ECreative || mOwner->GetGame()->GetGameState() == GameState::ETitle)
	{
		mFreeFallTime = 0;
		mJumpVector = 0;
		mOwner->SetJumpFlg(false);
		mOwner->SetProgressVector(VECTOR(0, 0, 0));
	}
}
// actor���g���Ă���R���C�_�[
ColliderComponent* RigidBodyComponent::UseCollider(Actor* actor)
{
	// �ǂ̃R���|�[�l���g�����t�����Ă��邩
	if (actor->GetBoxComponent())
	{
		return actor->GetBoxComponent();
	}
	else if (actor->GetCapsuleComponent())
	{
		return actor->GetCapsuleComponent();
	}
	else if (actor->GetModelMeshComponent())
	{
		return actor->GetModelMeshComponent();
	}
	return nullptr;
}
// �ǂ���x�N�^�[�v�Z
void RigidBodyComponent::PhysicalCalculus(ColliderComponent* collider)
{
	// ��������
	mFreeFallTime += g_elapsedTime;

	// �v���C���[�̌��ݒn
	VECTOR oldPos = mOwner->GetPosition();

	// �i�s�x�N�g��
	VECTOR ProgressVector = mOwner->GetProgressVector();

	// �W�����v�{�^���������ꂽ��
	if (mOwner->GetJumpFlg())
	{
		float saveVec = mJumpVector;
		mJumpVector = mFreeFallTime * mJumpPower;

		ProgressVector += VECTOR(0, mJumpVector - saveVec, 0);
	}

	ProgressVector += VECTOR(0, -(mGravity * mMass * mFreeFallTime * mFreeFallTime), 0);
	// �v���C���[�̎��̃t���[���̈ʒu
	VECTOR nowPos = mOwner->GetPosition() + ProgressVector;

	// --------- �ǂ���x�N�g�� -----------
	
	if (ProgressVector != VECTOR(0, 0, 0))
	{
		// ���̃t���[���Ői�ސi�s�x�N�g����������
		mOwner->SetPosition(nowPos);
		VECTOR rotation;
		ColliderComponent* OpponentjCollider;
		// ---------- �I�u�W�F�N�g�Ƃ̓����蔻�� ----------
		for (Actor* actor : mOwner->GetGame()->GetActors())
		{
			if (actor != mOwner)
			{
				OpponentjCollider = UseCollider(actor);
				if (OpponentjCollider)
				{
					// ��̃I�u�W�F�N�g�̋���
					float distance = distance_of(collider->GetCenter(), OpponentjCollider->GetCenter());
					if (distance < (collider->GetMaxDistance() + OpponentjCollider->GetMaxDistance()))
					{
						OpponentjCollider->SetVerticesNormals();
						collider->SetVerticesNormals();
						HitCheck_Collider(collider, OpponentjCollider, ProgressVector);
					}
				}
			}
		}
		//if (rotation != VECTOR(0, 0, 0))
		//{
		//	VECTOR r = mOwner->GetRotation();
		//	rotation = r + rotation;
		//	mOwner->SetRotation(rotation);
		//}
		// �i�߂��x�N�g�����߂�
		mOwner->SetPosition(oldPos);

		// �i�s�x�N�g�������肷��
		mOwner->SetProgressVector(ProgressVector);
	}

	// �������Ă��Ȃ�������0
	if (ProgressVector.y == 0)
	{
		mFreeFallTime = 0;
		mJumpVector = 0;
		mOwner->SetJumpFlg(false);
	}

	//	// �قڐ^�t�̖@���x�N�g��������΍폜
	//	int i;
	//	int j;
	//	for (i = 0; i < normals.size(); i++)
	//	{
	//		for (j = 0; j < normals.size(); j++)
	//		{
	//			if (i != j)
	//			{
	//				VECTOR difference = normals[i] + normals[j];
	//				// �����Ȍ덷�ł��폜
	//				if (difference.mag() < 0.00001f)
	//				{
	//					// �R�s�[���Ƃ��đS�Ă̗v�f����������
	//					std::vector<VECTOR> Normals = normals;
	//					normals.clear();
	//					for (int k = 0; k < Normals.size(); k++)
	//					{
	//						// �^�t�̖@���x�N�g����ȊO��ς�
	//						if (k != i && k != j)
	//						{
	//							normals.push_back(Normals[k]);
	//						}
	//					}
	//					i = 0;
	//					j = 0;
	//				}
	//			}
	//		}
	//	}

	//	// ---------- �@�������Ƃɕǂ���x�N�g������� ----------
	//	if (normals.size() > 0)
	//	{
	//		for (auto nor : normals)
	//		{
	//			if (dot(ProgressVector, nor) < 0)
	//			{
	//				ProgressVector = cross(ProgressVector, nor);
	//				ProgressVector = cross(nor, ProgressVector);
	//			}
	//		}
	//	}

	//	// �i�߂��x�N�g�����߂�
	//	mOwner->SetPosition(oldPos);


	// }
}