#include "ColliderComponent.h"
#include "Actor.h"
#include "PointLineComponent.h"
#include "../MATRIX.h"
ColliderComponent::ColliderComponent(Actor* owner, int order)
	:Component(owner, order)
	, mHeight(1)
	, mRadius(1)
	, mSaveRota(-1, -1, -1)
	, mSaveHeight(1)
	, mSaveRadius(1)
	, mSaveState(GameState::ETitle)
	, mMaxDistance(0)
{
}

ColliderComponent::~ColliderComponent()
{
	for (auto f : mFaces)
	{
		delete f;
	}
}

// ���̃R���|�[�l���g�̍X�V����
void ColliderComponent::Update()
{
	SetVerticesNormals();
}

// ���_�@�����X�V
void ColliderComponent::SetVerticesNormals()
{
}

VECTOR ColliderComponent::GetRotation()
{
	return mOwner->GetRotation();
}

VECTOR ColliderComponent::GetCenter()
{
	// ���_��]
	MATRIX rotationX;
	MATRIX rotationY;
	MATRIX rotationZ;

	rotationX.rotateX(ConvertToRadians(mOwner->GetRotation().x));
	rotationY.rotateY(ConvertToRadians(mOwner->GetRotation().y));
	rotationZ.rotateZ(ConvertToRadians(mOwner->GetRotation().z));

	// �ʒu
	MATRIX basePosition;
	basePosition.translate(mPositionGap.x, mPositionGap.y, mPositionGap.z);
	basePosition = rotationZ * rotationX * rotationY * basePosition;
	VECTOR rotationPos = VECTOR(basePosition._14, basePosition._24, basePosition._34);

	return  mOwner->GetPosition() + rotationPos;
}