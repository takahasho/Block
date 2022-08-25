#pragma once
#include "State.h"
#include "../VECTOR.h"
#include "../Input/DInput.h"
#include <vector>
#include "ObjectCreate.h"
//-----------------------------------------------------------------------------
class Title : public State
{
public:
	Title(class StateComponent* OwnerCompo) :State(OwnerCompo), eyeAngle(VECTOR(30, 0, 0)), mClientHeight(), mClientRect(), mClientWidth(),
		mMousePoint(), mMousePosX(0), mMousePosY(0)
	{
		CreativeUI Image(600, 200);
		Image.fileName = "Resouse\\Texture\\UI\\BlockImage.png";
		Image.position = VECTOR2((float)g_window_width / 2, 100);
		mBlockImage = Image;
		CreativeUI image(450, 100);
		image.fileName = "Resouse\\Texture\\UI\\PlayImage.png";
		image.position = VECTOR2((float)g_window_width / 2, (float)g_window_height - 300);
		mPlayImage = image;
		image.fileName = "Resouse\\Texture\\UI\\CreateImage.png";
		image.position = VECTOR2((float)g_window_width / 2, (float)g_window_height - 200);
		mCreateImage = image;
		image.fileName = "Resouse\\Texture\\UI\\ExitImage.png";
		image.position = VECTOR2((float)g_window_width / 2, (float)g_window_height - 100);
		mExitImage = image;
	}
	const char* GetName() const override { return "Title"; }

	void OnEnter() override;
	void Input() override;
	void Update() override;
	void OnExit() override;
private:
	// ---------- 変数宣言 ----------
	class Scene* SCENE = nullptr; // Sceneクラス

	bool mFirstFlag = true;		// 最初の一回だけ

	VECTOR addAngle;
	VECTOR eyeAngle;

	POINT mMousePoint;
	RECT mClientRect;
	// 横幅変数
	float mClientWidth;
	// 縦幅変数
	float mClientHeight;
	// マウス座標変数
	float mMousePosX;
	float mMousePosY;

	CreativeUI mBlockImage;
	CreativeUI mPlayImage;
	CreativeUI mCreateImage;
	CreativeUI mExitImage;
};

//-----------------------------------------------------------------------------
class Play : public State
{
public:
	Play(class StateComponent* OwnerCompo) :State(OwnerCompo), mClientHeight(), mClientRect(), mClientWidth(),
		mMousePoint(), mMousePosX(0), mMousePosY(0)
	{
		CreativeUI Image(200, 200);
		Image.fileName = "Resouse\\Texture\\UI\\PlayOption.png";
		Image.position = VECTOR2((float)g_window_width / 2, 400);
		Image.drawOrder = 100;
		mOption = Image;
		CreativeUI image(100, 50);
		image.fileName = "Resouse\\Texture\\UI\\TitleButton.png";
		image.position = VECTOR2((float)g_window_width / 2, 350);
		image.drawOrder = 100;
		mTitleButton = image;
		image.fileName = "Resouse\\Texture\\UI\\CreateButton.png";
		image.position = VECTOR2((float)g_window_width / 2, 450);
		mCreateButton = image;			
	}
	~Play() { }
	const char* GetName() const override { return "Play"; }
	void OnEnter() override;
	void Input() override;
	void Update() override;
	void OnExit() override;
private:
	// ---------- 変数宣言 ----------
	class Scene* SCENE = nullptr; // Sceneクラス

	POINT mMousePoint;
	RECT mClientRect;
	// 横幅変数
	float mClientWidth;
	// 縦幅変数
	float mClientHeight;
	// マウス座標変数
	float mMousePosX;
	float mMousePosY;

	CreativeUI mOption;
	CreativeUI mTitleButton;
	CreativeUI mCreateButton;

};

//-----------------------------------------------------------------------------
class Creative : public State
{
public:
	Creative(class StateComponent* OwnerCompo) :State(OwnerCompo),
		zoom2D(20),zoom3D(20),mRBPushPoint(), mRBStayPoint(), mSWPushPoint(), mSWStayPoint()	
	{}
	const char* GetName() const override { return "Creative"; }
	void OnEnter() override;
	void Input() override;
	void Update() override;
	void OnExit() override;
private:
	// ---------- 変数宣言 ----------
	class Scene* SCENE = nullptr; // Sceneクラス

	VECTOR addAngle;
	VECTOR eyeAngle3D;
	VECTOR eyeAngle2D;
	VECTOR targetPos3D;
	VECTOR targetPos2D;
	VECTOR saveEyeAngle;
	VECTOR saveTargetPos;
	const float mRotateSpeed = 0.1f;
	float zoom3D;
	const float zoom3DMin = 5;
	const float zoom3DMax = 100;
	float zoom2D;
	const float zoom2DMin = 100;
	const float zoom2DMax = 3000;

	// RB:右クリック SW:スクロールホイール
	// 押されたときの位置
	POINT mRBPushPoint;
	// 押してる間の位置
	POINT mRBStayPoint;

	// 押されたときの位置
	POINT mSWPushPoint;
	// 押してる間の位置
	POINT mSWStayPoint;

};
