#ifndef __MESH_H__
#define __MESH_H__
#pragma once
// --------------------- Include File --------------------
#include "BaseClass.h"
// ---------------------- �}�N���錾 ---------------------
// ---------------------- �N���X�錾 ---------------------
class Mesh : public BaseClass
{
	static const UINT instanceMax = 500;					// �C���X�^���X�̍ő�l(�V�F�[�_�[�Ɠ��l)
private:
	// ------------------ �\���̐錾 ---------------------

	// �t�я��
	struct MetaData
	{
		XMMATRIX world[instanceMax];	// ���f���{�̂���]��������ړ��������肷��s��
		XMFLOAT4 diffuse[instanceMax];	// �f�B�t���[�Y�F + ��
		XMFLOAT4 specular[instanceMax];	// �X�y�L�����[�F + ����(��Z�l)
		XMFLOAT4 ambient[instanceMax];	// �A���r�G���g�F
	};

	struct DataCheck
	{
		bool _dataCheck[instanceMax];
		DataCheck()
		{
			for (int i = 0; i < instanceMax; i++)
			{
				_dataCheck[i] = false;
			}
		}
	};
private:
	// ---------------- �����o�ϐ��錾 ----------------
	static Mesh* _instance;				// �V���O���g��
	
	ComPtr<ID3D12Resource> boxVertexBuffer;			// ���_�o�b�t�@�[
	ComPtr<ID3D12Resource> boxIndexBuffer;				// �C���f�b�N�X�o�b�t�@�[

	D3D12_VERTEX_BUFFER_VIEW boxVertexBufferView;		// ���_�o�b�t�@�[�r���[
	D3D12_INDEX_BUFFER_VIEW boxIndexBufferView;	    // �C���f�b�N�X�o�b�t�@�[�r���[

	ComPtr<ID3D12Resource> cylinderVertexBuffer;			// ���_�o�b�t�@�[
	ComPtr<ID3D12Resource> cylinderIndexBuffer;				// �C���f�b�N�X�o�b�t�@�[

	D3D12_VERTEX_BUFFER_VIEW cylinderVertexBufferView;		// ���_�o�b�t�@�[�r���[
	D3D12_INDEX_BUFFER_VIEW cylinderIndexBufferView;	    // �C���f�b�N�X�o�b�t�@�[�r���[

	ComPtr<ID3D12Resource> sphereVertexBuffer;			// ���_�o�b�t�@�[
	ComPtr<ID3D12Resource> sphereIndexBuffer;				// �C���f�b�N�X�o�b�t�@�[

	D3D12_VERTEX_BUFFER_VIEW sphereVertexBufferView;		// ���_�o�b�t�@�[�r���[
	D3D12_INDEX_BUFFER_VIEW sphereIndexBufferView;	    // �C���f�b�N�X�o�b�t�@�[�r���[

	ComPtr<ID3D12Resource> circleVertexBuffer;			// ���_�o�b�t�@�[
	ComPtr<ID3D12Resource> circleIndexBuffer;				// �C���f�b�N�X�o�b�t�@�[

	D3D12_VERTEX_BUFFER_VIEW circleVertexBufferView;		// ���_�o�b�t�@�[�r���[
	D3D12_INDEX_BUFFER_VIEW circleIndexBufferView;	    // �C���f�b�N�X�o�b�t�@�[�r���[

	std::vector<MESH*> _boxMeshs;						// ���b�V��
	std::vector<MESH*> _cylinderMeshs;						// ���b�V��
	std::vector<MESH*> _sphereMeshs;						// ���b�V��
	std::vector<MESH*> _circleMeshs;						// ���b�V��

		// �G���g���[�ԍ����g���Ă��邩�ǂ���
	std::vector<MetaData> _boxMetaData;			// ���^���(��)
	std::vector<MetaData> _cylinderMetaData;		// ���^���(�~��)
	std::vector<MetaData> _sphereMetaData;		// ���^���(��)
	std::vector<MetaData> _circleMetaData;		// ���^���(�~)

	UINT _boxInstanceEntry;								// �C���X�^���X�̐�(��)
	UINT _cylinderInstanceEntry;						// �C���X�^���X�̐�(�~��)
	UINT _sphereInstanceEntry;							// �C���X�^���X�̐�(��)
	UINT _circleInstanceEntry;							// �C���X�^���X�̐�(�~)

	// �G���g���[�ԍ����g���Ă��邩�ǂ���
	std::vector<DataCheck> _boxDataCheck;
	std::vector<DataCheck> _cylinderDataCheck;
	std::vector<DataCheck> _sphereDataCheck;
	std::vector<DataCheck> _circleDataCheck;
	unsigned int  _circleIndicesNum;					// Draw�ƃC���f�b�N�X��񐶐����Ɏg���̂Ń��\�b�h��

private:
	HRESULT CreateGraphicsPipelineState();		// �O���t�B�b�N�p�C�v���C���̐���
	HRESULT CreateRootSignature();				// ���[�g�V�O�l�`���[�̐���
	HRESULT CreateVertexIndexBufferView();		// ���_�C���f�b�N�X��񐶐�
	HRESULT CreateMaterialBuffer(MESH* mesh);	// �}�e���A������
	HRESULT CreateConstantBufferView(MESH* mesh);			// �R���X�^���g�o�b�t�@�[�r���[�̐���

public:
	Mesh();										// �R���X�g���N�^�[
	~Mesh();									// �f�X�g���N�^�[
	static void Create();					// ����
	static void Destroy();					// �j��
	void DrawBox();						// �`��
	void DrawCylinder();				// �`��
	void DrawSphere();					// �`��
	void DrawCircle();					// �`��

	// ��
	void BoxDeleate(int& entryPos);
	void SetBoxMetaData(int& entryPos, VECTOR position, VECTOR angle, VECTOR scale, VECTOR color);

	// �~��
	void CylinderDeleate(int& entryPos);
	void SetCylinderMetaData(int& entryPos, VECTOR position, VECTOR angle, VECTOR scale, VECTOR color);

	// ��
	void SphereDeleate(int& entryPos);
	void SetSphereMetaData(int& entryPos, VECTOR position, VECTOR angle, VECTOR scale, VECTOR color);

	// �~
	void CircleDeleate(int& entryPos);
	void SetCircleMetaData(int& entryPos, VECTOR position, VECTOR angle, VECTOR scale, VECTOR color);

	static Mesh* GetInstance() {
		return _instance;
	}
};

#endif	/* __MESH_H__ */