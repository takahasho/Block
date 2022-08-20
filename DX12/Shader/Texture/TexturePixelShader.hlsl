#include "TextureHeader.hlsli"

float4 TexturePS(Output input) : SV_TARGET
{
	// アンビエント色
	return float4(tex.Sample(smp, input.uv)) * color;
}