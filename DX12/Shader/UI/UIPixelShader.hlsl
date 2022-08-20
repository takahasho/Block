#include "UIHeader.hlsli"

float4 UIPS(Output input) : SV_TARGET
{
	return input.col;
}