#include "UIHeader.hlsli"

Output UIVS(float4 pos : POSITION)
{
	Output output;	// �s�N�Z���V�F�[�_�[�ɓn���l
	output.pos = mul(mat, float4(pos.xyz, 1.0));
	output.col = Color;

	return output;
}