#ifndef __POINTLINE_H__
#define __POINTLINE_H__
#pragma once
// -------------------- Define�錾 -------------------
// -------------------- IncludeFile ------------------
#include "BaseClass.h"
#include "ObjectStruct.h"
// -------------------- �N���X�錾 -------------------
class PointLine : public BaseClass
{
	static const UINT instanceMax = 1000;					// �C���X�^���X�̍ő�l(�V�F�[�_�[�Ɠ��l)
private:
	// ---------------- �\���̐錾 ----------------
	// �萔���
	struct ConstantBuffer
	{
		XMFLOAT4X4 viewMat;				// �r���[�s��
		XMFLOAT4X4 projectionMat;		// �v���W�F�N�V�����s��
		XMFLOAT4 startPos[instanceMax];	// �n�_
		XMFLOAT4 endPos[instanceMax];	// �I�_
		XMFLOAT4 color[instanceMax];	// �F
	};

	// �t�я��
	struct MetaData
	{
		XMFLOAT4 startPos[instanceMax];	// �n�_
		XMFLOAT4 endPos[instanceMax];	// �I�_
		XMFLOAT4 color[instanceMax];	// �F
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
	static PointLine* _instance;			// �V���O���g��

	ComPtr<ID3D12Device> _dev;				// �f�o�C�X
	ComPtr<ID3D12GraphicsCommandList> _cmdList;	// �R�}���h���X�g

	ComPtr<ID3D12PipelineState> _pointPipelineState;		// �O���t�B�b�N�p�C�v���C���X�e�[�g�I�u�W�F�N�g(�_)
	ComPtr<ID3D12PipelineState> _linePipelineState;			// �O���t�B�b�N�p�C�v���C���X�e�[�g�I�u�W�F�N�g(��)
	ComPtr<ID3D12RootSignature> _rootSignature;				// ���[�g�V�O�l�`���[

	ComPtr<ID3D12Resource> _pointVertexBuffer;				// ���_�o�b�t�@�[
	D3D12_VERTEX_BUFFER_VIEW _pointVertexBufferView;		// ���_�o�b�t�@�[�r���[
	ComPtr<ID3D12Resource> _pointIndexBuffer;				// �C���f�b�N�X�o�b�t�@�[
	D3D12_INDEX_BUFFER_VIEW _pointIndexBufferView;			// �C���f�b�N�X�o�b�t�@�[�r���[

	ComPtr<ID3D12Resource> _lineVertexBuffer;				// ���_�o�b�t�@�[
	D3D12_VERTEX_BUFFER_VIEW _lineVertexBufferView;			// ���_�o�b�t�@�[�r���[
	ComPtr<ID3D12Resource> _lineIndexBuffer;				// �C���f�b�N�X�o�b�t�@�[
	D3D12_INDEX_BUFFER_VIEW _lineIndexBufferView;			// �C���f�b�N�X�o�b�t�@�[�r���[

	std::vector<MESH*> _pointMeshs;							// ���b�V���i�_�j
	std::vector<MESH*> _lineMeshs;							// ���b�V���i���j

	UINT _pointInstanceEntry;								// �C���X�^���X�̐�(�_)
	UINT _lineInstanceEntry;								// �C���X�^���X�̐�(��)

	std::vector<MetaData> _pointMetaData;				// ���^���(�_)
	std::vector<MetaData> _lineMetaData;				// ���^���(��)

	// �G���g���[�ԍ����g���Ă��邩�ǂ���
	std::vector<DataCheck> _pointDataCheck;
	std::vector<DataCheck> _lineDataCheck;
private:
	HRESULT CreatePointLine();									// ����
	HRESULT CreateRootSignature();			// ���[�g�V�O�l�`���[�̐���
	HRESULT CreatePipelineState();			// �`��p�C�v���C���X�e�[�g�̐���
	HRESULT CreatePointVertexIndexBufferView();	// ���_/�C���f�b�N�X�o�b�t�@�[�̐���(�_)
	HRESULT CreateLineVertexIndexBufferView();	// ���_/�C���f�b�N�X�o�b�t�@�[�̐���(��)
	HRESULT CreatePointConstantBufferView(MESH* mesh);// �R���X�^���g�o�b�t�@�[�r���[�̐���(�_)
	HRESULT CreateLineConstantBufferView(MESH* mesh);// �R���X�^���g�o�b�t�@�[�r���[�̐���(��)

public:
	PointLine();								// �R���X�g���N�^�[
	~PointLine();											// �f�X�g���N�^�[
	static void Create();						// ����
	static void Destroy();								// �j��

	void DrawPoint();									// �`��(�_)
	void DrawLine();									// �`��(��)

	// �_
	// �w�肵���ԍ����폜
	void PointDeleate(int& entryPos);

	void SetPointMetaData(int& entryPos, VECTOR pos, VECTOR color);
	// ��
	// �w�肵���ԍ����폜
	void LineDeleate(int& entryPos);

	void SetLineMetaData(int& entryPos, VECTOR startPos, VECTOR endPos, VECTOR color);

	static PointLine* GetInstance() {
		return _instance;
	}
};
#endif	/* __POINTLINE_H__ */
