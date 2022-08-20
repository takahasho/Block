#include "FieldHeader.hlsli"

float4 FieldPS(Output input) : SV_TARGET
{
	return input.color;
}