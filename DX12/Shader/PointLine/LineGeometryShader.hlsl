#include "PointLineShaderHeader.hlsli"

[maxvertexcount(2)]
void LineGS(
	line float4 input[2] : SV_POSITION, 
	inout LineStream<Output> output
)
{
	Output element = (Output)0;

	uint instNo = (uint)input[0].w;
	element.svpos = mul(mul(projection, view), float4(startPos[instNo].xyz, 1.0));
	element.instNo = instNo;
	output.Append(element);
	element.svpos = mul(mul(projection, view), float4(endPos[instNo].xyz, 1.0));
	element.instNo = instNo;
	output.Append(element);
	output.RestartStrip();
}