#ifndef __TEXTURE_H__
#define __TEXTURE_H__
#pragma once
// --------------------- Include File --------------------
#include "BaseClass.h"
#include "GeometryData.h"
// ---------------------- �\���̐錾 ---------------------
// // Texture�f�[�^�\����
struct TextureVertex
{
	XMFLOAT3 pos;	// xyz���W
	XMFLOAT2 uv;	// uv���W
};
// ---------------------- �N���X�錾 ---------------------
class Texture : public BaseClass
{
	static const UINT instanceMax = 500;					// �C���X�^���X�̍ő�l(�V�F�[�_�[�Ɠ��l)
private:
	// ------------------ �\���̐錾 ---------------------

	// �t�я��
	struct MetaData
	{
		XMMATRIX world;	// ���f���{�̂���]��������ړ��������肷��s��
		XMFLOAT4 color;	// �F
	};

	struct Info
	{
		VECTOR position;
		VECTOR angle;
		VECTOR scale;
		VECTOR color;
		float alpha;
		bool uiFlag;
		Info()
		{
			alpha = 1;
			uiFlag = false;
		}
	};

	// �Q�[�����̈ʒu�Ȃǂ̏��
	struct Object
	{
		MESH* mesh;
		Info info;
	};

	struct ImageSize
	{
		float width, height;
		ImageSize()
		{
			width = height = 0;
		}
	};
	// ---------------- �����o�ϐ��錾 ----------------
	static Texture* _instance;				// �V���O���g��

	// ===== OBJ�֘A =====
	std::vector<Object> _object;						// mesh,info�f�[�^�i�[

	std::vector<int> _drawOrder;							// �`�揇

	std::vector<int> _drawflag;							// �`�悷�邩�ǂ���

	int _orderNumMax = 0;									// �ő�`��ԍ�			

	std::map<const char*, MESH*> _mesh;				// �t�@�C��Mesh

	std::map<const char*, ImageSize> _size;				// �t�@�C�����Ƃ̉摜�T�C�Y

	// ===== �`��֘A =====

	ComPtr<ID3D12Device> _dev;				// �f�o�C�X
	ComPtr<ID3D12GraphicsCommandList> _cmdList;	// �R�}���h���X�g
	ComPtr<ID3D12CommandAllocator> _cmdAllocator;	// �R�}���h�A���P�[�^�[
private:
	HRESULT CreateTexture();				// ����
	HRESULT CreateGraphicsPipelineState();		// �O���t�B�b�N�p�C�v���C���̐���
	HRESULT CreateRootSignature();				// ���[�g�V�O�l�`���[�̐���
	HRESULT CreateVertexIndexBufferView(MESH* mesh, float width, float height);// �v���[������
	HRESULT CreateShaderResource(const char* pFileName, ComPtr<ID3D12Resource>& textureBuffer, SIZE& size);// �V�F�[�_�[���\�[�X�̐���
	HRESULT CreateConstantBufferView(MESH* mesh);			// �R���X�^���g�o�b�t�@�[�r���[�̐���

public:
	Texture();										// �R���X�g���N�^�[
	~Texture();									// �f�X�g���N�^�[
	static void Create();					// ����
	static void Destroy();					// �j��
	void Draw();							// �`��
	// Texture
	void TextureDeleate(int& entryPos, const char* filename);
	void SetTextureMetaData(int& entryPos, VECTOR position, VECTOR angle, VECTOR scale, VECTOR color, float a, int drawOrder, bool uiFlag, const char* filename);
	void GetSize(const char* filename, float& width, float& height);

	static Texture* GetInstance() {
		return _instance;
	}
};

#endif	/* __TEXTURE_H__ */