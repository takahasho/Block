#pragma once
#include "ColliderComponent.h"
class ModelMeshComponent :
    public ColliderComponent
{
public:
    ModelMeshComponent(class Actor* owner, int order = 100);
    std::vector<VECTOR> UpdateVertex();        // ’¸“_î•ñXV
};