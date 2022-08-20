#ifndef OBJMODEL_H_
#define OBJMODEL_H_
#pragma once

// --------------------- Include File --------------------
#include "BaseClass.h"
#include "../VECTOR.h"
// ---------------------- クラス宣言 ---------------------
class ObjModel : public BaseClass
{
	static const UINT instanceMax = 500;					// インスタンスの最大値(シェーダーと同値)
private:
	// ===== 構造体宣言 =====
	typedef struct _VERTICES_
	{
		XMFLOAT4 position;				// 座標
		XMFLOAT4 normal;				// 法線
		XMFLOAT2 textureUV;				// テクスチャーUV値
		_VERTICES_()
		{
			position = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			normal = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			textureUV = XMFLOAT2(0.0f, 0.0f);
		}
	}VERTICES;


	typedef struct _tagMaterial_	// マテリアル情報
	{
		TCHAR materialName[MAX_PATH];// newmtl:マテリアル名
		FLOAT Ns;		// スペキュラ指数(0から1000)
		FLOAT dissolve;	// ディゾルブ(1.0 - 透過量)
		FLOAT Tr;		// 発光色
		FLOAT Ni;		// 屈折率(1のとき光が曲がらない)
		int illum;		// 照明モデル(2:ハイライト 有効)
		XMFLOAT4 Ka;	// Ka:アンビエント
		XMFLOAT4 Kd;	// Kd:ディフューズ
		XMFLOAT4 Ks;	// Ks:スペキュラー
		TCHAR textureNameKa[MAX_PATH];	// map_Ka:アンビエントテクスチャーマップ
		TCHAR textureNameKd[MAX_PATH];	// map_Kd:ディフューズテクスチャーマップ
		TCHAR textureNameKs[MAX_PATH];	// map_Ks:スペキュラカラーテクスチャーマップ
		TCHAR textureNameNs[MAX_PATH];	// map_Ns:スペキュラハイライト成分
		UINT textureNumberKd;			// ディフューズテクスチャー番号(その他は割愛)
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

	typedef struct _tagAttribute_	// 属性情報
	{
		// --- 情報展開 ---
		TCHAR materialName[MAX_PATH];// usemtl:マテリアル名
		UINT triangleStartIndex;	// インデックスのオフセット
		UINT quadrangleStartIndex;	// インデックスのオフセット
		UINT triangleIndexNum;		// インデックスの数
		UINT quadrangleIndexNum;	// インデックスの数
		// --- 描画関連 ---
		UINT verticesNum;			// 描画頂点数
		UINT materialNumber;		// マテリアル番号
		_tagAttribute_()
		{
			materialName[0] = '\0';
			triangleStartIndex = quadrangleStartIndex = triangleIndexNum = quadrangleIndexNum = 0;
			verticesNum = materialNumber = 0;
		}
	}ATTRIBUTE;

	typedef struct _tagTexture_		// テクスチャー情報
	{
		TCHAR filename[MAX_PATH];	// ファイル名
		ComPtr<ID3D12Resource> textureBuffer;					// テクスチャーバッファー
		_tagTexture_()
		{
			filename[0] = '\0';
		}
	}TEXTURE;

	// 付帯情報
	struct MetaData
	{
		XMMATRIX world[instanceMax];	// モデル本体を回転させたり移動させたりする行列
		XMFLOAT4 diffuse[instanceMax];		// ディフューズ色 + α
		XMFLOAT4 specular[instanceMax];		// スペキュラー色 + 強さ(乗算値)
		XMFLOAT4 ambient[instanceMax];		// アンビエント色
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
	// ---------------- メンバ変数宣言 ----------------
	static ObjModel* _instance;				// シングルトン
	// ===== OBJ関連 =====
	std::vector<XMFLOAT3>	_position;	// 頂点座標
	std::vector<XMFLOAT3>	_normal;	// 法線
	std::vector<XMFLOAT2>	_textureUV;	// テクスチャーUV値

	std::vector<UINT>		_triangleIndex;		// 三角形インデックス
	std::vector<UINT>		_quadrangleIndex;	// 四角形インデックス

	std::vector<ATTRIBUTE>	_attribute;	// 属性情報
	std::vector<MATERIAL>	_material;	// マテリアル情報		

	std::map<const char*, std::vector<ATTRIBUTE>>	_saveAttribute;	// 属性情報保存
	std::map<const char*, std::vector<MATERIAL>>	_saveMaterial;	// マテリアル情報保存

	std::map<const char*, std::vector<VECTOR>>	_saveVertices;		// 頂点情報保存
	std::vector<TEXTURE>	_fileTextures;			// テクスチャー(重複登録防止)

	BOOL					_attributeEntryFlag;	// 属性情報登録の有無
	BOOL					_materialEntryFlag;		// マテリアル情報登録の有無

	TCHAR* _path;						// パス(フォルダー)

	std::map<const char*, std::vector<MESH*>>  _objMeshs;	// メッシュ（objModel）

	std::map<const char*, std::vector<std::vector<MetaData>>> _objMetaData;			// メタ情報(objModel)

	std::map<const char*, UINT> _objInstanceEntry;		// インスタンスの数(objModel)

	std::map<const char*, std::vector<DataCheck>> _objDataCheck;		// エントリー番号が使われているかどうか
	// ===== 描画関連 =====

	ComPtr<ID3D12Device> _dev;				// デバイス
	ComPtr<ID3D12GraphicsCommandList> _cmdList;	// コマンドリスト

private:

	HRESULT CreateObjModel();				// 生成
	HRESULT CreateRootSignature();			// ルートシグネチャーの生成
	HRESULT CreatePipelineState();			// 描画パイプラインステートの生成
	HRESULT CreateMaterialBuffer(MESH* mesh);			// マテリアルバッファーの生成
	HRESULT CreateShaderResource(const TCHAR* pFileName,
		ComPtr<ID3D12Resource>& textureBuffer);	// シェーダーリソースの生成
	HRESULT CreateConstantBufferView(MESH* mesh);		// コンスタントバッファービューの生成
	BOOL LoadObjFromFile(const char* filename);
	// OBJファイルの読み込み
	BOOL LoadMaterialFromFile(const TCHAR* filename);
	// マテリアルの読み込み
	HRESULT Convert(MESH* mesh, std::vector<VECTOR>& saveVertex);			// 変換
public:
	ObjModel();								// コンストラクター
	~ObjModel();								// デストラクター
	static void Create();					// 生成
	static void Destroy();					// 破棄
	void Draw();							// 描画

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