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
	// タイトル、クリエイティブモードで初期化
	else if (mOwner->GetGame()->GetGameState() == GameState::ECreative || mOwner->GetGame()->GetGameState() == GameState::ETitle)
	{
		mFreeFallTime = 0;
		mJumpVector = 0;
		mOwner->SetJumpFlg(false);
		mOwner->SetProgressVector(VECTOR(0, 0, 0));
	}
}
// actorが使っているコライダー
ColliderComponent* RigidBodyComponent::UseCollider(Actor* actor)
{
	// どのコンポーネントが取り付けられているか
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
// 壁ずりベクター計算
void RigidBodyComponent::PhysicalCalculus(ColliderComponent* collider)
{
	// 落下時間
	mFreeFallTime += g_elapsedTime;

	// プレイヤーの現在地
	VECTOR oldPos = mOwner->GetPosition();

	// 進行ベクトル
	VECTOR ProgressVector = mOwner->GetProgressVector();

	// ジャンプボタンが押されたら
	if (mOwner->GetJumpFlg())
	{
		float saveVec = mJumpVector;
		mJumpVector = mFreeFallTime * mJumpPower;

		ProgressVector += VECTOR(0, mJumpVector - saveVec, 0);
	}

	ProgressVector += VECTOR(0, -(mGravity * mMass * mFreeFallTime * mFreeFallTime), 0);
	// プレイヤーの次のフレームの位置
	VECTOR nowPos = mOwner->GetPosition() + ProgressVector;

	// --------- 壁ずりベクトル -----------
	
	if (ProgressVector != VECTOR(0, 0, 0))
	{
		// このフレームで進む進行ベクトル分動かす
		mOwner->SetPosition(nowPos);
		VECTOR rotation;
		ColliderComponent* OpponentjCollider;
		// ---------- オブジェクトとの当たり判定 ----------
		for (Actor* actor : mOwner->GetGame()->GetActors())
		{
			if (actor != mOwner)
			{
				OpponentjCollider = UseCollider(actor);
				if (OpponentjCollider)
				{
					// 二つのオブジェクトの距離
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
		// 進めたベクトル分戻す
		mOwner->SetPosition(oldPos);

		// 進行ベクトルを決定する
		mOwner->SetProgressVector(ProgressVector);
	}

	// 落下していなかったら0
	if (ProgressVector.y == 0)
	{
		mFreeFallTime = 0;
		mJumpVector = 0;
		mOwner->SetJumpFlg(false);
	}

	//	// ほぼ真逆の法線ベクトルがあれば削除
	//	int i;
	//	int j;
	//	for (i = 0; i < normals.size(); i++)
	//	{
	//		for (j = 0; j < normals.size(); j++)
	//		{
	//			if (i != j)
	//			{
	//				VECTOR difference = normals[i] + normals[j];
	//				// 微妙な誤差でも削除
	//				if (difference.mag() < 0.00001f)
	//				{
	//					// コピーをとって全ての要素を消去する
	//					std::vector<VECTOR> Normals = normals;
	//					normals.clear();
	//					for (int k = 0; k < Normals.size(); k++)
	//					{
	//						// 真逆の法線ベクトル二つ以外を積む
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

	//	// ---------- 法線をもとに壁ずりベクトルを作る ----------
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

	//	// 進めたベクトル分戻す
	//	mOwner->SetPosition(oldPos);


	// }
}