/* ライトシェーダ pixel */

Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightShader
{
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

	// ピクセルの光の量を計算
	lightDir = -lightDirection;
	lightIntensity = saturate(dot(input.normal, lightDir));

	// 光の値をテクスチャのピクセル値と結合
	color = saturate(diffuseColor * lightIntensity);
	color = color * textureColor;

	return color;
}