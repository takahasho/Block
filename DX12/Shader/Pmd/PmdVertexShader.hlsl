#include "PmdHeader.hlsli"

Output PmdVS(
	float4 pos : POSITION,
	float4 normal : NORMAL,
	float2 uv : TEXCOORD,
	min16uint2 boneno : BONE_NO,
	min16uint weight : WEIGHT,
	min16uint edge_flg : EDGE_FLG)
{
	Output output;	// �s�N�Z���V�F�[�_�[�ɓn���l
	output.pos = mul(world, float4(pos.xyz, 1.0));
	output.svpos = mul(mul(Projection, view), output.pos);
	normal.w = 0;	// �������d�v
	output.normal = mul(world, normal);// �@���ɂ����[���h�ϊ����s��

	output.vnormal = mul(view, output.normal);// �@���ɂ����[���h�ϊ����s��

	output.uv = uv;

	output.ray = normalize(pos.xyz - eye);

	return output;
}
