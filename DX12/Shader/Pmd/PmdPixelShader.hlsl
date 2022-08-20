#include "PmdHeader.hlsli"

float4 PmdPS(Output input) : SV_TARGET
{
	float3 light = LightVector.xyz;

	// ディフーズ計算
	float diffuseB = dot(light, input.normal.xyz);

	// 光の反射ベクトル
	float3 refLight = normalize(reflect(light, input.normal.xyz));
	float specularB = pow(saturate(dot(refLight, -input.ray)), specular.a);

	// スフィアマップのUV値
	float2 sphereMapUV = input.vnormal.xy;
	sphereMapUV = (sphereMapUV + float2(1, -1))
	* float2(0.5, -0.5);

	// テクスチャーカラー
	float4 texColor = tex.Sample(smp, input.uv);


return max(diffuseB // 輝度
	* diffuse	// ディフーズ色
	* texColor	// テクスチャーカラー
	* sph.Sample(smp, sphereMapUV)				// スフィアマップ(乗算)
	+ spa.Sample(smp, sphereMapUV) * texColor	// スフィアマップ(加算)
	+ float4(specularB * specular.rgb, 1)	// スペキュラ
	, float4(texColor.xyz * ambient, 1));	// アンビエント
}