#include "PointLineShaderHeader.hlsli"

float4 LineVS(float4 pos : POSITION, uint instNo : SV_InstanceID) : SV_POSITION
{
    float4 output = (float)0;  // �W�I���g���[�V�F�[�_�[�ɓn���l
    output.xyz = pos.xyz;
    output.w = instNo;

    return output;
}