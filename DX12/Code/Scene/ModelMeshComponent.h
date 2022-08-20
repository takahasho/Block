#pragma once
#include "ColliderComponent.h"
class ModelMeshComponent :
    public ColliderComponent
{
public:
    ModelMeshComponent(class Actor* owner, int order = 100);
    // 頂点情報法線情報の更新
    void SetVerticesNormals() override;
};