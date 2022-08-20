#ifndef __CAMERA_H__
#define __CAMERA_H__
#pragma once
// -------------------- Define�錾 -------------------
// -------------------- IncludeFile ------------------
#include "../Global.h"
// -------------------- �N���X�錾 -------------------
class Camera
{
private:
	static Camera* _instance;				// �V���O���g��

	XMFLOAT4X4					_viewMat;				// �r���[�s��
	XMFLOAT4X4					_projectionMat;			// �v���W�F�N�V�����s��

	XMFLOAT3					_eyePosition;			// �J�������W
	XMFLOAT3					_targetPosition;		// �J�����̒����_
	XMFLOAT3					_eyeAngle;				// �J�����A���O��
	FLOAT						_zoom;					// �Y�[��(���_�ɃY�[��)
	// ------------------ ���C�g�̐ݒ� ------------------------
	// ���C�g�̌���
	XMFLOAT4 _lightVector = XMFLOAT4(0.1f, 0.2f, -0.3f, 0.0f);
	// ���C�g�̐F
	XMFLOAT4 _lightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1);
public:
	Camera();								// �R���X�g���N�^�[
	~Camera();											// �f�X�g���N�^�[
	void SetFirstPersonCamera();									// �J�����̔z�u
	void SetThirdPersonCamera();									// �J�����̔z�u
	static void Create();						// ����
	static void Destroy();								// �j��
	// �Q�b�^�[�A�Z�b�^�[
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

