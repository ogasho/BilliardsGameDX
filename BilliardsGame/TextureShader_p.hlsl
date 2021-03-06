/* テクスチャシェーダ pixel */

Texture2D shaderTexture;
SamplerState SampleType;

cbuffer TextureShader
{
	float4 blendColor;
};

struct PS_IN
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 TexturePixelShader(PS_IN input) : SV_TARGET
{
	float4 color;

	color = shaderTexture.Sample(SampleType, input.tex);
	color = color * blendColor;

	return color;
}