#pragma once
#include "ColliderComponent.h"
class ModelMeshComponent :
    public ColliderComponent
{
public:
    ModelMeshComponent(class Actor* owner, int order = 100);
    std::vector<VECTOR> UpdateVertex();        // 頂点情報更新
};