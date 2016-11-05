/* ���C�g�V�F�[�_ pixel */

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

	// �s�N�Z���̌��̗ʂ��v�Z
	lightDir = -lightDirection;
	lightIntensity = saturate(dot(input.normal, lightDir));

	// ���̒l���e�N�X�`���̃s�N�Z���l�ƌ���
	color = saturate(diffuseColor * lightIntensity);
	color = color * textureColor;

	return color;
}