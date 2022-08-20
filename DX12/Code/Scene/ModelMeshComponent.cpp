#include "ModelMeshComponent.h"
#include "Game.h"
#include "Actor.h"
#include "PointLineComponent.h"
#include "ModelComponent.h"
#include "../MATRIX.h"

ModelMeshComponent::ModelMeshComponent(Actor* owner, int order)
	:ColliderComponent(owner, order)
{
	mBaseVertices = mOwner->GetModelComponent()->GetModelVertex();
	for (int i = 0; i < mBaseVertices.size(); i++)
	{
		float distance = mBaseVertices[i].mag();
		if (distance > mMaxDistance)
		{
			mMaxDistance = distance;
		}
	}
	mVertices.resize(mBaseVertices.size());
	int num = (int)mBaseVertices.size() / 3;
	mFaces.resize(num);
	for (int i = 0; i < mFaces.size(); i++)
	{
		mFaces[i] = new FaceInfo;
	}
	// Colliderタイプ設定
	mType = ColliderType::ModelMesh;
	// 中心座標更新
	GetCenter();
}
// 頂点法線情報更新
void  ModelMeshComponent::SetVerticesNormals()
{
	GameState state = mOwner->GetGame()->GetGameState();
	// 情報に変更があれば
	if (mSavePos != mOwner->GetPosition()||mSaveSize != mOwner->GetScale() || mSaveRota != mOwner->GetRotation() || mSaveState != state)
	{
		// 頂点回転
		XMMATRIX world_matrix;
		XMMATRIX translate = XMMatrixTranslation(mOwner->GetPosition().x, mOwner->GetPosition().y, mOwner->GetPosition().z);
		XMMATRIX rotate_x = XMMatrixRotationX(XMConvertToRadians(mOwner->GetRotation().x));
		XMMATRIX rotate_y = XMMatrixRotationY(XMConvertToRadians(mOwner->GetRotation().y));
		XMMATRIX rotate_z = XMMatrixRotationZ(XMConvertToRadians(mOwner->GetRotation().z));
		XMMATRIX scale_mat = XMMatrixScaling(mOwner->GetScale().x, mOwner->GetScale().y, mOwner->GetScale().z);
		world_matrix = scale_mat * rotate_z * rotate_x * rotate_y * translate;

		MATRIX mat;
		mat = world_matrix;
		// 頂点情報
		for (int i = 0, size = (int)mVertices.size(); i < size; ++i)
		{
			// サイズ変更
			mVertices[i] = mat * mBaseVertices[i];
		}

		// 面情報
		for (int i = 0, size = (int)mFaces.size(); i < size; ++i)
		{
			for (int k = 0; k < polygonNum; k++)
			{
				int num = i * polygonNum + k;
				mFaces[i]->vertices[k] = mVertices[num];
			}
			mFaces[i]->UpdateCenter();
		}

		// 回転していたら法線の更新
		if (mSaveRota != mOwner->GetRotation())
		{
			for (int i = 0, size = (int)mFaces.size(); i < size; ++i)
			{
				mFaces[i]->UpdateNormal();
			}
		}

		// サイズが変わったら原点から頂点迄の距離も面積も変わる
		if (mSaveSize != mOwner->GetScale())
		{
			for (int i = 0, size = (int)mVertices.size(); i < size; ++i)
			{
				float distance = distance_of(GetCenter(), mVertices[i]);
				if (distance > mMaxDistance)
				{
					mMaxDistance = distance;
				};
			}
			for (int i = 0, size = (int)mFaces.size(); i < size; ++i)
			{
				mFaces[i]->UpdateDistance();
				mFaces[i]->UpdateArea();
			}
		}

		// Line

		if (state == GameState::ECreative)
		{
			std::vector<LineData> data = {};
			for (int i = 0, size = (int)mFaces.size(); i < size; ++i)
			{
				for (int k = 0; k < polygonNum; ++k)
				{
					// 重複する線を削除
					bool flg = false;
					int nextNum = k + 1;
					if (nextNum >= 3)
					{
						nextNum = 0;
					}
					for (LineData d : data)
					{
						if (d.startPos == mFaces[i]->vertices[k] ||
							d.startPos == mFaces[i]->vertices[nextNum])
						{
							if (d.endPos == mFaces[i]->vertices[nextNum] ||
								d.endPos == mFaces[i]->vertices[k])
							{
								flg = true;
								break;
							}
						}
					}
					if (flg)
					{
						continue;
					}
					LineData linedata;
					linedata.startPos = mFaces[i]->vertices[k];
					linedata.endPos = mFaces[i]->vertices[nextNum];
					linedata.color = green;
					data.push_back(linedata);
				}
			}
			// PointLineComponentに入れる			
			mLine->resizeLineData((int)data.size());
			for (int i = 0, size = (int)data.size(); i < size; ++i)
			{
				mLine->SetLineData(i, data[i]);
			}
			mLine->SetLine();
		}
		else if (mSaveState != state)
		{
			mLine->DeleteLine();
		}
		// 情報保存
		mSaveSize = mOwner->GetScale();
		mSaveRota = mOwner->GetRotation();
		mSavePos = mOwner->GetPosition();
		mSaveState = state;

		// 中心座標更新
		GetCenter();
	}
}