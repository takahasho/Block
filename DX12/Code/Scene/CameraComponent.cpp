#include "CameraComponent.h"
#include "Actor.h"
#include "../Camera/Camera.h"
#include "../Input/DInput.h"
#include "PlayerInputComponent.h"
CameraComponent::CameraComponent(Actor* actor, int drawOrder)
	:Component(actor)
	, mDrawOrder(drawOrder), mSubCam(false), mSubCenter(POINT())
	, mRotateSpeed(0.1f), SubjectiveInit(false), ObjectiveInit(false), mZoom3D(20), mZoom2D(100)
{
	mCamRotate = mOwner->GetRotation();
	RECT rect;

	GetWindowRect(g_hwnd, &rect);
	// 画面の中央に指定
	// カーソルを仮に(100 100)
	SetCursorPos(100, 100);

	// カーソルと(100, 100)の差異を求める
	POINT point = {};
	GetCursorPos(&point);

	mSubCenter.x = 100 - point.x;
	mSubCenter.y = 100 - point.y;

	// 画面の中央に指定(1920/2, 1080/2)
	SetCursorPos(rect.left + (rect.right - rect.left) / 2, rect.top + (rect.bottom - rect.top) / 2);
}

void CameraComponent::ProcessInput()
{

}

// 主観カメラを使う
void CameraComponent::UseSubjectiveCamera(Actor* actor)
{
	// ----- カメラ制御 -----
	if (!PlayerInputComponent::GetShowCursor())
	{
		RECT rect;

		GetWindowRect(g_hwnd, &rect);

		// 画面の中央からの差異を求める
		POINT point = {};
		GetCursorPos(&point);
		POINT center = { (rect.left + (rect.right - rect.left) / 2) - mSubCenter.x, (rect.top + (rect.bottom - rect.top) / 2) - mSubCenter.y };
		FLOAT subX = (FLOAT)(center.x - point.x);
		FLOAT subY = (FLOAT)(center.y - point.y);
		mCamRotate.y -= subX * mRotateSpeed;

		mCamRotate.x -= subY * mRotateSpeed;
		if ( mCamRotate.x < -90)
			 mCamRotate.x = -90;
		if ( mCamRotate.x > 45)
			 mCamRotate.x = 45.0f;
		// 画面の中央に戻す(960 540)
		SetCursorPos(rect.left + (rect.right - rect.left) / 2, rect.top + (rect.bottom - rect.top) / 2);

		actor->SetRotation(VECTOR(0, mCamRotate.y, 0));
	}
	// アングル
	Camera::GetInstance()->SetEyeAngle(XMFLOAT3(XMConvertToRadians(mCamRotate.x), XMConvertToRadians(mCamRotate.y), XMConvertToRadians(mCamRotate.z)));

	// 1は目線を合わせるため
	Camera::GetInstance()->SetEyePosition(XMFLOAT3(actor->GetPosition().x, actor->GetPosition().y + 1, actor->GetPosition().z));

	// カメラ配置
	Camera::GetInstance()->SetFirstPersonCamera();
}

// 客観カメラを使う
void CameraComponent::UseObjectiveCamera(const VECTOR targetPositin, const VECTOR eyeAngle, const float zoom)
{
	// ----- カメラ制御 -----
	if (!PlayerInputComponent::GetShowCursor())
	{
		RECT rect;

		GetWindowRect(g_hwnd, &rect);;

		// 画面の中央に戻す(960 540)
		SetCursorPos(rect.left + (rect.right - rect.left) / 2, rect.top + (rect.bottom - rect.top) / 2);
	}
	// アングル
	const float addAngle = 0.1f;
	XMFLOAT3 angle;
	angle.x = (XMConvertToRadians(eyeAngle.x));
	angle.y = (XMConvertToRadians(eyeAngle.y));
	angle.z = (XMConvertToRadians(eyeAngle.z));

	Camera::GetInstance()->SetEyeAngle(angle);
	Camera::GetInstance()->SetZoom(zoom);
	Camera::GetInstance()->SetTargetPosition(XMFLOAT3(targetPositin.x, targetPositin.y, targetPositin.z));
	// カメラ配置
	Camera::GetInstance()->SetThirdPersonCamera();
}

// 2Dカメラを使う
void CameraComponent::Use2DCamera()
{
	if (!PlayerInputComponent::GetShowCursor())
	{
		RECT rect;

		GetWindowRect(g_hwnd, &rect);
		// 画面の中央に戻す(960 540)
		SetCursorPos(rect.left + (rect.right - rect.left) / 2, rect.top + (rect.bottom - rect.top) / 2);
	}
}

VECTOR CameraComponent::GetEyeAngle()
{
	VECTOR eyeAngle;
	eyeAngle.x = XMConvertToDegrees(Camera::GetInstance()->GetEyeAngle().x);
	eyeAngle.y = XMConvertToDegrees(Camera::GetInstance()->GetEyeAngle().y);
	eyeAngle.z = XMConvertToDegrees(Camera::GetInstance()->GetEyeAngle().z);
	return eyeAngle;
}

VECTOR CameraComponent::GetTargetPosition()
{
	VECTOR target;
	target.x = Camera::GetInstance()->GetTargetPosition().x;
	target.y = Camera::GetInstance()->GetTargetPosition().y;
	target.z = Camera::GetInstance()->GetTargetPosition().z;
	return target;
}

float CameraComponent::GetZoom()
{
	float zoom = Camera::GetInstance()->GetZoom();
	
	return zoom;
}

void CameraComponent::SetZoom(FLOAT value)
{
	Camera::GetInstance()->SetZoom(value);
}