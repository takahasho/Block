// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����

#define instanceMax 1000			// �C���X�^���X�̍ő�l

cbuffer cbuff0 : register(b0)	// �萔�o�b�t�@�[
{
	matrix view;				// �r���[�s��
	matrix projection;			// �v���W�F�N�V�����s��
	float4 startPos[instanceMax];	// �n�_
	float4 endPos[instanceMax];		// �I�_
	float4 color[instanceMax];		// �F
};

struct Output
{
	float4 svpos : SV_POSITION;	// �V�X�e���p���_���W
	uint instNo : SV_InstanceID;// �C���X�^���X
};