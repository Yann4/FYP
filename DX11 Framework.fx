//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
struct DirectionalLight
{
	float4 diffuse;
	float4 ambient;
	float4 specular;

	float3 direction;
	float pad;
};

struct PointLight
{
	float4 diffuse;
	float4 ambient;
	float4 specular;

	float3 position;
	float range;

	float3 attenuation;
	float pad;
};

struct SpotLight
{
	float4 diffuse;
	float4 ambient;
	float4 specular;

	float3 position;
	float range;

	float3 direction;
	float spot;

	float3 attenuation;
	float pad;
};

struct Material
{
	float4 diffuse;
	float4 ambient;
	float4 specular;
};

cbuffer frameCB : register(b0)
{
	DirectionalLight dirLight;
	PointLight pointLight;
	SpotLight spotLight;
	float3 eyePos;
	float pad;
};

cbuffer objectCB : register(b1)
{
	matrix World;
	matrix View;
	matrix Projection;
	Material material;
};

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

void calculateDirectionalLight(Material mat, DirectionalLight light, float3 normal, float3 toEye, 
	out float4 ambient, out float4 specular, out float4 diffuse)
{
	ambient = float4(0, 0, 0, 0);
	specular = float4(0, 0, 0, 0);
	diffuse = float4(0, 0, 0, 0);

	float3 lightV = -light.direction;

	ambient = mat.ambient * light.ambient;

	//If surface is in line of sight
	float diffuseFact = dot(lightV, normal);

	[flatten]
	if (diffuseFact > 0.0f)
	{
		float3 vect = reflect(-lightV, normal);
		float specularFact = pow(max(dot(vect, toEye), 0.0f), mat.specular.w);

		diffuse = diffuseFact * mat.diffuse * light.diffuse;
		specular = specularFact * mat.specular * light.specular;
	}
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION, float3 NormalL : NORMAL, float2 TexC : TEXCOORD )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul( Pos, World );

	//Get normalised vector to camera position in world coordinates
	output.PosW = normalize(eyePos - output.Pos.xyz);

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
	input.Norm = texNormMap.Sample(samLinear, input.TexC);

	float4 texCol = texDiffuse.Sample(samLinear, input.TexC);
	float4 specCol = texSpec.Sample(samLinear, input.TexC).r;

	float4 ambient = (0, 0, 0, 0);
	float4 specular = (0, 0, 0, 0);
	float4 diffuse = (0, 0, 0, 0);


	float4 amb, spec, diff;
	calculateDirectionalLight(material, dirLight, input.Norm, normalize(eyePos - input.PosW), amb, diff, spec);

	ambient += amb;
	specular += spec;
	diffuse += diff;

	/*
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

	*/

	float4 colour;
	colour = ambient + specular + diffuse;
	//colour.rgb = texCol * (diffuse + ambient) + specular;
	colour.a = material.diffuse.a;
	return colour;
}
