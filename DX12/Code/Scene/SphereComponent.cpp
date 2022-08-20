#include "SphereComponent.h"
#include "Actor.h"

SphereComponent::SphereComponent(Actor* owner, int order)
	:Component(owner, order)
	, mRadius(0.5f)
{
}

const VECTOR& SphereComponent::GetCenter()const
{
	return mOwner->GetPosition();
}