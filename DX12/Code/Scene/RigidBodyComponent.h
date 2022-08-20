#pragma once
#include "MoveComponent.h"

class RigidBodyComponent :
	public MoveComponent
{
public:
	RigidBodyComponent(class Actor* owner);
private:
	void Update() override;
	class ColliderComponent* UseCollider(class Actor* actor);       // actorが使っているコライダー
	void PhysicalCalculus(ColliderComponent* collider);             // 壁ずりベクター計算
private:
	float mGravity;				// 重力
	float mMass;				// 質量
	VECTOR mPrevVector;         // 前フレームの進行ベクトル
	float mFreeFallTime;		// 自由落下時間
};