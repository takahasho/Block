#pragma once
#include "Component.h"
#include "../VECTOR.h"
class MoveComponent :
    public Component
{
public:
    MoveComponent(class Actor* owner, int order = 100);
    void Update() override;
    float GetSpeed() { return mSpeed; }
    void SetSpeed(float speed) { mSpeed = speed; }
    VECTOR GetDirection() { return mDirection; }
    void SetDirection(VECTOR direction) { mDirection = direction; }
protected:
    float mSpeed;                   // 速度
    VECTOR mDirection;              // 移動方向
    float mJumpPower;               // ジャンプ力
    float mJumpVector;              // ジャンプ力（フレーム毎）
};