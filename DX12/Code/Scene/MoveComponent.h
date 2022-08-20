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
    float mSpeed;                   // ���x
    VECTOR mDirection;              // �ړ�����
    float mJumpPower;               // �W�����v��
    float mJumpVector;              // �W�����v�́i�t���[�����j
};