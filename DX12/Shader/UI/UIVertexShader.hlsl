#include "UIHeader.hlsli"

Output UIVS(float4 pos : POSITION)
{
	Output output;	// ピクセルシェーダーに渡す値
	output.pos = mul(mat, float4(pos.xyz, 1.0));
	output.col = Color;

	return output;
}