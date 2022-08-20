// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
Texture2D<float4> tex : register(t0);	// 0番スロットに設定されたテクスチャー
SamplerState smp : register(s0);		// 0番スロットに設定されたサンプラー
cbuffer cbuff0 : register(b0)	// 定数バッファー
{
	matrix view;			// ビュー行列
	matrix projection;		// プロジェクション行列
	float4	LightVector;		// ライト方向
	float4	LightColor;			// ライトカラー
	float3 eye;				// 視点
};
#define instanceMax 500		// インスタンスの最大値
// 定数バッファー１
// 描画座標+マテリアル用
cbuffer Info : register(b1)
{
	matrix world[instanceMax];		// ワールド変換行列
	float4 diffuse[instanceMax];	// ディフューズ色
	float4 specular[instanceMax];	// スペキュラー
	float4 ambient[instanceMax];	// アンビエント
};

struct Output
{
	float4 svpos	: SV_POSITION;	// システム用頂点座標
	float4 pos		: POSITION;		// 頂点座標
	float4 normal : NORMAL;			// 法線ベクトル
	float2 uv  : TEXCOORD;			// UV値
	float3 ray		: VECTOR;		// ベクトル
	uint instNo		: SV_InstanceID;// インスタンス番号
};