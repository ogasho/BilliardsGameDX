/* ライトシェーダ vertex */

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
	float3 normal : NORMAL;
};

struct PS_OUT
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

PS_OUT LightVertexShader(VS_IN input)
{
	PS_OUT output;
	
	input.position.w = 1.0f;

	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);

	output.tex = input.tex;

	output.normal = mul(input.normal, (float3x3)world);
	output.normal = normalize(output.normal);

	return output;
}