// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体

cbuffer cbuff0 : register(b0)	// 定数バッファー
{
	matrix mat;	// 変換行列
	float4 Color;	// 色
};

struct Output
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
};