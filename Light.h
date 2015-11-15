#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <directxmath.h>

struct DirectionalLight
{
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 specular;
	
	DirectX::XMFLOAT3 lightVecW;
	float pad;

	DirectionalLight()
	{
		diffuse = DirectX::XMFLOAT4(0, 0, 0, 0);
		ambient = DirectX::XMFLOAT4(0, 0, 0, 0);
		specular = DirectX::XMFLOAT4(0, 0, 0, 0);

		lightVecW = DirectX::XMFLOAT3(0, 0, 0);
		pad = 0;
	}

	DirectionalLight(DirectX::XMFLOAT4 spec, DirectX::XMFLOAT4 amb, DirectX::XMFLOAT4 diff, DirectX::XMFLOAT3 lightVW):
		diffuse(diff), ambient(amb), specular(spec), lightVecW(lightVW), pad(0)
	{

	}

};

struct PointLight
{

	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 specular;

	DirectX::XMFLOAT3 position;
	float range;
	DirectX::XMFLOAT3 attenuation;
	float pad;

	PointLight()
	{
		diffuse = DirectX::XMFLOAT4(0, 0, 0, 0);
		ambient = DirectX::XMFLOAT4(0, 0, 0, 0);
		specular = DirectX::XMFLOAT4(0, 0, 0, 0);

		position = DirectX::XMFLOAT3(0, 0, 0);
		range = 0;
		attenuation = DirectX::XMFLOAT3(0, 0, 0);
		pad = 0;
	}

	PointLight(DirectX::XMFLOAT4 spec, DirectX::XMFLOAT4 amb, DirectX::XMFLOAT4 diff, DirectX::XMFLOAT3 position, float range, DirectX::XMFLOAT3 att) :
		diffuse(diff), ambient(amb), specular(spec), position(position), range(range), attenuation(att), pad(0)
	{
	}
};

struct SpotLight
{

	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 specular;

	DirectX::XMFLOAT3 position;
	float range;

	DirectX::XMFLOAT3 direction;
	float spot;

	DirectX::XMFLOAT3 attenuation;
	float pad;

	SpotLight()
	{
		diffuse = DirectX::XMFLOAT4(0, 0, 0, 0);
		ambient = DirectX::XMFLOAT4(0, 0, 0, 0);
		specular = DirectX::XMFLOAT4(0, 0, 0, 0);

		position = DirectX::XMFLOAT3(0, 0, 0);
		range = 0;

		direction = DirectX::XMFLOAT3(0, 0, 0);
		spot = 0;

		attenuation = DirectX::XMFLOAT3(0, 0, 0);
		pad = 0;
	}

	SpotLight(DirectX::XMFLOAT4 spec, DirectX::XMFLOAT4 amb, DirectX::XMFLOAT4 diff, DirectX::XMFLOAT3 position, float range, DirectX::XMFLOAT3 att, DirectX::XMFLOAT3 direction, float spot) :
		diffuse(diff), ambient(amb), specular(spec), position(position), range(range), attenuation(att), direction(direction), spot(spot), pad(0)
	{
	}
};