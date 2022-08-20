#include "PmdHeader.hlsli"

float4 PmdPS(Output input) : SV_TARGET
{
	float3 light = LightVector.xyz;

	// �f�B�t�[�Y�v�Z
	float diffuseB = dot(light, input.normal.xyz);

	// ���̔��˃x�N�g��
	float3 refLight = normalize(reflect(light, input.normal.xyz));
	float specularB = pow(saturate(dot(refLight, -input.ray)), specular.a);

	// �X�t�B�A�}�b�v��UV�l
	float2 sphereMapUV = input.vnormal.xy;
	sphereMapUV = (sphereMapUV + float2(1, -1))
	* float2(0.5, -0.5);

	// �e�N�X�`���[�J���[
	float4 texColor = tex.Sample(smp, input.uv);


return max(diffuseB // �P�x
	* diffuse	// �f�B�t�[�Y�F
	* texColor	// �e�N�X�`���[�J���[
	* sph.Sample(smp, sphereMapUV)				// �X�t�B�A�}�b�v(��Z)
	+ spa.Sample(smp, sphereMapUV) * texColor	// �X�t�B�A�}�b�v(���Z)
	+ float4(specularB * specular.rgb, 1)	// �X�y�L����
	, float4(texColor.xyz * ambient, 1));	// �A���r�G���g
}