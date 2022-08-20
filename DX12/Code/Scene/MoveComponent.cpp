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
			// 位置更新
			VECTOR pos = mOwner->GetPosition();
			pos += mOwner->GetProgressVector();
			mOwner->SetPosition(pos);
			if (mOwner->GetDashFlg())
			{
				mOwner->SetProgressVector(mDirection * mSpeed * 2 * g_elapsedTime);
			}
			else
			{
				mOwner->SetProgressVector(mDirection * mSpeed * g_elapsedTime);
			}

			// 回転更新
			VECTOR angle = mOwner->GetRotation();
			angle += mOwner->GetAngularVelocity();
			mOwner->SetRotation(angle);
			mOwner->SetAngularVelocity(VECTOR(0, 0, 0));
		}
	}
}