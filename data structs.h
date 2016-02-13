#pragma once
#include <directxmath.h>
#include <string>
#include <vector>
#include "Light.h"

struct SimpleVertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexC;
};

struct Material
{
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 specular;

	std::string name;
	Material()
	{
		diffuse = DirectX::XMFLOAT4(0, 0, 0, 0);
		ambient = DirectX::XMFLOAT4(0, 0, 0, 0);
		specular = DirectX::XMFLOAT4(0, 0, 0, 0);
	}

	Material(DirectX::XMFLOAT4 spec, DirectX::XMFLOAT4 amb, DirectX::XMFLOAT4 diff) : specular(spec), ambient(amb), diffuse(diff) {}
};

struct CBMaterial
{
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 specular;

	CBMaterial()
	{
		diffuse = DirectX::XMFLOAT4(0, 0, 0, 0);
		ambient = DirectX::XMFLOAT4(0, 0, 0, 0);
		specular = DirectX::XMFLOAT4(0, 0, 0, 0);
	}

	CBMaterial(DirectX::XMFLOAT4 spec, DirectX::XMFLOAT4 amb, DirectX::XMFLOAT4 diff) : specular(spec), ambient(amb), diffuse(diff) {}
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

struct frameCB
{
	DirectionalLight dirLight;
	PointLight pointLight;
	SpotLight spotLight;

	DirectX::XMFLOAT3 eyePos;
	float pad;
};

struct objectCB
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;

	CBMaterial material;
	int useTextures = false;
};

struct MeshSection
{
	std::string materialName;
	std::string sectionName;
	Material* material;
	int startIndex, endIndex;
	DirectX::XMFLOAT3 centre;
	DirectX::XMFLOAT3 size;

	MeshSection() { materialName = ""; sectionName = ""; material = nullptr; startIndex = 0; endIndex = 0; centre = DirectX::XMFLOAT3(0, 0, 0); size = DirectX::XMFLOAT3(0, 0, 0); }
	MeshSection(std::string materialName, std::string sectionName, Material* material, int startIndex, int endIndex, DirectX::XMFLOAT3 centre, DirectX::XMFLOAT3 size) :
		materialName(materialName), sectionName(sectionName), material(material), startIndex(startIndex), endIndex(endIndex), centre(centre), size(size)
	{}
	~MeshSection(){ material = nullptr; }
};

//One instance of MeshData should be used per object type, and referenced in each gameobject
struct MeshData
{
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int numIndices;
	std::vector<MeshSection> parts;
	DirectX::XMFLOAT3 size;

	ID3D11ShaderResourceView* textureRV;
	ID3D11ShaderResourceView* specularRV;
	ID3D11ShaderResourceView* normalMapRV;
	Material material;

	MeshData() : vertexBuffer(nullptr), indexBuffer(nullptr), numIndices(0), material(Material()), textureRV(nullptr), specularRV(nullptr), normalMapRV(nullptr), parts(std::vector<MeshSection>()), size(DirectX::XMFLOAT3(0,0,0))
	{}

	MeshData(ID3D11Buffer* vBuffer, ID3D11Buffer* iBuffer, int numIndices, std::vector<MeshSection> parts, Material mat, ID3D11ShaderResourceView* texRV, ID3D11ShaderResourceView* specRV, ID3D11ShaderResourceView* normalRV) : 
		vertexBuffer(vBuffer), indexBuffer(iBuffer), numIndices(numIndices), material(mat), parts(parts), textureRV(texRV), specularRV(specRV), normalMapRV(normalRV)
	{
		vertexBuffer->AddRef();
		indexBuffer->AddRef();
		if (texRV) texRV->AddRef();
		if (specRV) specRV->AddRef();
		if (normalMapRV) normalMapRV->AddRef();
	}

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

		if (normalMapRV)
		{
			normalMapRV->Release();
		}
	}
};