#pragma once
#include "ColliderComponent.h"
class BoxComponent :
    public ColliderComponent
{
public:
    BoxComponent(class Actor* owner, int order = 100);
    // ���_���@�����̍X�V
    void SetVerticesNormals() override;
};