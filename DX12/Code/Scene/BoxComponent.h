#pragma once
#include "ColliderComponent.h"
class BoxComponent :
    public ColliderComponent
{
public:
    BoxComponent(class Actor* owner, int order = 100);
    // 頂点情報法線情報の更新
    void SetVerticesNormals() override;
};