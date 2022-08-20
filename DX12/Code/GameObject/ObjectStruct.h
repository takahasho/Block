#pragma once
// --------------------- Include File --------------------
#include "../Global.h"
#include "../VECTOR.h"
#include "../VECTOR2.h"
// -------------- �񋓌^�錾 -------------
enum class ObjectType
{
	Box = 0,
	Cylinder = 1,
	Sphere = 2,
	Triangle = 3,
	Square = 4,
	TypeNone = 5,
};
// ---------------------- �\���̐錾 ---------------------
// �V�F�[�_�[���ɓn�����߂̊�{�I�ȍs��f�[�^
struct ConstantBuffer
{
	XMMATRIX view;	// �r���[�̍s��
	XMMATRIX projection;// �v���W�F�N�V�����̍s��
	XMFLOAT4 lightVector;	// ���C�g�̌���
	XMFLOAT4 lightColor;	// ���C�g�̐F
	XMFLOAT3 eye;	// ���_���W
};

typedef struct _MESHVERTEX_
{
	XMFLOAT4 position;				// ���W
	XMFLOAT4 normal;				// �@��
	_MESHVERTEX_()
	{
		position = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		normal = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
}MESHVERTEX;
// ���_�f�[�^�\����
struct Vertex2D
{
	XMFLOAT3 pos;	// xyz���W
	XMFLOAT3 normal;// uv���W
};
// MESH���
typedef struct _Mesh_
{
	ComPtr<ID3D12Resource> vertexBuffer;			// ���_�o�b�t�@�[
	ComPtr<ID3D12Resource> indexBuffer;				// �C���f�b�N�X�o�b�t�@�[

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;		// ���_�o�b�t�@�[�r���[
	D3D12_INDEX_BUFFER_VIEW indexBufferView;	    // �C���f�b�N�X�o�b�t�@�[�r���[

	std::vector<ComPtr<ID3D12Resource>> textureBuffer;			// �e�N�X�`���[�o�b�t�@�[

	ComPtr<ID3D12DescriptorHeap> basicDescHeap;	// ��{�f�X�N���v�^�q�[�v
	ComPtr<ID3D12Resource> constBuffer;			// �R���X�^���g�o�b�t�@�[
	ComPtr<ID3D12Resource> materialBuffer;			// �}�e���A���o�b�t�@�[
	_Mesh_()
	{
		vertexBuffer = nullptr;
		indexBuffer = nullptr;
		textureBuffer.resize(1);
		basicDescHeap = nullptr;
		constBuffer = nullptr;
		materialBuffer = nullptr;
		vertexBufferView = {};
		indexBufferView = {};
	}
}MESH;