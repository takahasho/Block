#ifndef __PMDMODEL_H__
#define __PMDMODEL_H__
#pragma once
// --------------------- Include File --------------------
#include "BaseClass.h"
// ---------------------- 変数宣言 ---------------------
constexpr size_t _pmdVertex_size = 38;					// 頂点１つあたりのサイズ
// ---------------------- 構造体宣言 --------------------
// ======== PMDモデル ========
#pragma pack(1)	// ここから１バイトパッキングとなり、アライメントは発生しない
// PMDマテリアル構造体
struct PMDMaterial
{
	XMFLOAT3 diffuse;		// ディフューズ色
	FLOAT alpha;			// ディフューズα
	FLOAT specularity;		// スペキュラーの強さ(乗算値)
	XMFLOAT3 specular;		// スペキュラー色
	XMFLOAT3 ambient;		// アンビエント色
	unsigned char toonIdx;	// トゥーン番号(後述)
	unsigned char edgeFlg;	// マテリアルごとの輪郭線フラグ

	// ２バイトのパディングが発生しない

	unsigned int indicesNum;// このマテリアルが割り当てられる
							// インデックス数(後述)
	char texFilePath[20];	// テクスチャーファイルパス＋α(後述)
};	// パディングが発生しないため70バイト

#pragma pack()	// パッキング指定を解除(デフォルトに戻す)

// シェーダー側に投げられるマテリアルデータ
struct MaterialForHlsl
{
	XMFLOAT3 diffuse;		// ディフューズ色
	FLOAT alpha;			// ディフューズα
	XMFLOAT3 specular;		// スペキュラー色
	FLOAT specularity;		// スペキュラーの強さ(乗算値)
	XMFLOAT3 ambient;		// アンビエント色
};

// シェーダー側以外のマテリアルデータ
struct AdditionalMaterial
{
	std::string texPath;	// テクスチャーファイルパス
	int toonIdx;			// トゥーン番号
	bool edgeFlg;			// マテリアルごとの輪郭線フラグ
	AdditionalMaterial()
	{
		toonIdx = 0;
		edgeFlg = false;
	}
};

// 全体をまとまるデータ
struct AllPMDMaterial
{
	unsigned int indicesNum;// インデックス数
	MaterialForHlsl material;
	AdditionalMaterial additional;
	AllPMDMaterial()
	{
		indicesNum = 0;
		material = {};
	}
};

// MESH情報
typedef struct _PmdMesh_
{
	ComPtr<ID3D12Resource> vertexBuffer;			// 頂点バッファー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;		// 頂点バッファービュー

	ComPtr<ID3D12Resource> indexBuffer;				// インデックスバッファー
	D3D12_INDEX_BUFFER_VIEW indexBufferView;	    // インデックスバッファービュー

	ComPtr<ID3D12Resource> materialBuffer;					// マテリアルバッファー

	std::vector<ComPtr<ID3D12Resource>> texBuffer;			// テクスチャーバッファー
	std::vector<ComPtr<ID3D12Resource>> sphTexBuffer;		// テクスチャーバッファー(乗算スフィア)
	std::vector<ComPtr<ID3D12Resource>> spaTexBuffer;		// テクスチャーバッファー(加算スフィア)
	std::vector<ComPtr<ID3D12Resource>> toonTexBuffer;		// トゥーンテクスチャーバッファー

	ComPtr<ID3D12DescriptorHeap> basicDescHeap;	// 基本デスクリプタヒープ
	ComPtr<ID3D12Resource> constBuffer;			// コンスタントバッファー

	// ----- pmd読み込み情報 ------
	unsigned int verticesNum = 0;							// 頂点数
	std::vector<unsigned char> vertices;					// 頂点情報

	unsigned int indicesNum = 0;							// インデックス数
	std::vector<unsigned short> indices;					// インデックス情報

	unsigned int materialNum = 0;							// マテリアル数
	std::vector<PMDMaterial> pmdMaterials;					// マテリアル情報
	std::vector<AllPMDMaterial> materials;						// マテリアル情報
	// ----------------------------
	_PmdMesh_()
	{
		vertexBufferView = {};
		indexBufferView = {};
	}
}PMDMESH;

// ゲーム内の位置などの情報
typedef struct _PMDModel_
{
	PMDMESH mesh;					// メッシュ
	XMFLOAT3 position;				// 座標
	XMFLOAT3 angle;					// アングル
	XMFLOAT3 size;					// 大きさ

	_PMDModel_()
	{
		mesh = PMDMESH();
		position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		angle = XMFLOAT3(0.0f, 0.0f, 0.0f);
		size = XMFLOAT3(1.0f, 1.0f, 1.0f);
	}
}PMDMODEL;
// ---------------------- クラス宣言 ---------------------
class PmdModel : public BaseClass
{
private:
	// ---------------- 構造体宣言 ----------------
	// // PMDヘッダー
	struct PMDHeader
	{
		float version;			// 例:00 00 80 3F == 1.00
		char model_name[20];	// モデル名
		char comment[256];		// モデルコメント
	};

	// PMD頂点構造体
	struct PMDVertex
	{
		XMFLOAT3 pos;			// 頂点座標:12バイト
		XMFLOAT3 normal;		// 法線ベクトル:12バイト
		XMFLOAT2 uv;			// uv座標:8バイト
		unsigned short boneNo[2];// ボーン番号(後述):4バイト
		unsigned char boneWeight;// ボーン影響度(後述):1バイト
		unsigned char edgeFlg;	// 輪郭線フラグ:1バイト
	};	// 合計38バイト
	// シェーダー側に渡すための基本的な行列データ
	struct SceneMatrix
	{
		XMMATRIX world;			// ワールド行列
		XMMATRIX view;			// ビュー行列
		XMMATRIX Projection;    // プロジェクション行列
		XMFLOAT3 eye;			// 視点座標
		XMFLOAT4 LightVector;	// ライト方向
	};
private:
	// ---------------- メンバ変数宣言 ----------------
	static PmdModel* _instance;				// シングルトン
	std::map<std::string, ComPtr<ID3D12Resource>*>	_resourceTable;
private:

	HRESULT CreateGraphicsPipelineState();		// グラフィックパイプラインの生成
	HRESULT CreateRootSignature();				// ルートシグネチャーの生成

	HRESULT LoadPMD(PMDMODEL& model, const char* file_name);			// pmdファイルの読み込み
	HRESULT CreateVertexIndexBufferView(PMDMODEL& model);		// 頂点インデックスバッファービューの生成
	HRESULT CreateTexture(PMDMODEL& model);			// テクスチャの生成
	HRESULT CreateConstantBufferView(PMDMODEL& model);			// コンスタントバッファービューの生成

	HRESULT CreateShaderResource(
		ComPtr<ID3D12Resource>* texBuffer,
		const std::string fileName);			// シェーダーリソースの生成

	HRESULT CreateMaterialBuffer(PMDMODEL& model, const char* file_name);	// マテリアルバッファーの生成

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

	// ファイル名から拡張子を取得
	std::string GetExtension(const std::string& path)
	{
		int idx = (int)path.rfind('.');
		return path.substr((size_t)idx + 1, path.length() - idx - 1);
	}
	// テクスチャーのセパレーター文字で分離する
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
	PmdModel();										// コンストラクター
	~PmdModel();									// デストラクター
	static void Create();					// 生成
	static void Destroy();					// 破棄
	HRESULT CreatePMD(PMDMODEL& model, const char* file_name);		// 生成
	void Draw(PMDMODEL model);					// 描画
	static PmdModel* GetInstance() {
		return _instance;
	}
};

#endif	/* __PMDMODEL_H__ */