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

// このコンポーネントの更新処理
void ColliderComponent::Update()
{
	SetVerticesNormals();
}

// 頂点法線情報更新
void ColliderComponent::SetVerticesNormals()
{
}

VECTOR ColliderComponent::GetRotation()
{
	return mOwner->GetRotation();
}

VECTOR ColliderComponent::GetCenter()
{
	// 頂点回転
	MATRIX rotationX;
	MATRIX rotationY;
	MATRIX rotationZ;

	rotationX.rotateX(ConvertToRadians(mOwner->GetRotation().x));
	rotationY.rotateY(ConvertToRadians(mOwner->GetRotation().y));
	rotationZ.rotateZ(ConvertToRadians(mOwner->GetRotation().z));

	// 位置
	MATRIX basePosition;
	basePosition.translate(mPositionGap.x, mPositionGap.y, mPositionGap.z);
	basePosition = rotationZ * rotationX * rotationY * basePosition;
	VECTOR rotationPos = VECTOR(basePosition._14, basePosition._24, basePosition._34);

	return  mOwner->GetPosition() + rotationPos;
}