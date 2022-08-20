#include "TextureHeader.hlsli"

Output TextureVS(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	Output output;	// �s�N�Z���V�F�[�_�[�ɓn���l
	output.svpos = mul(mat, float4(pos.xyz, 1.0));
	output.uv = uv;

	return output;
}