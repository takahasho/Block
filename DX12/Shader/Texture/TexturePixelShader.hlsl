#include "TextureHeader.hlsli"

float4 TexturePS(Output input) : SV_TARGET
{
	// �A���r�G���g�F
	return float4(tex.Sample(smp, input.uv)) * color;
}