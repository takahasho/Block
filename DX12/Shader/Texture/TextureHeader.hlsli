// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
Texture2D<float4> tex : register(t0);	// 0番スロットに設定されたテクスチャー
SamplerState smp : register(s0);		// 0番スロットに設定されたサンプラー
#define instanceMax 500		// インスタンスの最大値
cbuffer cbuff0 : register(b0)	// 定数バッファー
{
	matrix mat;	// 変換行列
	float4 color;	// 色
};

struct Output
{
	float4 svpos : SV_POSITION;	// システム用頂点座標
	float2 uv	 : TEXCOORD;	// UV値
};