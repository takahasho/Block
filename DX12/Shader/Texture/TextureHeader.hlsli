// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
Texture2D<float4> tex : register(t0);	// 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`���[
SamplerState smp : register(s0);		// 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[
#define instanceMax 500		// �C���X�^���X�̍ő�l
cbuffer cbuff0 : register(b0)	// �萔�o�b�t�@�[
{
	matrix mat;	// �ϊ��s��
	float4 color;	// �F
};

struct Output
{
	float4 svpos : SV_POSITION;	// �V�X�e���p���_���W
	float2 uv	 : TEXCOORD;	// UV�l
};