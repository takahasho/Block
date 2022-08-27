#include "PlayerInputComponent.h"
#include "../Input/DInput.h"
#include "Actor.h"
#include "../VECTOR.h"
#include "Game.h"
BOOL PlayerInputComponent::mShowCursor = true;			// インスタンスの実態

PlayerInputComponent::PlayerInputComponent(Actor* owner)
	:MoveComponent(owner)
{

}

void PlayerInputComponent::ProcessInput()
{
	RECT rect;

	GetWindowRect(g_hwnd, &rect);
	// カーソルの表示
	if ((KEY_ENTER(M_KEY)&& !mPushMKey)||mCursor)
	{
		mShowCursor ^= true;
		SendMessage(g_hwnd
			, UM_CURSORSTATUS, mShowCursor, 0L);
		mPushMKey = true;
		// 画面の中央に戻す(960,540)
		SetCursorPos(rect.left + (rect.right - rect.left) / 2, rect.top + (rect.bottom - rect.top) / 2);
		mCursor = false;
	}
	if (!(KEY_ENTER(M_KEY)))
	{
		mPushMKey = false;
	}
	if (mOwner->GetGame()->GetGameState() == GameState::EPlay)
	{
		if (mOwner->GetGame()->GetGameState() == GameState::EPlay)
		{

			int keyData = -1;

			// 移動
			if (KEY(D_KEY) && KEY(A_KEY) && KEY(W_KEY) && KEY(S_KEY))
			{
				keyData = -1;
			}
			else
			{
				if (KEY(W_KEY))
				{
					keyData = 0;
				}
				else if (KEY(S_KEY))
				{
					keyData = 4;
				}

				if (KEY(D_KEY))
				{
					keyData = 2;
				}
				else if (KEY(A_KEY))
				{
					keyData = 6;
				}

				if (KEY(D_KEY) && KEY(A_KEY))
				{
					keyData = -1;
				}
				if (KEY(W_KEY) && KEY(S_KEY))
				{
					keyData = -1;
				}

				if (KEY(W_KEY) && KEY(D_KEY))
				{
					keyData = 1;
				}
				if (KEY(S_KEY) && KEY(D_KEY))
				{
					keyData = 3;
				}
				if (KEY(S_KEY) && KEY(A_KEY))
				{
					keyData = 5;
				}
				if (KEY(W_KEY) && KEY(A_KEY))
				{
					keyData = 7;
				}
				if (KEY(D_KEY) && KEY(A_KEY) && KEY(W_KEY))
				{
					keyData = 0;
				}
				if (KEY(D_KEY) && KEY(A_KEY) && KEY(S_KEY))
				{
					keyData = 4;
				}
				if (KEY(W_KEY) && KEY(S_KEY) && KEY(D_KEY))
				{
					keyData = 2;
				}
				if (KEY(W_KEY) && KEY(S_KEY) && KEY(A_KEY))
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
}

void PlayerInputComponent::SetShowCursor()
{
	mCursor = true;
}