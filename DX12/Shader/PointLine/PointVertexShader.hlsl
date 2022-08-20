#include "PointLineShaderHeader.hlsli"

Output PointVS(float4 pos : POSITION, uint instNo : SV_InstanceID)
{
    Output output = (Output)0;  // �s�N�Z���V�F�[�_�[�ɓn���l
    output.svpos = mul(mul(projection, view), float4(startPos[instNo].xyz, 1.0));
    output.instNo = instNo;

    return output;
}