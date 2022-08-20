#include "FieldHeader.hlsli"

Output FieldVS(
	float4 pos : POSITION,
	float4 color : COLOR)
{
	Output output;	// ピクセルシェーダーに渡す値
	output.svpos = mul(mul(projection, mul(view, world)), float4(pos.xyz, 1.0));

	output.color = color;

	return output;
}