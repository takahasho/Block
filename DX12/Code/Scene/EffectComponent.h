#pragma once
#include "ObjectComponent.h"
#include "../Effect/EffectStruct.h"
class EffectComponent :
    public ObjectComponent
{
public:
    EffectComponent(class Actor*, int drawOrder = 100);  // コンストラクタ

    void SetEffect(EFFEKSEER* effect);
    void SetPlayType(PlayType type);
    void SetPosition(const VECTOR& position) { mPosition = position; }
    void SetRotation(const VECTOR& angle) { mRotation = angle; }
    void SetScale(const VECTOR& scale) { mScale = scale; }
    void SetSpeed(const float speed) { mSpeed = speed; }
private:
    EFFEKSEER* mEffect;
    VECTOR mPosition;
    VECTOR mRotation;
    VECTOR mScale;
    float mSpeed;
};

