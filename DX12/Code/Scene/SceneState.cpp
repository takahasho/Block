#include "SceneState.h"

#include "Game.h"
#include "Scene.h"
#include "StateComponent.h"
#include "CameraComponent.h"	
#include "PlayerInputComponent.h"
#include "PointLineComponent.h"
#include "ModelComponent.h"
#include "../MATRIX.h"
//-------------------------------------------------------------------
void Title::OnEnter()
{
	// ----- Sceneクラスの取得 -----
	if (!SCENE)
	{
		SCENE = dynamic_cast<Scene*>(mOwnerCompo->GetActor()); // プレイヤーActor
	}
	if (mFirstFlag)
	{
		SCENE->GetGame()->GetObjectCreate()->LoadFile();
		mFirstFlag = false;
	}
	// GameState変更
	SCENE->GetGame()->SetGameState(GameState::ETitle);
	// カメラ初期化
	eyeAngle = VECTOR(30, 180, 0);
	addAngle = {};
}
void Title::Input()
{
	if (SCENE->GetGame()->GetGameState() == GameState::ETitle)
	{
		// 拡大されたときにも対応できるようにクライアント領域を受け取る
		GetClientRect(g_hwnd, &mClientRect);

		// ウィンドウ領域のマウス座標
		GetCursorPos(&mMousePoint);

		// ウィンドウ領域からクライアント座標に
		ScreenToClient(g_hwnd, &mMousePoint);

		// 横幅変数更新
		mClientWidth = (float)mClientRect.right;
		// 縦幅変数更新
		mClientHeight = (float)mClientRect.bottom;
		// マウス座標変数更新
		mMousePosX = (float)mMousePoint.x;
		mMousePosY = (float)mMousePoint.y;

		//　ウィンドウ拡大時に引き伸ばされた割合だけマウス座標にかける
		float w = g_window_width / mClientWidth;
		float h = g_window_height / mClientHeight;
		mMousePosX *= w;
		mMousePosY *= h;

		if (mMousePosX >= (mPlayImage.position.x + (mPlayImage.left * mPlayImage.scale.x)) && mMousePosX <= (mPlayImage.position.x + (mPlayImage.right * mPlayImage.scale.x))
			&& mMousePosY >= (mPlayImage.position.y + (mPlayImage.top * mPlayImage.scale.y)) && mMousePosY <= (mPlayImage.position.y + (mPlayImage.bottom * mPlayImage.scale.y)))
		{
			if (MOUSE_ENTER(0))
			{
				mOwnerCompo->ChangeState("Play");
				//SCENE->GetInput()->SetShowCursor();
			}
		}
		else if (mMousePosX >= (mCreateImage.position.x + (mCreateImage.left * mCreateImage.scale.x)) && mMousePosX <= (mCreateImage.position.x + (mCreateImage.right * mCreateImage.scale.x))
			&& mMousePosY >= (mCreateImage.position.y + (mCreateImage.top * mCreateImage.scale.y)) && mMousePosY <= (mCreateImage.position.y + (mCreateImage.bottom * mCreateImage.scale.y)))
		{
			if (MOUSE_ENTER(0))
			{
				mOwnerCompo->ChangeState("Creative");
			}
		}
		else if (mMousePosX >= (mExitImage.position.x + (mExitImage.left * mExitImage.scale.x)) && mMousePosX <= (mExitImage.position.x + (mExitImage.right * mExitImage.scale.x))
			&& mMousePosY >= (mExitImage.position.y + (mExitImage.top * mExitImage.scale.y)) && mMousePosY <= (mExitImage.position.y + (mExitImage.bottom * mExitImage.scale.y)))
		{
			if (MOUSE_ENTER(0))
			{
				SCENE->GetGame()->SetGameFlag(true);
			}
		}
		// カメラを回し続ける
		addAngle = VECTOR(0, 0.3f, 0);
		eyeAngle += addAngle;
		// 客観カメラ
		SCENE->GetCamera()->UseObjectiveCamera(VECTOR(0, 0, 0), eyeAngle, SCENE->GetCamera()->GetZoom());
	}
}
void Title::Update()
{
	SetUI(mBlockImage);
	SetUI(mPlayImage);
	SetUI(mCreateImage);
	SetUI(mExitImage);
}
void Title::OnExit()
{
	RiseUI(mBlockImage);
	RiseUI(mPlayImage);
	RiseUI(mCreateImage);
	RiseUI(mExitImage);
}
//-------------------------------------------------------------------

