cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix Projection;
}

struct VS_IN
{
	float3 posL   : POSITION;
	float3 normalL : NORMAL;
	float2 texC : TEXCOORD;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float3 texC : TEXCOORD;
};

TextureCube SkyMap;
SamplerState ObjSamplerState;

VS_OUT SKYMAP_VS(float3 inPos : POSITION, float3 normal : NORMAL, float2 inTexCoord : TEXCOORD)
{
	VS_OUT output = (VS_OUT)0;

	//Set Pos to xyww instead of xyzw, so that z will always be 1 (furthest from camera)
	output.Pos = mul(float4(inPos, 1.0f), Projection).xyww;

	output.texC = inPos;

	return output;
}

float4 SKYMAP_PS(VS_OUT input) : SV_Target
{
	return SkyMap.Sample(ObjSamplerState, input.texC);
}