/* テクスチャシェーダ vertex */

cbuffer MatrixBuffer
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;
};

struct VS_IN
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct PS_OUT
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PS_OUT TextureVertexShader(VS_IN input)
{
	PS_OUT output;
	
	input.position.w = 1.0f;

	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);

	output.tex = input.tex;

	return output;
}