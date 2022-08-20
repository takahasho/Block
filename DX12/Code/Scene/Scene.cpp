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
	// カメラ
	mCamera = new CameraComponent(this);

	mPlayerInput = new PlayerInputComponent(this);

	//Scene状態切り替えコンポーネント
	mState = new StateComponent(this);
	mState->RegisterState(new Title(mState));
	mState->RegisterState(new Play(mState));
	mState->RegisterState(new Creative(mState));
	mState->ChangeState("Title");

	// CreateScene用のCanvasFrameを作る
	mLine = new PointLineComponent(this);
	// Line情報登録
	// 矩形なので４
	std::vector<LineData> data(4);
	// 左線
	data[0].startPos = VECTOR(0, (float)g_window_height, 0);
	data[0].endPos = VECTOR(0, 0, 0);
	// 色は青
	data[0].color = blue;

	// 右線
	data[1].startPos = VECTOR((float)g_window_width, (float)g_window_height, 0);
	data[1].endPos = VECTOR((float)g_window_width, 0, 0);
	// 色は青
	data[1].color = blue;

	// 上線
	data[2].startPos = VECTOR(0, (float)g_window_height, 0);
	data[2].endPos = VECTOR((float)g_window_width, (float)g_window_height, 0);
	// 色は青
	data[2].color = blue;

	// 下線
	data[3].startPos = VECTOR(0, 0, 0);
	data[3].endPos = VECTOR((float)g_window_width, 0, 0);
	// 色は青
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
