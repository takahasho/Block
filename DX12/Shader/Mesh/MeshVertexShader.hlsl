#include "MeshHeader.hlsli"

Output MeshVS(
	float4 pos : POSITION,
	float4 normal : NORMAL,
	uint instNo : SV_InstanceID)
{
	Output output;	// �s�N�Z���V�F�[�_�[�ɓn���l
	output.svpos = mul(mul(projection, mul(view, world[instNo])), float4(pos.xyz, 1.0));
	output.pos = mul(world[instNo], pos);
	float4 nor;
	// �ړ����v�Z�ɔ��f�����Ȃ�
	normal.w = 0;
	// ���_�̖@���Ƀ��[���h�s����|�����킹��
	// ���[���h���W��ł̖@���̌����ɕϊ�����
	nor = mul(world[instNo], normal).xyzw;
	nor = normalize(nor);
	// dot => ���όv�Z
	float rad = saturate(dot(nor, LightVector));

	// ���C�g�̃J���[ * ���̂��������
	output.normal = LightColor * rad;

	output.ray = normalize(pos.xyz - eye);	// �����x�N�g��

	output.instNo = instNo;	// �C���X�^���X�ԍ�

	return output;
}
