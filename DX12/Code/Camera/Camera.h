#ifndef __CAMERA_H__
#define __CAMERA_H__
#pragma once
// -------------------- Define宣言 -------------------
// -------------------- IncludeFile ------------------
#include "../Global.h"
// -------------------- クラス宣言 -------------------
class Camera
{
private:
	static Camera* _instance;				// シングルトン

	XMFLOAT4X4					_viewMat;				// ビュー行列
	XMFLOAT4X4					_projectionMat;			// プロジェクション行列

	XMFLOAT3					_eyePosition;			// カメラ座標
	XMFLOAT3					_targetPosition;		// カメラの注視点
	XMFLOAT3					_eyeAngle;				// カメラアングル
	FLOAT						_zoom;					// ズーム(原点にズーム)
	// ------------------ ライトの設定 ------------------------
	// ライトの向き
	XMFLOAT4 _lightVector = XMFLOAT4(0.1f, 0.2f, -0.3f, 0.0f);
	// ライトの色
	XMFLOAT4 _lightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1);
public:
	Camera();								// コンストラクター
	~Camera();											// デストラクター
	void SetFirstPersonCamera();									// カメラの配置
	void SetThirdPersonCamera();									// カメラの配置
	static void Create();						// 生成
	static void Destroy();								// 破棄
	// ゲッター、セッター
	XMMATRIX GetView() const {
		return XMLoadFloat4x4(&_viewMat);
	}
	XMMATRIX GetProjection() const {
		return XMLoadFloat4x4(&_projectionMat);
	}
	XMFLOAT3 GetEyePosition() const {
		return _eyePosition;
	}
	void SetEyePosition(XMFLOAT3 value) {
		_eyePosition = value;
	}
	XMFLOAT3 GetTargetPosition() const {
		return _targetPosition;
	}
	void SetTargetPosition(XMFLOAT3 value)
	{
		_targetPosition = value;
	}
	FLOAT GetZoom() const {
		return _zoom;
	}
	void SetZoom(FLOAT value) {
		_zoom = value;
	}
	XMFLOAT3 GetEyeAngle() const {
		return _eyeAngle;
	}
	void SetEyeAngle(XMFLOAT3 value) {
		_eyeAngle = value;
	}
	XMFLOAT4 GetLightVector() const {
		return _lightVector;
	}
	void SetLightVector(XMFLOAT4 value) {
		_lightVector = value;
	}
	XMFLOAT4 GetLightColor() const {
		return _lightColor;
	}
	void SetLightColor(XMFLOAT4 value) {
		_lightColor = value;
	}
	static Camera* GetInstance() {
		return _instance;
	}
};
#endif	/* __CAMERA_H__ */

