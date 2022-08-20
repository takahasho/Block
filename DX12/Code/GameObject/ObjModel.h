#ifndef OBJMODEL_H_
#define OBJMODEL_H_
#pragma once

// --------------------- Include File --------------------
#include "BaseClass.h"
#include "../VECTOR.h"
// ---------------------- �N���X�錾 ---------------------
class ObjModel : public BaseClass
{
	static const UINT instanceMax = 500;					// �C���X�^���X�̍ő�l(�V�F�[�_�[�Ɠ��l)
private:
	// ===== �\���̐錾 =====
	typedef struct _VERTICES_
	{
		XMFLOAT4 position;				// ���W
		XMFLOAT4 normal;				// �@��
		XMFLOAT2 textureUV;				// �e�N�X�`���[UV�l
		_VERTICES_()
		{
			position = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			normal = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			textureUV = XMFLOAT2(0.0f, 0.0f);
		}
	}VERTICES;


	typedef struct _tagMaterial_	// �}�e���A�����
	{
		TCHAR materialName[MAX_PATH];// newmtl:�}�e���A����
		FLOAT Ns;		// �X�y�L�����w��(0����1000)
		FLOAT dissolve;	// �f�B�]���u(1.0 - ���ߗ�)
		FLOAT Tr;		// �����F
		FLOAT Ni;		// ���ܗ�(1�̂Ƃ������Ȃ���Ȃ�)
		int illum;		// �Ɩ����f��(2:�n�C���C�g �L��)
		XMFLOAT4 Ka;	// Ka:�A���r�G���g
		XMFLOAT4 Kd;	// Kd:�f�B�t���[�Y
		XMFLOAT4 Ks;	// Ks:�X�y�L�����[
		TCHAR textureNameKa[MAX_PATH];	// map_Ka:�A���r�G���g�e�N�X�`���[�}�b�v
		TCHAR textureNameKd[MAX_PATH];	// map_Kd:�f�B�t���[�Y�e�N�X�`���[�}�b�v
		TCHAR textureNameKs[MAX_PATH];	// map_Ks:�X�y�L�����J���[�e�N�X�`���[�}�b�v
		TCHAR textureNameNs[MAX_PATH];	// map_Ns:�X�y�L�����n�C���C�g����
		UINT textureNumberKd;			// �f�B�t���[�Y�e�N�X�`���[�ԍ�(���̑��͊���)
		_tagMaterial_()
		{
			materialName[0] = '\0';
			Ns = Tr = Ni = 0.0f;
			illum = 0;
			Ka = Kd = Ks = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			textureNameKa[0] = '\0';
			textureNameKd[0] = '\0';
			textureNameKs[0] = '\0';
			textureNameNs[0] = '\0';
			dissolve = 1.0f;
			textureNumberKd = 0;
		}
	}MATERIAL;

	typedef struct _tagAttribute_	// �������
	{
		// --- ���W�J ---
		TCHAR materialName[MAX_PATH];// usemtl:�}�e���A����
		UINT triangleStartIndex;	// �C���f�b�N�X�̃I�t�Z�b�g
		UINT quadrangleStartIndex;	// �C���f�b�N�X�̃I�t�Z�b�g
		UINT triangleIndexNum;		// �C���f�b�N�X�̐�
		UINT quadrangleIndexNum;	// �C���f�b�N�X�̐�
		// --- �`��֘A ---
		UINT verticesNum;			// �`�撸�_��
		UINT materialNumber;		// �}�e���A���ԍ�
		_tagAttribute_()
		{
			materialName[0] = '\0';
			triangleStartIndex = quadrangleStartIndex = triangleIndexNum = quadrangleIndexNum = 0;
			verticesNum = materialNumber = 0;
		}
	}ATTRIBUTE;

	typedef struct _tagTexture_		// �e�N�X�`���[���
	{
		TCHAR filename[MAX_PATH];	// �t�@�C����
		ComPtr<ID3D12Resource> textureBuffer;					// �e�N�X�`���[�o�b�t�@�[
		_tagTexture_()
		{
			filename[0] = '\0';
		}
	}TEXTURE;

