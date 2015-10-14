#pragma once
#include <directxmath.h>

struct SimpleVertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexC;
};

struct ConstantBuffer
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;

	DirectX::XMFLOAT4 diffuseMtl;
	DirectX::XMFLOAT4 diffuseLight;

	DirectX::XMFLOAT4 ambientMtl;
	DirectX::XMFLOAT4 ambientLight;

	DirectX::XMFLOAT4 specularMtl;
	DirectX::XMFLOAT4 specularLight;
	float			  specularPower;
	DirectX::XMFLOAT3 cameraPosW;

	DirectX::XMFLOAT3 lightVecW;
};

struct Material
{
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 specular;

	Material()
	{
		diffuse = DirectX::XMFLOAT4(0, 0, 0, 0);
		ambient = DirectX::XMFLOAT4(0, 0, 0, 0);
		specular = DirectX::XMFLOAT4(0, 0, 0, 0);
	}

	Material(DirectX::XMFLOAT4 spec, DirectX::XMFLOAT4 amb, DirectX::XMFLOAT4 diff) : specular(spec), ambient(amb), diffuse(diff) {}
};

struct Light
{
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 specular;
	float			  specularPower;
	DirectX::XMFLOAT3 lightVecW;

	Light()
	{
		diffuse = DirectX::XMFLOAT4(0, 0, 0, 0);
		ambient = DirectX::XMFLOAT4(0, 0, 0, 0);
		specular = DirectX::XMFLOAT4(0, 0, 0, 0);
		lightVecW = DirectX::XMFLOAT3(0, 0, 0);
		specularPower = 0;
	}

	Light(DirectX::XMFLOAT4 spec, DirectX::XMFLOAT4 amb, DirectX::XMFLOAT4 diff, float specPow, DirectX::XMFLOAT3 lightVW) : specular(spec), ambient(amb), diffuse(diff), specularPower(specPow), lightVecW(lightVW) {}
};

//One instance of MeshData should be used per object type, and referenced in each gameobject
struct MeshData
{
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int numIndices;

	ID3D11ShaderResourceView* textureRV;
	ID3D11ShaderResourceView* specularRV;
	Material material;

	MeshData() : vertexBuffer(nullptr), indexBuffer(nullptr), numIndices(0), material(Material()), textureRV(nullptr), specularRV(nullptr)
	{}

	MeshData(ID3D11Buffer* vBuffer, ID3D11Buffer* iBuffer, int numIndices, Material mat, ID3D11ShaderResourceView* texRV, ID3D11ShaderResourceView* specRV) : 
		vertexBuffer(vBuffer), indexBuffer(iBuffer), numIndices(numIndices), material(mat), textureRV(texRV), specularRV(specRV)
	{}

	~MeshData()
	{
		if (vertexBuffer)
		{
			vertexBuffer->Release();
		}

		if (indexBuffer)
		{
			indexBuffer->Release();
		}

		if (textureRV)
		{
			textureRV->Release();
		}

		if (specularRV)
		{
			specularRV->Release();
		}
	}
};