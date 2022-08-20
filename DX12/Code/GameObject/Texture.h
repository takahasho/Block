#ifndef __TEXTURE_H__
#define __TEXTURE_H__
#pragma once
// --------------------- Include File --------------------
#include "BaseClass.h"
#include "GeometryData.h"
// ---------------------- 構造体宣言 ---------------------
// // Textureデータ構造体
struct TextureVertex
{
	XMFLOAT3 pos;	// xyz座標
	XMFLOAT2 uv;	// uv座標
};
// ---------------------- クラス宣言 ---------------------
class Texture : public BaseClass
{
	static const UINT instanceMax = 500;					// インスタンスの最大値(シェーダーと同値)
private:
	// ------------------ 構造体宣言 ---------------------

	// 付帯情報
	struct MetaData
	{
		XMMATRIX world;	// モデル本体を回転させたり移動させたりする行列
		XMFLOAT4 color;	// 色
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

	// ゲーム内の位置などの情報
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
	// ---------------- メンバ変数宣言 ----------------
	static Texture* _instance;				// シングルトン

	// ===== OBJ関連 =====
	std::vector<Object> _object;						// mesh,infoデータ格納

	std::vector<int> _drawOrder;							// 描画順

	std::vector<int> _drawflag;							// 描画するかどうか

	int _orderNumMax = 0;									// 最大描画番号			

	std::map<const char*, MESH*> _mesh;				// ファイルMesh

	std::map<const char*, ImageSize> _size;				// ファイルごとの画像サイズ

	// ===== 描画関連 =====

	ComPtr<ID3D12Device> _dev;				// デバイス
	ComPtr<ID3D12GraphicsCommandList> _cmdList;	// コマンドリスト
	ComPtr<ID3D12CommandAllocator> _cmdAllocator;	// コマンドアロケーター
private:
	HRESULT CreateTexture();				// 生成
	HRESULT CreateGraphicsPipelineState();		// グラフィックパイプラインの生成
	HRESULT CreateRootSignature();				// ルートシグネチャーの生成
	HRESULT CreateVertexIndexBufferView(MESH* mesh, float width, float height);// プレーン生成
	HRESULT CreateShaderResource(const char* pFileName, ComPtr<ID3D12Resource>& textureBuffer, SIZE& size);// シェーダーリソースの生成
	HRESULT CreateConstantBufferView(MESH* mesh);			// コンスタントバッファービューの生成

public:
	Texture();										// コンストラクター
	~Texture();									// デストラクター
	static void Create();					// 生成
	static void Destroy();					// 破棄
	void Draw();							// 描画
	// Texture
	void TextureDeleate(int& entryPos, const char* filename);
	void SetTextureMetaData(int& entryPos, VECTOR position, VECTOR angle, VECTOR scale, VECTOR color, float a, int drawOrder, bool uiFlag, const char* filename);
	void GetSize(const char* filename, float& width, float& height);

	static Texture* GetInstance() {
		return _instance;
	}
};

#endif	/* __TEXTURE_H__ */