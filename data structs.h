#pragma once

#include <directxmath.h>

struct SimpleVertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
};

struct ConstantBuffer
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;

	DirectX::XMFLOAT4 diffuseMtl;
	DirectX::XMFLOAT4 diffuseLight;
	DirectX::XMFLOAT3 lightVecW;
};

struct MeshData
{
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int numIndices;

	MeshData() : vertexBuffer(nullptr), indexBuffer(nullptr), numIndices(0)
	{}

	MeshData(ID3D11Buffer* vBuffer, ID3D11Buffer* iBuffer, int numIndices) : vertexBuffer(vBuffer), indexBuffer(iBuffer), numIndices(numIndices)
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
	}
};