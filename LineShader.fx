cbuffer lineCBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
};

struct VS_IN
{
	float4 position : POSITION;
	float4 colour : COLOUR;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 colour : COLOUR;
};

VS_OUT lineVertexShader(float4 position : POSITION, float4 colour : COLOUR)
{
	VS_OUT output = (VS_OUT)0;

	output.pos = mul(position, World);
	output.pos = mul(output.pos, View);
	output.pos = mul(output.pos, Projection);
	output.colour = colour;

	return output;
}

float4 linePixelShader(VS_OUT input) : SV_TARGET
{
	return input.colour;
}