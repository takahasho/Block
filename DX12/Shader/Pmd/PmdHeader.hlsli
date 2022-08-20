// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
Texture2D<float4> tex : register(t0);	// 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`���[
Texture2D<float4> sph : register(t1);	// 1�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`���[
Texture2D<float4> spa : register(t2);	// 2�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`���[
SamplerState smp : register(s0);		// 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

// �萔�o�b�t�@�[0
// ���W
cbuffer cbuff0 : register(b0)
{
	matrix world;			// ���[���h�s��
	matrix view;			// �r���[�s��
	matrix Projection;		// �v���W�F�N�V�����s��
	float3 eye;				// ���_���W
	float4 LightVector;		// ���C�g����
};

// �萔�o�b�t�@�[1
// �}�e���A��
cbuffer AllPMDMaterial : register(b1)
{
	float4 diffuse;		// �f�B�t���[�Y�F
	float4 specular;	// �X�y�L�����[
	float3 ambient;		// �A���r�G���g
};

struct Output
{
	float4 svpos	: SV_POSITION;	// �V�X�e���p���_���W
	float4 pos		: POSITION;		// ���_���W
	float4 normal   : NORMAL0;		// �@���x�N�g��
	float4 vnormal	: NORMAL1;		// �r���[�ϊ���̖@���x�N�g��
	float2 uv		: TEXCOORD;		// UV�l
	float3 ray		: VECTOR;		// �����x�N�g��
};