void Play::OnEnter()
{

	// ----- Sceneクラスの取得 -----
	if (!SCENE)
	{
		SCENE = dynamic_cast<Scene*>(mOwnerCompo->GetActor()); // プレイヤーActor
	}
	// GameState変更
	SCENE->GetGame()->SetGameState(GameState::EPlay);
	if (SCENE->GetGame()->GetActor(_T("Player")))
	{
		SCENE->GetGame()->GetActor(_T("Player"))->GetModelComponent()->DeleteObjModel();
	}
}
void Play::Input()
{

	if (KEY_ENTER(TAB_KEY))
	{
		//SCENE->GetInput()->SetShowCursor();
		if (SCENE->GetGame()->GetGameState() == GameState::EPouse)
		{
			SCENE->GetGame()->SetGameState(GameState::EPlay);
		}
		else if (SCENE->GetGame()->GetGameState() == GameState::EPlay)
		{
			SCENE->GetGame()->SetGameState(GameState::EPouse);
		}
	}

	if (SCENE->GetGame()->GetGameState() == GameState::EPouse)
	{
		// 拡大されたときにも対応できるようにクライアント領域を受け取る
		GetClientRect(g_hwnd, &mClientRect);

		// ウィンドウ領域のマウス座標
		GetCursorPos(&mMousePoint);

		// ウィンドウ領域からクライアント座標に
		ScreenToClient(g_hwnd, &mMousePoint);

		// 横幅変数更新
		mClientWidth = (float)mClientRect.right;
		// 縦幅変数更新
		mClientHeight = (float)mClientRect.bottom;
		// マウス座標変数更新
		mMousePosX = (float)mMousePoint.x;
		mMousePosY = (float)mMousePoint.y;

		//　ウィンドウ拡大時に引き伸ばされた割合だけマウス座標にかける
		float w = g_window_width / mClientWidth;
		float h = g_window_height / mClientHeight;
		mMousePosX *= w;
		mMousePosY *= h;
		if (mMousePosX >= (mTitleButton.position.x + (mTitleButton.left * mTitleButton.scale.x)) && mMousePosX <= (mTitleButton.position.x + (mTitleButton.right * mTitleButton.scale.x))
			&& mMousePosY >= (mTitleButton.position.y + (mTitleButton.top * mTitleButton.scale.y)) && mMousePosY <= (mTitleButton.position.y + (mTitleButton.bottom * mTitleButton.scale.y)))
		{
			if (MOUSE_ENTER(0))
			{
				mOwnerCompo->ChangeState("Title");
			}
		}
		else if (mMousePosX >= (mCreateButton.position.x + (mCreateButton.left * mCreateButton.scale.x)) && mMousePosX <= (mCreateButton.position.x + (mCreateButton.right * mCreateButton.scale.x))
			&& mMousePosY >= (mCreateButton.position.y + (mCreateButton.top * mCreateButton.scale.y)) && mMousePosY <= (mCreateButton.position.y + (mCreateButton.bottom * mCreateButton.scale.y)))
		{
			if (MOUSE_ENTER(0))
			{
				mOwnerCompo->ChangeState("Creative");
			}
		}
	}
	// 主観カメラ
	if (SCENE->GetGame()->GetActor(_T("Player")))
	{
		SCENE->GetCamera()->UseSubjectiveCamera(SCENE->GetGame()->GetActor(_T("Player")));
	}
	else
	{
		// 客観カメラ(仮)：2DGameをつくるので適当
		SCENE->GetCamera()->UseObjectiveCamera(VECTOR(0, 0, 0), VECTOR(1, 0, 0), 20);
	}
}
void Play::Update()
{
	if (SCENE->GetGame()->GetGameState() == GameState::EPouse)
	{
		SetUI(mOption);
		SetUI(mTitleButton);
		SetUI(mCreateButton);
	}
	else if (SCENE->GetGame()->GetGameState() == GameState::EPlay)
	{
		RiseUI(mOption);
		RiseUI(mTitleButton);
		RiseUI(mCreateButton);
	}
}

void Play::OnExit()
{
	RiseUI(mOption);
	RiseUI(mTitleButton);
	RiseUI(mCreateButton);
}

