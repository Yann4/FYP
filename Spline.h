#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <directxmath.h>
#include <vector>
#include "Camera.h"

class Spline
{
private:
	struct LineCBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 colour;
	};

	ID3D11DeviceContext* context;

	std::vector<DirectX::XMFLOAT3> controlPoints;
	std::vector<DirectX::XMFLOAT3> linePoints;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* constBuffer;
public:
	Spline();
	Spline(std::vector<DirectX::XMFLOAT3> controlPoints, ID3D11DeviceContext* context, ID3D11Device* device);

private:
	void generateLine();
	void createBuffers(ID3D11Device* device);

	DirectX::XMFLOAT3 lerp(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, float u);

public:
	void Draw(ID3D11PixelShader* pShader, ID3D11VertexShader* vShader, Camera& cam);
};
