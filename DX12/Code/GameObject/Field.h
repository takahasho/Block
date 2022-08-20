#ifndef __FIELD_H__
#define __FIELD_H__
#pragma once
// --------------------- Include File --------------------
#include "BaseClass.h"
// ---------------------- �\���̐錾 ---------------------

// ---------------------- �N���X�錾 ---------------------
class Field : public BaseClass
{
private:
	typedef struct _FieldVertex_
	{
		XMFLOAT4 position;				// ���W
		XMFLOAT4 color;					// ���_�̐F
	}FIELDVERTEX;
	// �V�F�[�_�[���ɓn�����߂̊�{�I�ȍs��f�[�^
	struct SceneMatrix
	{
		XMMATRIX world;	// ���f���{�̂���]��������ړ��������肷��s��
		XMMATRIX view;	// �r���[�̍s��
		XMMATRIX projection;// �v���W�F�N�V�����̍s��
		XMFLOAT3 eye;	// ���_���W
	};
	// ---------------- �����o�ϐ��錾 ----------------
	static Field* _instance;				// �V���O���g��
	ComPtr<ID3D12Resource> _vertexBuffer;           // ���_�o�b�t�@�[
	D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;	    // ���_�o�b�t�@�[�r���[

	ComPtr<ID3D12Resource> _indexBuffer;            // �C���f�b�N�X�o�b�t�@�[
	D3D12_INDEX_BUFFER_VIEW _indexBufferView;	    // �C���f�b�N�X�o�b�t�@�[�r���[

	ComPtr<ID3D12DescriptorHeap> _basicDescHeap;	// ��{�f�X�N���v�^�q�[�v

	ComPtr<ID3D12Resource> _constBuffer;			// �R���X�^���g�o�b�t�@�[

	std::vector<FIELDVERTEX> _vertices;				// �o�b�t�@�[�̊m��
	std::vector<unsigned short> _indices;			// �C���f�b�N�X�o�b�t�@�[�̊m��
private:
	HRESULT CreateGraphicsPipelineState();		// �O���t�B�b�N�p�C�v���C���̐���
	HRESULT CreateRootSignature();				// ���[�g�V�O�l�`���[�̐���
	HRESULT CreateVertexIndexBufferView();		// ���_/�C���f�b�N�X�o�b�t�@�[�r���[�̐���
	HRESULT CreateConstantBufferView();			// �R���X�^���g�o�b�t�@�[�r���[�̐���

public:
	Field();									// �R���X�g���N�^�[
	~Field();									// �f�X�g���N�^�[
	static void Create();					// ����
	static void Destroy();					// �j��
	void Draw();								// �`��
	static Field* GetInstance() {
		return _instance;
	}
};

#endif	/* __FIELD_H__ */

