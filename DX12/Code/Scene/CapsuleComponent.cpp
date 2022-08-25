#include "CapsuleComponent.h"
#include "Game.h"
#include "Actor.h"
#include "PointLineComponent.h"
#include "../GameObject/MeshCreate.h"
#include "../MATRIX.h"

CapsuleComponent::CapsuleComponent(Actor* owner, int order)
	:ColliderComponent(owner, order)
{
	MeshData meshData = CreateCapsule(10, 10,mHeight,mRadius);

	mBaseVertices.resize(meshData.indices.size());
	for (int i = 0; i < mBaseVertices.size(); i++)
	{
		mBaseVertices[i] = meshData.vertices[meshData.indices[i]];
		float distance = mBaseVertices[i].mag();
		if (distance > mMaxDistance)
		{
			mMaxDistance = distance;
		}
	}


}
// 頂点法線情報更新
void CapsuleComponent::SetVerticesNormals()
{
	GameState state = mOwner->GetGame()->GetGameState();
	// 情報に変更があれば
	if (mSavePos != mOwner->GetPosition() || mSaveRadius != mRadius || mSaveHeight != mHeight || mSaveRota != mOwner->GetRotation() || mPositionGap!=mSavePosGap|| mSaveState != state)
	{
		if (mSaveRadius != mRadius || mSaveHeight != mHeight)
		{
			MeshData meshData = CreateCapsule(10, 10, mHeight, mRadius);

			mBaseVertices.resize(meshData.indices.size());
			for (int i = 0; i < mBaseVertices.size(); i++)
			{
				mBaseVertices[i] = meshData.vertices[meshData.indices[i]];
			}
		}
	}
}