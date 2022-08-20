// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
Texture2D<float4> tex : register(t0);	// 0番スロットに設定されたテクスチャー
Texture2D<float4> sph : register(t1);	// 1番スロットに設定されたテクスチャー
Texture2D<float4> spa : register(t2);	// 2番スロットに設定されたテクスチャー
SamplerState smp : register(s0);		// 0番スロットに設定されたサンプラー

// 定数バッファー0
// 座標
cbuffer cbuff0 : register(b0)
{
	matrix world;			// ワールド行列
	matrix view;			// ビュー行列
	matrix Projection;		// プロジェクション行列
	float3 eye;				// 視点座標
	float4 LightVector;		// ライト方向
};

// 定数バッファー1
// マテリアル
cbuffer AllPMDMaterial : register(b1)
{
	float4 diffuse;		// ディフューズ色
	float4 specular;	// スペキュラー
	float3 ambient;		// アンビエント
};

struct Output
{
	float4 svpos	: SV_POSITION;	// システム用頂点座標
	float4 pos		: POSITION;		// 頂点座標
	float4 normal   : NORMAL0;		// 法線ベクトル
	float4 vnormal	: NORMAL1;		// ビュー変換後の法線ベクトル
	float2 uv		: TEXCOORD;		// UV値
	float3 ray		: VECTOR;		// 視線ベクトル
};