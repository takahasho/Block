#ifndef __PMDMODEL_H__
#define __PMDMODEL_H__
#pragma once
// --------------------- Include File --------------------
#include "BaseClass.h"
// ---------------------- �ϐ��錾 ---------------------
constexpr size_t _pmdVertex_size = 38;					// ���_�P������̃T�C�Y
// ---------------------- �\���̐錾 --------------------
// ======== PMD���f�� ========
#pragma pack(1)	// ��������P�o�C�g�p�b�L���O�ƂȂ�A�A���C�����g�͔������Ȃ�
// PMD�}�e���A���\����
struct PMDMaterial
{
	XMFLOAT3 diffuse;		// �f�B�t���[�Y�F
	FLOAT alpha;			// �f�B�t���[�Y��
	FLOAT specularity;		// �X�y�L�����[�̋���(��Z�l)
	XMFLOAT3 specular;		// �X�y�L�����[�F
	XMFLOAT3 ambient;		// �A���r�G���g�F
	unsigned char toonIdx;	// �g�D�[���ԍ�(��q)
	unsigned char edgeFlg;	// �}�e���A�����Ƃ̗֊s���t���O

	// �Q�o�C�g�̃p�f�B���O���������Ȃ�

	unsigned int indicesNum;// ���̃}�e���A�������蓖�Ă���
							// �C���f�b�N�X��(��q)
	char texFilePath[20];	// �e�N�X�`���[�t�@�C���p�X�{��(��q)
};	// �p�f�B���O���������Ȃ�����70�o�C�g

#pragma pack()	// �p�b�L���O�w�������(�f�t�H���g�ɖ߂�)

// �V�F�[�_�[���ɓ�������}�e���A���f�[�^
struct MaterialForHlsl
{
	XMFLOAT3 diffuse;		// �f�B�t���[�Y�F
	FLOAT alpha;			// �f�B�t���[�Y��
	XMFLOAT3 specular;		// �X�y�L�����[�F
	FLOAT specularity;		// �X�y�L�����[�̋���(��Z�l)
	XMFLOAT3 ambient;		// �A���r�G���g�F
};

// �V�F�[�_�[���ȊO�̃}�e���A���f�[�^
struct AdditionalMaterial
{
	std::string texPath;	// �e�N�X�`���[�t�@�C���p�X
	int toonIdx;			// �g�D�[���ԍ�
	bool edgeFlg;			// �}�e���A�����Ƃ̗֊s���t���O
	AdditionalMaterial()
	{
		toonIdx = 0;
		edgeFlg = false;
	}
};

// �S�̂��܂Ƃ܂�f�[�^
struct AllPMDMaterial
{
	unsigned int indicesNum;// �C���f�b�N�X��
	MaterialForHlsl material;
	AdditionalMaterial additional;
	AllPMDMaterial()
	{
		indicesNum = 0;
		material = {};
	}
};

// MESH���
typedef struct _PmdMesh_
{
	ComPtr<ID3D12Resource> vertexBuffer;			// ���_�o�b�t�@�[
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;		// ���_�o�b�t�@�[�r���[

	ComPtr<ID3D12Resource> indexBuffer;				// �C���f�b�N�X�o�b�t�@�[
	D3D12_INDEX_BUFFER_VIEW indexBufferView;	    // �C���f�b�N�X�o�b�t�@�[�r���[

	ComPtr<ID3D12Resource> materialBuffer;					// �}�e���A���o�b�t�@�[

	std::vector<ComPtr<ID3D12Resource>> texBuffer;			// �e�N�X�`���[�o�b�t�@�[
	std::vector<ComPtr<ID3D12Resource>> sphTexBuffer;		// �e�N�X�`���[�o�b�t�@�[(��Z�X�t�B�A)
	std::vector<ComPtr<ID3D12Resource>> spaTexBuffer;		// �e�N�X�`���[�o�b�t�@�[(���Z�X�t�B�A)
	std::vector<ComPtr<ID3D12Resource>> toonTexBuffer;		// �g�D�[���e�N�X�`���[�o�b�t�@�[

	ComPtr<ID3D12DescriptorHeap> basicDescHeap;	// ��{�f�X�N���v�^�q�[�v
	ComPtr<ID3D12Resource> constBuffer;			// �R���X�^���g�o�b�t�@�[

	// ----- pmd�ǂݍ��ݏ�� ------
	unsigned int verticesNum = 0;							// ���_��
	std::vector<unsigned char> vertices;					// ���_���

	unsigned int indicesNum = 0;							// �C���f�b�N�X��
	std::vector<unsigned short> indices;					// �C���f�b�N�X���

	unsigned int materialNum = 0;							// �}�e���A����
	std::vector<PMDMaterial> pmdMaterials;					// �}�e���A�����
	std::vector<AllPMDMaterial> materials;						// �}�e���A�����
	// ----------------------------
	_PmdMesh_()
	{
		vertexBufferView = {};
		indexBufferView = {};
	}
}PMDMESH;

