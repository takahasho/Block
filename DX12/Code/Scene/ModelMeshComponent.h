#pragma once
#include "ColliderComponent.h"
class ModelMeshComponent :
    public ColliderComponent
{
public:
    ModelMeshComponent(class Actor* owner, int order = 100);
    // ���_���@�����̍X�V
    void SetVerticesNormals() override;
};