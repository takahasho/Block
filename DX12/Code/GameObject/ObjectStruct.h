#pragma once
// --------------------- Include File --------------------
#include "../Global.h"
#include "../VECTOR.h"
#include "../VECTOR2.h"
// -------------- 列挙型宣言 -------------
enum class ObjectType
{
	Box = 0,
	Cylinder = 1,
	Sphere = 2,
	Triangle = 3,
	Square = 4,
	TypeNone = 5,
};
// ---------------------- 構造体宣言 ---------------------
// シェーダー側に渡すための基本的な行列データ
struct ConstantBuffer
{
	XMMATRIX view;	// ビューの行列
	XMMATRIX projection;// プロジェクションの行列
	XMFLOAT4 lightVector;	// ライトの向き
	XMFLOAT4 lightColor;	// ライトの色
	XMFLOAT3 eye;	// 視点座標
};

typedef struct _MESHVERTEX_
{
	XMFLOAT4 position;				// 座標
	XMFLOAT4 normal;				// 法線
	_MESHVERTEX_()
	{
		position = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		normal = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
}MESHVERTEX;
// 頂点データ構造体
struct Vertex2D
{
	XMFLOAT3 pos;	// xyz座標
	XMFLOAT3 normal;// uv座標
};
// MESH情報
typedef struct _Mesh_
{
	ComPtr<ID3D12Resource> vertexBuffer;			// 頂点バッファー
	ComPtr<ID3D12Resource> indexBuffer;				// インデックスバッファー

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;		// 頂点バッファービュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView;	    // インデックスバッファービュー

	std::vector<ComPtr<ID3D12Resource>> textureBuffer;			// テクスチャーバッファー

	ComPtr<ID3D12DescriptorHeap> basicDescHeap;	// 基本デスクリプタヒープ
	ComPtr<ID3D12Resource> constBuffer;			// コンスタントバッファー
	ComPtr<ID3D12Resource> materialBuffer;			// マテリアルバッファー
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