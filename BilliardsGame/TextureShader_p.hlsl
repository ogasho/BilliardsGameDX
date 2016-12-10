/* テクスチャシェーダ pixel */

Texture2D shaderTexture;
SamplerState SampleType;

struct PS_IN
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 TexturePixelShader(PS_IN input) : SV_TARGET
{
	float4 textureColor;
	
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	return textureColor;
}