// -------------------- IncludeFile -----------------
#include "Camera.h"
Camera* Camera::_instance = nullptr;			// インスタンスの実態
// コンストラクタ
Camera::Camera() :
	 _viewMat(), _projectionMat(),
	_eyePosition(0.0f, 0.0f, 0.0f), _targetPosition(0.0f, 0.0f, 0.0f), _eyeAngle(0.0f, 0.0f, 0.0f), _zoom(12.0f)
{

}
// デストラクタ
Camera::~Camera()
{

}
// 作成
void Camera::Create()
{
	if (!_instance)
		_instance = new Camera();
}
// 破棄
void Camera::Destroy()
{
	delete _instance;
}
void Camera::SetFirstPersonCamera()
{
	// カメラ座標(基本)
	XMVECTOR baseTergetPosition = XMVectorSet(0.0f, 0.0f, +1.0f, 0.0f);
	// カメラアングルを元に、向きを更新し、注視点を決定
	XMMATRIX rotationMat = XMMatrixRotationRollPitchYaw(
		_eyeAngle.x,	// ピッチを指定
		_eyeAngle.y,	// ヨーを指定
		_eyeAngle.z);	// ロールを指定
	baseTergetPosition = XMVector3Transform(
		baseTergetPosition,	// 演算元ベクトル
		rotationMat);		// 演算用行列
	// カメラの注視点
	XMVECTOR targetPosition = XMLoadFloat3(&_eyePosition);
	targetPosition += baseTergetPosition;
	//	カメラの注視点
	XMStoreFloat3(&_targetPosition, targetPosition);
	// カメラの座標
	XMVECTOR eyePosition = XMLoadFloat3(&_eyePosition);

	// カメラの上方(0.0f, 1.0f, 0.0f)
	XMVECTOR eyeUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	eyeUp = XMVector3Transform(
		eyeUp,			// 演算元ベクトル
		rotationMat);	// 演算用行列

	// 左手座標系ビュー行列を作成
	XMMATRIX viewMat = XMMatrixLookAtLH(
		eyePosition,		// 視点を定義したXMVECTOR構造体を指定
		targetPosition,		// 焦点を定義したXMVECTOR構造体を指定
		eyeUp);				// カレントワールドの上方、一般には[0, 1, 0]を定義したXMVECTOR構造体を指定
	// ビュー行列の保存
	XMStoreFloat4x4(&_viewMat, viewMat);

	// 視野(FOV)に基づいて、左手座標系パースペクティブ射影行列を作成
	XMMATRIX projectionMat = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),	// 視野角を指定(ラジアン単位)(45度を指定)
		(float)g_window_width /	// アスペクト比を指定
		(float)g_window_height,
		0.2f,						// 近くのビュープレーンのz値を指定
		1000.0f);					// 遠くのビュープレーンのz値を指定
	// パースペクティブ射影行列の保存
	XMStoreFloat4x4(&_projectionMat, projectionMat);
}

void Camera::SetThirdPersonCamera()
{
	// カメラ座標(基本)
	XMVECTOR baseEyePosition = XMVectorSet(0.0f, 0.0f, -_zoom, 0.0f);
	// カメラアングルを元に、向きを更新し、注視点を決定
	XMMATRIX rotationMat = XMMatrixRotationRollPitchYaw(
		_eyeAngle.x,	// ピッチを指定
		_eyeAngle.y,	// ヨーを指定
		_eyeAngle.z);	// ロールを指定
	baseEyePosition = XMVector3Transform(
		baseEyePosition,	// 演算元ベクトル
		rotationMat);		// 演算用行列
	// カメラの注視点(モデルの中心)
	XMVECTOR targetPosition = XMLoadFloat3(&_targetPosition);
	// カメラの座標
	XMVECTOR eyePosition = targetPosition;
	eyePosition += baseEyePosition;
	// カメラの座標の保存
	XMStoreFloat3(&_eyePosition, eyePosition);

	// カメラの上方(0.0f, 1.0f, 0.0f)
	XMVECTOR eyeUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	eyeUp = XMVector3Transform(
		eyeUp,			// 演算元ベクトル
		rotationMat);	// 演算用行列

	// 左手座標系ビュー行列を作成
	XMMATRIX viewMat = XMMatrixLookAtLH(
		eyePosition,		// 視点を定義したXMVECTOR構造体を指定
		targetPosition,		// 焦点を定義したXMVECTOR構造体を指定
		eyeUp);				// カレントワールドの上方、一般には[0, 1, 0]を定義したXMVECTOR構造体を指定
	// ビュー行列の保存
	XMStoreFloat4x4(&_viewMat, viewMat);

	// 視野(FOV)に基づいて、左手座標系パースペクティブ射影行列を作成
	XMMATRIX projectionMat = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),	// 視野角を指定(ラジアン単位)(45度を指定)
		(float)g_window_width /	// アスペクト比を指定
		(float)g_window_height,
		0.2f,						// 近くのビュープレーンのz値を指定
		7000.0f);					// 遠くのビュープレーンのz値を指定
	// パースペクティブ射影行列の保存
	XMStoreFloat4x4(&_projectionMat, projectionMat);
}
