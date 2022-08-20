#include "EffectComponent.h"
#include "../Effect/Effect.h"
#include "Actor.h"
#include "Game.h"

EffectComponent::EffectComponent(Actor* actor, int drawOrder)
	:ObjectComponent(actor, drawOrder)
	, mEffect(0)
	, mPosition(VECTOR(0, 0, 0))
	, mRotation(VECTOR(0, 90, 90))
	, mScale(VECTOR(1, 1, 1))
	, mSpeed(5)
{

}

void EffectComponent::SetEffect(EFFEKSEER* effect)
{
	mEffect = effect;
}

void EffectComponent::SetPlayType(PlayType type) {
	if (type == PlayType::ONESHOT)
	{
		PLAY_ONESHOT(mEffect);
	}
	if (type == PlayType::STOP)
	{
		PLAY_STOP(mEffect);
	}
	if (type == PlayType::LOOP)
	{
		PLAY_LOOP(mEffect);
	}
}