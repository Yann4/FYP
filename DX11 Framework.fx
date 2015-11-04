//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;

	float4 DiffuseMtl;
	float4 DiffuseLight;

	float4 AmbientMtl;
	float4 AmbientLight;

	float4 SpecularMtl;
	float4 SpecularLight;
	float SpecularPower;
	float3 EyePosW;

	float3 LightVecW;
}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 PosW : POSITION;
	float3 Norm : NORMAL;
	float2 TexC : TEXCOORD;
};

Texture2D texDiffuse : register(t0);
Texture2D texSpec : register(t1);
Texture2D texNormMap : register(t2);

SamplerState samLinear: register(s0);

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION, float3 NormalL : NORMAL, float2 TexC : TEXCOORD )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul( Pos, World );

	//Get normalised vector to camera position in world coordinates
	output.PosW = normalize(EyePosW - output.Pos.xyz);

    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    
	//Getting normalised surface normal
	float3 normalW = mul(float4(NormalL, 0.0f), World).xyz;
	normalW = normalize(normalW);
	output.Norm = normalW;
	
	output.TexC = TexC;

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
	input.Norm = texNormMap.Sample(samLinear, input.TexC);//normalize(input.Norm);

	float4 texCol = texDiffuse.Sample(samLinear, input.TexC);

	//Calculate reflection vector
	float3 reflectVect = reflect(-LightVecW, input.Norm);

	//How much specular light makes it to the camera
	float specularAmount = pow(max(dot(reflectVect, input.PosW), 0.0f), texSpec.Sample(samLinear, input.TexC).r);

	//Calculate Specular light
	float3 specular = specularAmount * (SpecularMtl * SpecularLight).rgb;

	//Calculate Diffuse light
	float diffuseAmount = max(dot(LightVecW, input.Norm), 0.0f);
	float3 diffuse = diffuseAmount * (DiffuseMtl * DiffuseLight).rgb;

	//Calculate Ambient light
	float3 ambient = AmbientMtl * AmbientLight;

	float4 colour;
	colour.rgb = texCol * (diffuse + ambient) + specular;
	colour.a = DiffuseMtl.a;
	return colour;
}
