#include "PointLineShaderHeader.hlsli"

float4 LineVS(float4 pos : POSITION, uint instNo : SV_InstanceID) : SV_POSITION
{
    float4 output = (float)0;  // ジオメトリーシェーダーに渡す値
    output.xyz = pos.xyz;
    output.w = instNo;

    return output;
}