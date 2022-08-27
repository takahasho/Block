#include "BoxInputScript.h"
#include "../Input/DInput.h"
#include "Actor.h"
#include "../VECTOR.h"
#include "Game.h"
BoxInputComponent::BoxInputComponent(Actor* owner)
	:MoveComponent(owner)
{

}

void BoxInputComponent::ProcessInput()
{

	if (mOwner->GetGame()->GetGameState() == GameState::EPlay)
	{

		int keyData = -1;

		if (KEY(L_KEY) && KEY(J_KEY) && KEY(I_KEY) && KEY(K_KEY))
		{
			keyData = -1;
		}
		else
		{
			if (KEY(I_KEY))
			{
				keyData = 0;
			}
			else if (KEY(K_KEY))
			{
				keyData = 4;
			}

			if (KEY(L_KEY))
			{
				keyData = 2;
			}
			else if (KEY(J_KEY))
			{
				keyData = 6;
			}

			if (KEY(L_KEY) && KEY(J_KEY))
			{
				keyData = -1;
			}
			if (KEY(I_KEY) && KEY(K_KEY))
			{
				keyData = -1;
			}

			if (KEY(I_KEY) && KEY(L_KEY))
			{
				keyData = 1;
			}
			if (KEY(K_KEY) && KEY(L_KEY))
			{
				keyData = 3;
			}
			if (KEY(K_KEY) && KEY(J_KEY))
			{
				keyData = 5;
			}
			if (KEY(I_KEY) && KEY(J_KEY))
			{
				keyData = 7;
			}
			if (KEY(L_KEY) && KEY(J_KEY) && KEY(I_KEY))
			{
				keyData = 0;
			}
			if (KEY(L_KEY) && KEY(J_KEY) && KEY(K_KEY))
			{
				keyData = 4;
			}
			if (KEY(I_KEY) && KEY(K_KEY) && KEY(L_KEY))
			{
				keyData = 2;
			}
			if (KEY(I_KEY) && KEY(K_KEY) && KEY(J_KEY))
			{
				keyData = 6;
			}
		}
		SetDirection(VECTOR(0, 0, 0));

		if (keyData != -1)
		{
			XMVECTOR direction = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
			XMMATRIX rotationMat = XMMatrixRotationY(XMConvertToRadians(mOwner->GetRotation().y) + XMConvertToRadians(45.0f) * keyData);
			direction = XMVector3Transform(direction, rotationMat);
			SetDirection(VECTOR(direction.m128_f32[0], direction.m128_f32[1], direction.m128_f32[2]));
		}
	}
}