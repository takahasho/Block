// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����

cbuffer cbuff0 : register(b0)	// �萔�o�b�t�@�[
{
	matrix mat;	// �ϊ��s��
	float4 Color;	// �F
};

struct Output
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
};