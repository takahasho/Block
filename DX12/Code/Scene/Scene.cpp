#include "Game.h"
#include "PlayerInputComponent.h"
#include "CameraComponent.h"
#include "Scene.h"
#include "SceneState.h"
#include "StateComponent.h"
#include "PointLineComponent.h"

Scene::Scene(Game* game)
	:Actor(game)
{
	// �J����
	mCamera = new CameraComponent(this);

	mPlayerInput = new PlayerInputComponent(this);

	//Scene��Ԑ؂�ւ��R���|�[�l���g
	mState = new StateComponent(this);
	mState->RegisterState(new Title(mState));
	mState->RegisterState(new Play(mState));
	mState->RegisterState(new Creative(mState));
	mState->ChangeState("Title");

	// CreateScene�p��CanvasFrame�����
	mLine = new PointLineComponent(this);
	// Line���o�^
	// ��`�Ȃ̂łS
	std::vector<LineData> data(4);
	// ����
	data[0].startPos = VECTOR(0, (float)g_window_height, 0);
	data[0].endPos = VECTOR(0, 0, 0);
	// �F�͐�
	data[0].color = blue;

	// �E��
	data[1].startPos = VECTOR((float)g_window_width, (float)g_window_height, 0);
	data[1].endPos = VECTOR((float)g_window_width, 0, 0);
	// �F�͐�
	data[1].color = blue;

	// ���
	data[2].startPos = VECTOR(0, (float)g_window_height, 0);
	data[2].endPos = VECTOR((float)g_window_width, (float)g_window_height, 0);
	// �F�͐�
	data[2].color = blue;

	// ����
	data[3].startPos = VECTOR(0, 0, 0);
	data[3].endPos = VECTOR((float)g_window_width, 0, 0);
	// �F�͐�
	data[3].color = blue;

	mLine->resizeLineData((int)data.size());
	for (int i = 0, size = (int)data.size(); i < size; ++i)
	{
		mLine->SetLineData(i, data[i]);
	}
}

Scene::~Scene()
{
}
