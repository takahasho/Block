#include "PointLineShaderHeader.hlsli"

float4 PointLinePS(Output input) : SV_TARGET
{
	return color[input.instNo];
}