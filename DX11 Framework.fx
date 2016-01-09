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
	int useTextures;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 ToEye : TO_EYE_VECT;
	float3 PosW : POSITION_WORLD;
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

void calculatePointLight(Material mat, PointLight light, float3 position, float3 normal, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 specular)
{
	ambient = float4(0, 0, 0, 0);
	specular = float4(0, 0, 0, 0);
	diffuse = float4(0, 0, 0, 0);

	float3 lightV = light.position - position;

	float dist = length(lightV);

	if (dist > light.range)
	{
		return;
	}

	lightV /= dist;

	ambient = mat.ambient * light.ambient;

	float diffuseFact = dot(lightV, normal);

	[flatten]
	if (diffuseFact > 0.0f)
	{
		float3 vect = reflect(-lightV, normal);
		float specularFact = pow(max(dot(vect, toEye), 0.0f), mat.specular.w);

		diffuse = diffuseFact * mat.diffuse * light.diffuse;
		specular = specularFact * mat.specular * light.specular;
	}

	float attenuation = 1.0f / dot(light.attenuation, float3(1.0f, dist, dist*dist));

	diffuse *= attenuation;
	specular *= attenuation;
}

void calculateSpotLight(Material mat, SpotLight light, float3 position, float3 normal, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 specular)
{
	ambient = float4(0, 0, 0, 0);
	specular = float4(0, 0, 0, 0);
	diffuse = float4(0, 0, 0, 0);

	float3 lightV = light.position - position;

	float distance = length(lightV);

	if (distance > light.range)
	{
		return;
	}

	lightV /= distance;

	ambient = mat.ambient * light.ambient;

	float diffuseFact = dot(lightV, normal);

	[flatten]
	if (diffuseFact > 0.0f)
	{
		float3 vect = reflect(-lightV, normal);

		float specularFact = pow(max(dot(vect, toEye), 0.0f), mat.specular.w);

		diffuse = diffuseFact * mat.diffuse * light.diffuse;
		specular = specularFact * mat.specular * light.specular;
	}

	float spot = pow(max(dot(-lightV, float3(-light.direction.x, -light.direction.y, light.direction.z)), 0.0f), light.spot);

	float attenuation = spot / dot(light.attenuation, float3(1.0f, distance, distance*distance));

	ambient *= spot;
	diffuse *= attenuation;
	specular *= attenuation;
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION, float3 NormalL : NORMAL, float2 TexC : TEXCOORD )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul( Pos, World );
	output.PosW = output.Pos;

	//Get normalised vector to camera position in world coordinates
	output.ToEye = normalize(eyePos - output.Pos.xyz);

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
	//input.Norm = normalize(mul(input.Norm, (texNormMap.Sample(samLinear, input.TexC).rgb*2.0f - 1.0f)));
	input.Norm = normalize(input.Norm);
	float4 texCol = texDiffuse.Sample(samLinear, input.TexC);
	float4 specCol = texSpec.Sample(samLinear, input.TexC).r;

	Material newMat;
	if (useTextures == 1)
	{
		newMat.ambient = material.ambient;
		newMat.diffuse = texCol;
		newMat.specular = specCol;
	}
	else
	{
		newMat = material;
	}

	float4 ambient = (0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = (0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = (0.0f, 0.0f, 0.0f, 0.0f);

	float4 amb, spec, diff;
	calculateDirectionalLight(newMat, dirLight, input.Norm, input.ToEye, amb, diff, spec);

	ambient += amb;
	specular += spec;
	diffuse += diff;

	calculatePointLight(newMat, pointLight, input.PosW, input.Norm, normalize(input.ToEye), amb, diff, spec);

	ambient += amb;
	specular += spec;
	diffuse += diff;

	calculateSpotLight(newMat, spotLight, input.PosW, input.Norm, normalize(input.ToEye), amb, diff, spec);

	ambient += amb;
	specular += spec;
	diffuse += diff;

	float4 colour;
	colour = ambient + specular + diffuse;
	colour.a = material.diffuse.a;
	return colour;
}
