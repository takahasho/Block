// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体

#define instanceMax 1000			// インスタンスの最大値

cbuffer cbuff0 : register(b0)	// 定数バッファー
{
	matrix view;				// ビュー行列
	matrix projection;			// プロジェクション行列
	float4 startPos[instanceMax];	// 始点
	float4 endPos[instanceMax];		// 終点
	float4 color[instanceMax];		// 色
};

struct Output
{
	float4 svpos : SV_POSITION;	// システム用頂点座標
	uint instNo : SV_InstanceID;// インスタンス
};