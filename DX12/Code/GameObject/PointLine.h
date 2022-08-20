#ifndef __POINTLINE_H__
#define __POINTLINE_H__
#pragma once
// -------------------- Define宣言 -------------------
// -------------------- IncludeFile ------------------
#include "BaseClass.h"
#include "ObjectStruct.h"
// -------------------- クラス宣言 -------------------
class PointLine : public BaseClass
{
	static const UINT instanceMax = 1000;					// インスタンスの最大値(シェーダーと同値)
private:
	// ---------------- 構造体宣言 ----------------
	// 定数情報
	struct ConstantBuffer
	{
		XMFLOAT4X4 viewMat;				// ビュー行列
		XMFLOAT4X4 projectionMat;		// プロジェクション行列
		XMFLOAT4 startPos[instanceMax];	// 始点
		XMFLOAT4 endPos[instanceMax];	// 終点
		XMFLOAT4 color[instanceMax];	// 色
	};

	// 付帯情報
	struct MetaData
	{
		XMFLOAT4 startPos[instanceMax];	// 始点
		XMFLOAT4 endPos[instanceMax];	// 終点
		XMFLOAT4 color[instanceMax];	// 色
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
	static PointLine* _instance;			// シングルトン

	ComPtr<ID3D12Device> _dev;				// デバイス
	ComPtr<ID3D12GraphicsCommandList> _cmdList;	// コマンドリスト

	ComPtr<ID3D12PipelineState> _pointPipelineState;		// グラフィックパイプラインステートオブジェクト(点)
	ComPtr<ID3D12PipelineState> _linePipelineState;			// グラフィックパイプラインステートオブジェクト(線)
	ComPtr<ID3D12RootSignature> _rootSignature;				// ルートシグネチャー

	ComPtr<ID3D12Resource> _pointVertexBuffer;				// 頂点バッファー
	D3D12_VERTEX_BUFFER_VIEW _pointVertexBufferView;		// 頂点バッファービュー
	ComPtr<ID3D12Resource> _pointIndexBuffer;				// インデックスバッファー
	D3D12_INDEX_BUFFER_VIEW _pointIndexBufferView;			// インデックスバッファービュー

	ComPtr<ID3D12Resource> _lineVertexBuffer;				// 頂点バッファー
	D3D12_VERTEX_BUFFER_VIEW _lineVertexBufferView;			// 頂点バッファービュー
	ComPtr<ID3D12Resource> _lineIndexBuffer;				// インデックスバッファー
	D3D12_INDEX_BUFFER_VIEW _lineIndexBufferView;			// インデックスバッファービュー

	std::vector<MESH*> _pointMeshs;							// メッシュ（点）
	std::vector<MESH*> _lineMeshs;							// メッシュ（線）

	UINT _pointInstanceEntry;								// インスタンスの数(点)
	UINT _lineInstanceEntry;								// インスタンスの数(線)

	std::vector<MetaData> _pointMetaData;				// メタ情報(点)
	std::vector<MetaData> _lineMetaData;				// メタ情報(線)

	// エントリー番号が使われているかどうか
	std::vector<DataCheck> _pointDataCheck;
	std::vector<DataCheck> _lineDataCheck;
private:
	HRESULT CreatePointLine();									// 生成
	HRESULT CreateRootSignature();			// ルートシグネチャーの生成
	HRESULT CreatePipelineState();			// 描画パイプラインステートの生成
	HRESULT CreatePointVertexIndexBufferView();	// 頂点/インデックスバッファーの生成(点)
	HRESULT CreateLineVertexIndexBufferView();	// 頂点/インデックスバッファーの生成(線)
	HRESULT CreatePointConstantBufferView(MESH* mesh);// コンスタントバッファービューの生成(点)
	HRESULT CreateLineConstantBufferView(MESH* mesh);// コンスタントバッファービューの生成(線)

public:
	PointLine();								// コンストラクター
	~PointLine();											// デストラクター
	static void Create();						// 生成
	static void Destroy();								// 破棄

	void DrawPoint();									// 描画(点)
	void DrawLine();									// 描画(線)

	// 点
	// 指定した番号を削除
	void PointDeleate(int& entryPos);

	void SetPointMetaData(int& entryPos, VECTOR pos, VECTOR color);
	// 線
	// 指定した番号を削除
	void LineDeleate(int& entryPos);

	void SetLineMetaData(int& entryPos, VECTOR startPos, VECTOR endPos, VECTOR color);

	static PointLine* GetInstance() {
		return _instance;
	}
};
#endif	/* __POINTLINE_H__ */
