// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����

cbuffer cbuff0 : register(b0)	// �萔�o�b�t�@�[
{
	matrix view;			// �r���[�s��
	matrix projection;		// �v���W�F�N�V�����s��
	float4	LightVector;		// ���C�g����
	float4	LightColor;			// ���C�g�J���[
	float3 eye;				// ���_
};

#define instanceMax 500		// �C���X�^���X�̍ő�l
// �萔�o�b�t�@�[�P
// �`����W+�}�e���A���p
cbuffer Info : register(b1)
{
	matrix world[instanceMax];		// ���[���h�ϊ��s��
	float4 diffuse[instanceMax];	// �f�B�t���[�Y�F
	float4 specular[instanceMax];	// �X�y�L�����[
	float4 ambient[instanceMax];	// �A���r�G���g
};

struct Output
{
	float4 svpos	: SV_POSITION;	// �V�X�e���p���_���W
	float4 pos		: POSITION;		// ���_���W
	float4 normal	: NORMAL0;		// �@���x�N�g��
	float3 ray		: VECTOR;		// �x�N�g��
	uint instNo		: SV_InstanceID;// �C���X�^���X�ԍ�
};