	// �t�я��
	struct MetaData
	{
		XMMATRIX world[instanceMax];	// ���f���{�̂���]��������ړ��������肷��s��
		XMFLOAT4 diffuse[instanceMax];		// �f�B�t���[�Y�F + ��
		XMFLOAT4 specular[instanceMax];		// �X�y�L�����[�F + ����(��Z�l)
		XMFLOAT4 ambient[instanceMax];		// �A���r�G���g�F
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
	// ---------------- �����o�ϐ��錾 ----------------
	static ObjModel* _instance;				// �V���O���g��
	// ===== OBJ�֘A =====
	std::vector<XMFLOAT3>	_position;	// ���_���W
	std::vector<XMFLOAT3>	_normal;	// �@��
	std::vector<XMFLOAT2>	_textureUV;	// �e�N�X�`���[UV�l

	std::vector<UINT>		_triangleIndex;		// �O�p�`�C���f�b�N�X
	std::vector<UINT>		_quadrangleIndex;	// �l�p�`�C���f�b�N�X

	std::vector<ATTRIBUTE>	_attribute;	// �������
	std::vector<MATERIAL>	_material;	// �}�e���A�����		

	std::map<const char*, std::vector<ATTRIBUTE>>	_saveAttribute;	// �������ۑ�
	std::map<const char*, std::vector<MATERIAL>>	_saveMaterial;	// �}�e���A�����ۑ�

	std::map<const char*, std::vector<VECTOR>>	_saveVertices;		// ���_���ۑ�
	std::vector<TEXTURE>	_fileTextures;			// �e�N�X�`���[(�d���o�^�h�~)

	BOOL					_attributeEntryFlag;	// �������o�^�̗L��
	BOOL					_materialEntryFlag;		// �}�e���A�����o�^�̗L��

	TCHAR* _path;						// �p�X(�t�H���_�[)

	std::map<const char*, std::vector<MESH*>>  _objMeshs;	// ���b�V���iobjModel�j

	std::map<const char*, std::vector<std::vector<MetaData>>> _objMetaData;			// ���^���(objModel)

	std::map<const char*, UINT> _objInstanceEntry;		// �C���X�^���X�̐�(objModel)

	std::map<const char*, std::vector<DataCheck>> _objDataCheck;		// �G���g���[�ԍ����g���Ă��邩�ǂ���
	// ===== �`��֘A =====

	ComPtr<ID3D12Device> _dev;				// �f�o�C�X
	ComPtr<ID3D12GraphicsCommandList> _cmdList;	// �R�}���h���X�g

private:

	HRESULT CreateObjModel();				// ����
	HRESULT CreateRootSignature();			// ���[�g�V�O�l�`���[�̐���
	HRESULT CreatePipelineState();			// �`��p�C�v���C���X�e�[�g�̐���
	HRESULT CreateMaterialBuffer(MESH* mesh);			// �}�e���A���o�b�t�@�[�̐���
	HRESULT CreateShaderResource(const TCHAR* pFileName,
		ComPtr<ID3D12Resource>& textureBuffer);	// �V�F�[�_�[���\�[�X�̐���
	HRESULT CreateConstantBufferView(MESH* mesh);		// �R���X�^���g�o�b�t�@�[�r���[�̐���
	BOOL LoadObjFromFile(const char* filename);
	// OBJ�t�@�C���̓ǂݍ���
	BOOL LoadMaterialFromFile(const TCHAR* filename);
	// �}�e���A���̓ǂݍ���
	HRESULT Convert(MESH* mesh, std::vector<VECTOR>& saveVertex);			// �ϊ�
public:
	ObjModel();								// �R���X�g���N�^�[
	~ObjModel();								// �f�X�g���N�^�[
	static void Create();					// ����
	static void Destroy();					// �j��
	void Draw();							// �`��

	// ObjModel
	void ObjModelDeleate(int& entryPos, const char* filename);
	void SetObjModelMetaData(int& entryPos, VECTOR position, VECTOR angle, VECTOR scale, VECTOR color, const char* filename);

	std::vector<VECTOR> GetVertex(const char* fileName){
		return _saveVertices[fileName];
	}
	static ObjModel* GetInstance() {
		return _instance;
	}
};

#endif /* __OBJMODEL_H__ */