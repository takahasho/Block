#include "MeshHeader.hlsli"

float4 MeshPS(Output input) : SV_TARGET
{
	// 光の反射ベクトル
	float3 refLight = normalize(reflect(-LightVector.xyz, input.normal.xyz));
	float specularB = pow(saturate(dot(refLight, -input.ray)), specular[input.instNo].a);

	// 輝度*ディフューズ色
	float3 diffuseColor = (diffuse[input.instNo].xyz * input.normal.xyz);
	// アンビエント色
	float3 ambientColor = ambient[input.instNo].xyz;
	// スペキュラー色
	float3 specularColor = specularB * specular[input.instNo].rgb;

float4 color = LightColor * float4(
	diffuseColor			// 輝度*ディフューズ色
	+ ambientColor			// アンビエント色
	+ specularColor,		// スペキュラー色
	diffuse[input.instNo].a);				// α

	return color;
}