#include "MeshHeader.hlsli"

float4 MeshPS(Output input) : SV_TARGET
{
	// ���̔��˃x�N�g��
	float3 refLight = normalize(reflect(-LightVector.xyz, input.normal.xyz));
	float specularB = pow(saturate(dot(refLight, -input.ray)), specular[input.instNo].a);

	// �P�x*�f�B�t���[�Y�F
	float3 diffuseColor = (diffuse[input.instNo].xyz * input.normal.xyz);
	// �A���r�G���g�F
	float3 ambientColor = ambient[input.instNo].xyz;
	// �X�y�L�����[�F
	float3 specularColor = specularB * specular[input.instNo].rgb;

float4 color = LightColor * float4(
	diffuseColor			// �P�x*�f�B�t���[�Y�F
	+ ambientColor			// �A���r�G���g�F
	+ specularColor,		// �X�y�L�����[�F
	diffuse[input.instNo].a);				// ��

	return color;
}