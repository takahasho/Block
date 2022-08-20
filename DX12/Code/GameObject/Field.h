#ifndef __FIELD_H__
#define __FIELD_H__
#pragma once
// --------------------- Include File --------------------
#include "BaseClass.h"
// ---------------------- 構造体宣言 ---------------------

// ---------------------- クラス宣言 ---------------------
class Field : public BaseClass
{
private:
	typedef struct _FieldVertex_
	{
		XMFLOAT4 position;				// 座標
		XMFLOAT4 color;					// 頂点の色
	}FIELDVERTEX;
	// シェーダー側に渡すための基本的な行列データ
	struct SceneMatrix
	{
		XMMATRIX world;	// モデル本体を回転させたり移動させたりする行列
		XMMATRIX view;	// ビューの行列
		XMMATRIX projection;// プロジェクションの行列
		XMFLOAT3 eye;	// 視点座標
	};
	// ---------------- メンバ変数宣言 ----------------
	static Field* _instance;				// シングルトン
	ComPtr<ID3D12Resource> _vertexBuffer;           // 頂点バッファー
	D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;	    // 頂点バッファービュー

	ComPtr<ID3D12Resource> _indexBuffer;            // インデックスバッファー
	D3D12_INDEX_BUFFER_VIEW _indexBufferView;	    // インデックスバッファービュー

	ComPtr<ID3D12DescriptorHeap> _basicDescHeap;	// 基本デスクリプタヒープ

	ComPtr<ID3D12Resource> _constBuffer;			// コンスタントバッファー

	std::vector<FIELDVERTEX> _vertices;				// バッファーの確保
	std::vector<unsigned short> _indices;			// インデックスバッファーの確保
private:
	HRESULT CreateGraphicsPipelineState();		// グラフィックパイプラインの生成
	HRESULT CreateRootSignature();				// ルートシグネチャーの生成
	HRESULT CreateVertexIndexBufferView();		// 頂点/インデックスバッファービューの生成
	HRESULT CreateConstantBufferView();			// コンスタントバッファービューの生成

public:
	Field();									// コンストラクター
	~Field();									// デストラクター
	static void Create();					// 生成
	static void Destroy();					// 破棄
	void Draw();								// 描画
	static Field* GetInstance() {
		return _instance;
	}
};

#endif	/* __FIELD_H__ */

