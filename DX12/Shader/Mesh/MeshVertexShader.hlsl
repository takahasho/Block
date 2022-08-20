#include "MeshHeader.hlsli"

Output MeshVS(
	float4 pos : POSITION,
	float4 normal : NORMAL,
	uint instNo : SV_InstanceID)
{
	Output output;	// ピクセルシェーダーに渡す値
	output.svpos = mul(mul(projection, mul(view, world[instNo])), float4(pos.xyz, 1.0));
	output.pos = mul(world[instNo], pos);
	float4 nor;
	// 移動が計算に反映させない
	normal.w = 0;
	// 頂点の法線にワールド行列を掛け合わせて
	// ワールド座標上での法線の向きに変換する
	nor = mul(world[instNo], normal).xyzw;
	nor = normalize(nor);
	// dot => 内積計算
	float rad = saturate(dot(nor, LightVector));

	// ライトのカラー * 光のあたり加減
	output.normal = LightColor * rad;

	output.ray = normalize(pos.xyz - eye);	// 視線ベクトル

	output.instNo = instNo;	// インスタンス番号

	return output;
}
