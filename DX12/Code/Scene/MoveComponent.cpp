#include "MoveComponent.h"
#include "Actor.h"
#include "Game.h"
#include "../Global.h"
MoveComponent::MoveComponent(Actor* owner, int order)
	:Component(owner, order), mSpeed(3), mJumpPower(3), mJumpVector(0)
{
}

void MoveComponent::Update()
{
	if (mOwner->GetGame()->GetGameState() == GameState::EPlay)
	{
		if (mSpeed > 0)
		{

		}
	}
}