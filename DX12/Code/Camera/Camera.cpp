// -------------------- IncludeFile -----------------
#include "Camera.h"
Camera* Camera::_instance = nullptr;			// �C���X�^���X�̎���
// �R���X�g���N�^
Camera::Camera() :
	 _viewMat(), _projectionMat(),
	_eyePosition(0.0f, 0.0f, 0.0f), _targetPosition(0.0f, 0.0f, 0.0f), _eyeAngle(0.0f, 0.0f, 0.0f), _zoom(12.0f)
{

}
// �f�X�g���N�^
Camera::~Camera()
{

}
// �쐬
void Camera::Create()
{
	if (!_instance)
		_instance = new Camera();
}
// �j��
void Camera::Destroy()
{
	delete _instance;
}
void Camera::SetFirstPersonCamera()
{
	// �J�������W(��{)
	XMVECTOR baseTergetPosition = XMVectorSet(0.0f, 0.0f, +1.0f, 0.0f);
	// �J�����A���O�������ɁA�������X�V���A�����_������
	XMMATRIX rotationMat = XMMatrixRotationRollPitchYaw(
		_eyeAngle.x,	// �s�b�`���w��
		_eyeAngle.y,	// ���[���w��
		_eyeAngle.z);	// ���[�����w��
	baseTergetPosition = XMVector3Transform(
		baseTergetPosition,	// ���Z���x�N�g��
		rotationMat);		// ���Z�p�s��
	// �J�����̒����_
	XMVECTOR targetPosition = XMLoadFloat3(&_eyePosition);
	targetPosition += baseTergetPosition;
	//	�J�����̒����_
	XMStoreFloat3(&_targetPosition, targetPosition);
	// �J�����̍��W
	XMVECTOR eyePosition = XMLoadFloat3(&_eyePosition);

	// �J�����̏��(0.0f, 1.0f, 0.0f)
	XMVECTOR eyeUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	eyeUp = XMVector3Transform(
		eyeUp,			// ���Z���x�N�g��
		rotationMat);	// ���Z�p�s��

	// ������W�n�r���[�s����쐬
	XMMATRIX viewMat = XMMatrixLookAtLH(
		eyePosition,		// ���_���`����XMVECTOR�\���̂��w��
		targetPosition,		// �œ_���`����XMVECTOR�\���̂��w��
		eyeUp);				// �J�����g���[���h�̏���A��ʂɂ�[0, 1, 0]���`����XMVECTOR�\���̂��w��
	// �r���[�s��̕ۑ�
	XMStoreFloat4x4(&_viewMat, viewMat);

	// ����(FOV)�Ɋ�Â��āA������W�n�p�[�X�y�N�e�B�u�ˉe�s����쐬
	XMMATRIX projectionMat = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),	// ����p���w��(���W�A���P��)(45�x���w��)
		(float)g_window_width /	// �A�X�y�N�g����w��
		(float)g_window_height,
		0.2f,						// �߂��̃r���[�v���[����z�l���w��
		1000.0f);					// �����̃r���[�v���[����z�l���w��
	// �p�[�X�y�N�e�B�u�ˉe�s��̕ۑ�
	XMStoreFloat4x4(&_projectionMat, projectionMat);
}

void Camera::SetThirdPersonCamera()
{
	// �J�������W(��{)
	XMVECTOR baseEyePosition = XMVectorSet(0.0f, 0.0f, -_zoom, 0.0f);
	// �J�����A���O�������ɁA�������X�V���A�����_������
	XMMATRIX rotationMat = XMMatrixRotationRollPitchYaw(
		_eyeAngle.x,	// �s�b�`���w��
		_eyeAngle.y,	// ���[���w��
		_eyeAngle.z);	// ���[�����w��
	baseEyePosition = XMVector3Transform(
		baseEyePosition,	// ���Z���x�N�g��
		rotationMat);		// ���Z�p�s��
	// �J�����̒����_(���f���̒��S)
	XMVECTOR targetPosition = XMLoadFloat3(&_targetPosition);
	// �J�����̍��W
	XMVECTOR eyePosition = targetPosition;
	eyePosition += baseEyePosition;
	// �J�����̍��W�̕ۑ�
	XMStoreFloat3(&_eyePosition, eyePosition);

	// �J�����̏��(0.0f, 1.0f, 0.0f)
	XMVECTOR eyeUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	eyeUp = XMVector3Transform(
		eyeUp,			// ���Z���x�N�g��
		rotationMat);	// ���Z�p�s��

	// ������W�n�r���[�s����쐬
	XMMATRIX viewMat = XMMatrixLookAtLH(
		eyePosition,		// ���_���`����XMVECTOR�\���̂��w��
		targetPosition,		// �œ_���`����XMVECTOR�\���̂��w��
		eyeUp);				// �J�����g���[���h�̏���A��ʂɂ�[0, 1, 0]���`����XMVECTOR�\���̂��w��
	// �r���[�s��̕ۑ�
	XMStoreFloat4x4(&_viewMat, viewMat);

	// ����(FOV)�Ɋ�Â��āA������W�n�p�[�X�y�N�e�B�u�ˉe�s����쐬
	XMMATRIX projectionMat = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),	// ����p���w��(���W�A���P��)(45�x���w��)
		(float)g_window_width /	// �A�X�y�N�g����w��
		(float)g_window_height,
		0.2f,						// �߂��̃r���[�v���[����z�l���w��
		7000.0f);					// �����̃r���[�v���[����z�l���w��
	// �p�[�X�y�N�e�B�u�ˉe�s��̕ۑ�
	XMStoreFloat4x4(&_projectionMat, projectionMat);
}