//-------------------------------------------------------------------
void Creative::OnEnter()
{

	// ----- Sceneクラスの取得 -----
	if (!SCENE)
	{
		SCENE = dynamic_cast<Scene*>(mOwnerCompo->GetActor()); // プレイヤーActor
	}
	// ObjectCreateクラスの初期化
	SCENE->GetGame()->GetObjectCreate()->Init();
	if (SCENE->GetGame()->GetActor(_T("Player")))
	{
		SCENE->GetGame()->GetActor(_T("Player"))->GetModelComponent()->SetObjModel();
	}
	// カメラ初期化
	eyeAngle3D = VECTOR(30, 0, 0);
	zoom3D = SCENE->GetCamera()->GetZoom();
	SCENE->GetCamera()->SetZoom3D(zoom3D);
	saveEyeAngle = eyeAngle3D;
	targetPos3D = VECTOR(0, 0, 0);
	saveTargetPos = targetPos3D;

	eyeAngle2D = VECTOR(0, 0, 0);
	// 横幅、縦幅大きいほうの長さから初期Zoomを求める
	float value = (float)g_window_width;
	if (value < (float)g_window_height)
	{
		value = (float)g_window_height;
	}
	zoom2D = value * 1.4f;
	// zoom最大値より大きければ最大値に固定する
	if (zoom2D >= zoom2DMax)
	{
		zoom2D = zoom2DMax;
	}

	SCENE->GetCamera()->SetZoom2D(zoom2D);
	saveEyeAngle = eyeAngle2D;
	// 1980x1020の中心座標
	targetPos2D = VECTOR((float)g_window_width / 2, (float)g_window_height / 2, 0);
	saveTargetPos = targetPos2D;

	SCENE->GetGame()->SetGameState(GameState::ECreative);
}
void Creative::Input()
{
	SCENE->GetGame()->GetObjectCreate()->ProcessInput();
	// ----- Scene遷移 -----
	if (SCENE->GetGame()->GetObjectCreate()->GetPlayTrans())
	{
		mOwnerCompo->ChangeState("Play");
	}
	if (SCENE->GetGame()->GetObjectCreate()->GetTitleTrans())
	{
		mOwnerCompo->ChangeState("Title");
	}

	// ----- CanvesFrame -----
	// 2Dモードなら表示、そうでなければ非表示
	if (SCENE->GetGame()->GetObjectCreate()->Get2DMode())
	{
		SCENE->GetLine()->SetLine();
	}
	else
	{
		SCENE->GetLine()->DeleteLine();
	}
	// ----- カメラ制御 -----
	if (!SCENE->GetGame()->GetObjectCreate()->GetPutOnUI())
	{

		LPDIMOUSESTATE2 pMouseState = DInput::GetInstance()->GetMouseState();

		if (SCENE->GetGame()->GetObjectCreate()->Get2DMode())
		{
			// ズーム
			if (-1000 < pMouseState->lZ && pMouseState->lZ < 1000)
			{
				zoom2D = SCENE->GetCamera()->GetZoom2D();
				if (pMouseState->lZ > 0.0f)	// ズームイン
				{
					zoom2D -= 50.0;
					if (zoom2D <= zoom2DMin)
						zoom2D = zoom2DMin;
				}
				if (pMouseState->lZ < 0.0f)	// ズームアウト
				{
					zoom2D += 50.0;
					if (zoom2D >= zoom2DMax)
						zoom2D = zoom2DMax;
				}
			}
			// アングル
			// 押したときの位置から差異を求める		

			if (MOUSE_ENTER(1))
			{
				GetCursorPos(&mRBPushPoint);
				saveEyeAngle = SCENE->GetCamera()->GetEyeAngle();
			}
			if (MOUSE(1))
			{
				GetCursorPos(&mRBStayPoint);
				FLOAT subX = (FLOAT)(mRBPushPoint.x - mRBStayPoint.x);
				FLOAT subY = (FLOAT)(mRBPushPoint.y - mRBStayPoint.y);

				eyeAngle2D.y = saveEyeAngle.y - subX * mRotateSpeed;

				eyeAngle2D.x = saveEyeAngle.x - subY * mRotateSpeed;
			}

			if (eyeAngle2D.x > 90)
				eyeAngle2D.x = 90;
			if (eyeAngle2D.x < -90)
				eyeAngle2D.x = -90;

			// 移動
			if (MOUSE_ENTER(2))
			{
				GetCursorPos(&mSWPushPoint);
				saveTargetPos = SCENE->GetCamera()->GetTargetPosition();
			}
			if (MOUSE(2))
			{
				GetCursorPos(&mSWStayPoint);
				FLOAT subX = (FLOAT)(mSWPushPoint.x - mSWStayPoint.x);
				FLOAT subY = (FLOAT)(mSWPushPoint.y - mSWStayPoint.y);

				VECTOR tes = VECTOR(subX, subY, 0);
				MATRIX basePosition;
				basePosition.translate(tes.x, tes.y, tes.z);
				MATRIX rotationX;
				MATRIX rotationY;
				rotationX.rotateX(XMConvertToRadians(eyeAngle2D.x));
				rotationY.rotateY(XMConvertToRadians(-eyeAngle2D.y));
				basePosition = rotationY * rotationX * basePosition;
				VECTOR pos = VECTOR(basePosition._14, basePosition._24, basePosition._34);

				targetPos2D.x = saveTargetPos.x + pos.x * zoom2D * 0.00075f;

				targetPos2D.y = saveTargetPos.y - pos.y * zoom2D * 0.00075f;

				targetPos2D.z = saveTargetPos.z - pos.z * zoom2D * 0.00075f;
			}
			// 客観カメラ
			SCENE->GetCamera()->UseObjectiveCamera(targetPos2D, eyeAngle2D, zoom2D);
			// zoom情報保存
			SCENE->GetCamera()->SetZoom2D(zoom2D);
		}
		else
		{
			// ズーム
			if (-1000 < pMouseState->lZ && pMouseState->lZ < 1000)
			{
				zoom3D = SCENE->GetCamera()->GetZoom3D();
				if (pMouseState->lZ > 0.0f)	// ズームイン
				{
					zoom3D -= 1.0;
					if (zoom3D <= zoom3DMin)
						zoom3D = zoom3DMin;
				}
				if (pMouseState->lZ < 0.0f)	// ズームアウト
				{
					zoom3D += 1.0;
					if (zoom3D >= zoom3DMax)
						zoom3D = zoom3DMax;
				}
			}
			// アングル
			// 押したときの位置から差異を求める		

			if (MOUSE_ENTER(1))
			{
				GetCursorPos(&mRBPushPoint);
				saveEyeAngle = SCENE->GetCamera()->GetEyeAngle();
			}
			if (MOUSE(1))
			{
				GetCursorPos(&mRBStayPoint);
				FLOAT subX = (FLOAT)(mRBPushPoint.x - mRBStayPoint.x);
				FLOAT subY = (FLOAT)(mRBPushPoint.y - mRBStayPoint.y);

				eyeAngle3D.y = saveEyeAngle.y - subX * mRotateSpeed;

				eyeAngle3D.x = saveEyeAngle.x - subY * mRotateSpeed;
			}

			if (eyeAngle3D.x > 90)
				eyeAngle3D.x = 90;
			if (eyeAngle3D.x < -90)
				eyeAngle3D.x = -90;

			// 移動
			if (MOUSE_ENTER(2))
			{
				GetCursorPos(&mSWPushPoint);
				saveTargetPos = SCENE->GetCamera()->GetTargetPosition();
			}
			if (MOUSE(2))
			{
				GetCursorPos(&mSWStayPoint);
				FLOAT subX = (FLOAT)(mSWPushPoint.x - mSWStayPoint.x);
				FLOAT subY = (FLOAT)(mSWPushPoint.y - mSWStayPoint.y);

				VECTOR tes = VECTOR(subX, subY, 0);
				MATRIX basePosition;
				basePosition.translate(tes.x, tes.y, tes.z);
				MATRIX rotationX;
				MATRIX rotationY;
				rotationX.rotateX(XMConvertToRadians(eyeAngle3D.x));
				rotationY.rotateY(XMConvertToRadians(-eyeAngle3D.y));
				basePosition = rotationY * rotationX * basePosition;
				VECTOR pos = VECTOR(basePosition._14, basePosition._24, basePosition._34);

				targetPos3D.x = saveTargetPos.x + pos.x * zoom3D * 0.00075f;

				targetPos3D.y = saveTargetPos.y - pos.y * zoom3D * 0.00075f;

				targetPos3D.z = saveTargetPos.z - pos.z * zoom3D * 0.00075f;
			}
			// 客観カメラ
			SCENE->GetCamera()->UseObjectiveCamera(targetPos3D, eyeAngle3D, zoom3D);
			// zoom情報保存
			SCENE->GetCamera()->SetZoom3D(zoom3D);
		}
	}
}

void Creative::Update()
{
	SCENE->GetGame()->GetObjectCreate()->UpdateGame();
}
void Creative::OnExit()
{
	SCENE->GetGame()->GetObjectCreate()->DeleteUI();
	SCENE->GetCamera()->SetZoom(SCENE->GetCamera()->GetZoom3D());
}