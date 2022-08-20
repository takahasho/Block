#ifndef __MESH_H__
#define __MESH_H__
#pragma once
// --------------------- Include File --------------------
#include "BaseClass.h"
// ---------------------- マクロ宣言 ---------------------
// ---------------------- クラス宣言 ---------------------
class Mesh : public BaseClass
{
	static const UINT instanceMax = 500;					// インスタンスの最大値(シェーダーと同値)
private:
	// ------------------ 構造体宣言 ---------------------

	// 付帯情報
	struct MetaData
	{
		XMMATRIX world[instanceMax];	// モデル本体を回転させたり移動させたりする行列
		XMFLOAT4 diffuse[instanceMax];	// ディフューズ色 + α
		XMFLOAT4 specular[instanceMax];	// スペキュラー色 + 強さ(乗算値)
		XMFLOAT4 ambient[instanceMax];	// アンビエント色
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
	// ---------------- メンバ変数宣言 ----------------
	static Mesh* _instance;				// シングルトン
	
	ComPtr<ID3D12Resource> boxVertexBuffer;			// 頂点バッファー
	ComPtr<ID3D12Resource> boxIndexBuffer;				// インデックスバッファー

	D3D12_VERTEX_BUFFER_VIEW boxVertexBufferView;		// 頂点バッファービュー
	D3D12_INDEX_BUFFER_VIEW boxIndexBufferView;	    // インデックスバッファービュー

	ComPtr<ID3D12Resource> cylinderVertexBuffer;			// 頂点バッファー
	ComPtr<ID3D12Resource> cylinderIndexBuffer;				// インデックスバッファー

	D3D12_VERTEX_BUFFER_VIEW cylinderVertexBufferView;		// 頂点バッファービュー
	D3D12_INDEX_BUFFER_VIEW cylinderIndexBufferView;	    // インデックスバッファービュー

	ComPtr<ID3D12Resource> sphereVertexBuffer;			// 頂点バッファー
	ComPtr<ID3D12Resource> sphereIndexBuffer;				// インデックスバッファー

	D3D12_VERTEX_BUFFER_VIEW sphereVertexBufferView;		// 頂点バッファービュー
	D3D12_INDEX_BUFFER_VIEW sphereIndexBufferView;	    // インデックスバッファービュー

	ComPtr<ID3D12Resource> circleVertexBuffer;			// 頂点バッファー
	ComPtr<ID3D12Resource> circleIndexBuffer;				// インデックスバッファー

	D3D12_VERTEX_BUFFER_VIEW circleVertexBufferView;		// 頂点バッファービュー
	D3D12_INDEX_BUFFER_VIEW circleIndexBufferView;	    // インデックスバッファービュー

	std::vector<MESH*> _boxMeshs;						// メッシュ
	std::vector<MESH*> _cylinderMeshs;						// メッシュ
	std::vector<MESH*> _sphereMeshs;						// メッシュ
	std::vector<MESH*> _circleMeshs;						// メッシュ

		// エントリー番号が使われているかどうか
	std::vector<MetaData> _boxMetaData;			// メタ情報(箱)
	std::vector<MetaData> _cylinderMetaData;		// メタ情報(円柱)
	std::vector<MetaData> _sphereMetaData;		// メタ情報(球)
	std::vector<MetaData> _circleMetaData;		// メタ情報(円)

	UINT _boxInstanceEntry;								// インスタンスの数(箱)
	UINT _cylinderInstanceEntry;						// インスタンスの数(円柱)
	UINT _sphereInstanceEntry;							// インスタンスの数(球)
	UINT _circleInstanceEntry;							// インスタンスの数(円)

	// エントリー番号が使われているかどうか
	std::vector<DataCheck> _boxDataCheck;
	std::vector<DataCheck> _cylinderDataCheck;
	std::vector<DataCheck> _sphereDataCheck;
	std::vector<DataCheck> _circleDataCheck;
	unsigned int  _circleIndicesNum;					// Drawとインデックス情報生成時に使うのでメソッド化

private:
	HRESULT CreateGraphicsPipelineState();		// グラフィックパイプラインの生成
	HRESULT CreateRootSignature();				// ルートシグネチャーの生成
	HRESULT CreateVertexIndexBufferView();		// 頂点インデックス情報生成
	HRESULT CreateMaterialBuffer(MESH* mesh);	// マテリアル生成
	HRESULT CreateConstantBufferView(MESH* mesh);			// コンスタントバッファービューの生成

public:
	Mesh();										// コンストラクター
	~Mesh();									// デストラクター
	static void Create();					// 生成
	static void Destroy();					// 破棄
	void DrawBox();						// 描画
	void DrawCylinder();				// 描画
	void DrawSphere();					// 描画
	void DrawCircle();					// 描画

	// 箱
	void BoxDeleate(int& entryPos);
	void SetBoxMetaData(int& entryPos, VECTOR position, VECTOR angle, VECTOR scale, VECTOR color);

	// 円柱
	void CylinderDeleate(int& entryPos);
	void SetCylinderMetaData(int& entryPos, VECTOR position, VECTOR angle, VECTOR scale, VECTOR color);

	// 球
	void SphereDeleate(int& entryPos);
	void SetSphereMetaData(int& entryPos, VECTOR position, VECTOR angle, VECTOR scale, VECTOR color);

	// 円
	void CircleDeleate(int& entryPos);
	void SetCircleMetaData(int& entryPos, VECTOR position, VECTOR angle, VECTOR scale, VECTOR color);

	static Mesh* GetInstance() {
		return _instance;
	}
};

#endif	/* __MESH_H__ */