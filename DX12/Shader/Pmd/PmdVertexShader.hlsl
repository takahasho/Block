#include "PmdHeader.hlsli"

Output PmdVS(
	float4 pos : POSITION,
	float4 normal : NORMAL,
	float2 uv : TEXCOORD,
	min16uint2 boneno : BONE_NO,
	min16uint weight : WEIGHT,
	min16uint edge_flg : EDGE_FLG)
{
	Output output;	// ピクセルシェーダーに渡す値
	output.pos = mul(world, float4(pos.xyz, 1.0));
	output.svpos = mul(mul(Projection, view), output.pos);
	normal.w = 0;	// ここが重要
	output.normal = mul(world, normal);// 法線にもワールド変換を行う

	output.vnormal = mul(view, output.normal);// 法線にもワールド変換を行う

	output.uv = uv;

	output.ray = normalize(pos.xyz - eye);

	return output;
}
