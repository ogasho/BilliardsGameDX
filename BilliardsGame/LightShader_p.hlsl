/* ライトシェーダ pixel */

Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
};

struct PS_IN
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

float4 LightPixelShader(PS_IN input) : SV_TARGET
{
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;

	textureColor = shaderTexture.Sample(SampleType, input.tex);
	color = ambientColor;

	// ピクセルの光の量を計算
	lightDir = -lightDirection;
	lightIntensity = saturate(dot(input.normal, lightDir));

	// 光が当たっているならば
	if (lightIntensity > 0.0f)
	{
		// 拡散色を加える
		color += (diffuseColor * lightIntensity);
	}

	// 光色とテクスチャ色を結合
	color = saturate(color);
	color = color * textureColor;

	return color;
}