// �Q�[�����̈ʒu�Ȃǂ̏��
typedef struct _PMDModel_
{
	PMDMESH mesh;					// ���b�V��
	XMFLOAT3 position;				// ���W
	XMFLOAT3 angle;					// �A���O��
	XMFLOAT3 size;					// �傫��

	_PMDModel_()
	{
		mesh = PMDMESH();
		position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		angle = XMFLOAT3(0.0f, 0.0f, 0.0f);
		size = XMFLOAT3(1.0f, 1.0f, 1.0f);
	}
}PMDMODEL;
// ---------------------- �N���X�錾 ---------------------
class PmdModel : public BaseClass
{
private:
	// ---------------- �\���̐錾 ----------------
	// // PMD�w�b�_�[
	struct PMDHeader
	{
		float version;			// ��:00 00 80 3F == 1.00
		char model_name[20];	// ���f����
		char comment[256];		// ���f���R�����g
	};

	// PMD���_�\����
	struct PMDVertex
	{
		XMFLOAT3 pos;			// ���_���W:12�o�C�g
		XMFLOAT3 normal;		// �@���x�N�g��:12�o�C�g
		XMFLOAT2 uv;			// uv���W:8�o�C�g
		unsigned short boneNo[2];// �{�[���ԍ�(��q):4�o�C�g
		unsigned char boneWeight;// �{�[���e���x(��q):1�o�C�g
		unsigned char edgeFlg;	// �֊s���t���O:1�o�C�g
	};	// ���v38�o�C�g
	// �V�F�[�_�[���ɓn�����߂̊�{�I�ȍs��f�[�^
	struct SceneMatrix
	{
		XMMATRIX world;			// ���[���h�s��
		XMMATRIX view;			// �r���[�s��
		XMMATRIX Projection;    // �v���W�F�N�V�����s��
		XMFLOAT3 eye;			// ���_���W
		XMFLOAT4 LightVector;	// ���C�g����
	};
private:
	// ---------------- �����o�ϐ��錾 ----------------
	static PmdModel* _instance;				// �V���O���g��
	std::map<std::string, ComPtr<ID3D12Resource>*>	_resourceTable;
private:

	HRESULT CreateGraphicsPipelineState();		// �O���t�B�b�N�p�C�v���C���̐���
	HRESULT CreateRootSignature();				// ���[�g�V�O�l�`���[�̐���

	HRESULT LoadPMD(PMDMODEL& model, const char* file_name);			// pmd�t�@�C���̓ǂݍ���
	HRESULT CreateVertexIndexBufferView(PMDMODEL& model);		// ���_�C���f�b�N�X�o�b�t�@�[�r���[�̐���
	HRESULT CreateTexture(PMDMODEL& model);			// �e�N�X�`���̐���
	HRESULT CreateConstantBufferView(PMDMODEL& model);			// �R���X�^���g�o�b�t�@�[�r���[�̐���

	HRESULT CreateShaderResource(
		ComPtr<ID3D12Resource>* texBuffer,
		const std::string fileName);			// �V�F�[�_�[���\�[�X�̐���

	HRESULT CreateMaterialBuffer(PMDMODEL& model, const char* file_name);	// �}�e���A���o�b�t�@�[�̐���

	std::string GetTexturePathFromModelAndTexPath(
		const std::string& modelPath,
		const char* texPath)
	{
		int pathIndex1 = (int)modelPath.rfind('/');
		int pathIndex2 = (int)modelPath.rfind('\\');

		auto pathIndex = max(pathIndex1, pathIndex2);
		auto folderPath = modelPath.substr(0, pathIndex);
		return folderPath + "\\" + texPath;
	}

	// �t�@�C��������g���q���擾
	std::string GetExtension(const std::string& path)
	{
		int idx = (int)path.rfind('.');
		return path.substr((size_t)idx + 1, path.length() - idx - 1);
	}
	// �e�N�X�`���[�̃Z�p���[�^�[�����ŕ�������
	std::pair<std::string, std::string> SplitFileName(
		const std::string& path, const char splitter = '*')
	{
		int idx = (int)path.find(splitter);
		std::pair<std::string, std::string> ret;
		ret.first = path.substr(0, idx);
		ret.second = path.substr(
			(size_t)idx + 1, path.length() - idx - 1);
		return ret;
	}

public:
	PmdModel();										// �R���X�g���N�^�[
	~PmdModel();									// �f�X�g���N�^�[
	static void Create();					// ����
	static void Destroy();					// �j��
	HRESULT CreatePMD(PMDMODEL& model, const char* file_name);		// ����
	void Draw(PMDMODEL model);					// �`��
	static PmdModel* GetInstance() {
		return _instance;
	}
};

#endif	/* __PMDMODEL_H__ */