#pragma once
#include "ColliderComponent.h"
class CapsuleComponent :
    public ColliderComponent
{
public:
    CapsuleComponent(class Actor* owner, int order = 100);
    // ���_���@�����̍X�V
    void SetVerticesNormals() override;
};