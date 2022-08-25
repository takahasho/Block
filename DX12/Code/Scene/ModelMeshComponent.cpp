#include "ModelMeshComponent.h"
#include "Game.h"
#include "Actor.h"
#include "PointLineComponent.h"
#include "ModelComponent.h"
#include "../MATRIX.h"

ModelMeshComponent::ModelMeshComponent(Actor* owner, int order)
	:ColliderComponent(owner, order)
{
}

 // 頂点情報更新
std::vector<VECTOR>ModelMeshComponent::UpdateVertex()
{
	// 頂点重複削除
	std::vector<VECTOR> vertices = mOwner->GetModelComponent()->GetModelVertex();
	for (VECTOR vertex : vertices)
	{
		bool flg = false;
		for (VECTOR baseVertex : mBaseVertices)
		{
			if (vertex == baseVertex)
			{
				flg = true;
				break;
			}
		}
		if (!flg)
		{
			mBaseVertices.push_back(vertex);
		}
	}
	return mBaseVertices;
}