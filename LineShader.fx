cbuffer lineCBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
};

struct VS_IN
{
	float3 pos : POSITION;
	float4 colour : COLOUR;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 colour : COLOUR;
};

VS_OUT lineVertexShader(float3 pos : POSITION, float4 colour : COLOUR)
{
	VS_OUT output;
	output.pos = mul(pos, World);
	output.pos = mul(output.pos, View);
	output.pos = mul(output.pos, Projection);
	output.pos.w = 0;
	output.colour = colour;

	return output;
}

float4 linePixelShader(VS_OUT input) : SV_TARGET0
{
	return input.colour;
}