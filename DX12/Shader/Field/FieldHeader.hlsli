// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct Output
{
	float4 svpos	: SV_POSITION;	// システム用頂点座標
	float4 color	: COLOR0;		// 色
};

cbuffer cbuff0 : register(b0)	// 定数バッファー
{
	matrix world;			// ワールド変換行列
	matrix view;			// ビュー行列
	matrix projection;		// プロジェクション行列
	float3 eye;				// 視点
};