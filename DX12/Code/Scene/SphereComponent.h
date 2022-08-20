#pragma once
#include "Component.h"
#include "../VECTOR.h"
class SphereComponent :
    public Component
{
public:
    SphereComponent(class Actor* owner, int order = 100);
    const VECTOR& GetCenter()const;
    void SetRadius(float r) { mRadius = r; }
    float GetRadius() const { return mRadius; }
private:
    float mRadius;
};