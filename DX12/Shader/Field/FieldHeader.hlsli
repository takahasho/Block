// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct Output
{
	float4 svpos	: SV_POSITION;	// �V�X�e���p���_���W
	float4 color	: COLOR0;		// �F
};

cbuffer cbuff0 : register(b0)	// �萔�o�b�t�@�[
{
	matrix world;			// ���[���h�ϊ��s��
	matrix view;			// �r���[�s��
	matrix projection;		// �v���W�F�N�V�����s��
	float3 eye;				// ���_
};