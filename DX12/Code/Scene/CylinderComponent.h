#pragma once
#include "ColliderComponent.h"
class CylinderComponent:
    public ColliderComponent
{
public:
    CylinderComponent(class Actor* owner, int order = 100);
};