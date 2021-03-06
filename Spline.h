#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <directxmath.h>
#include <vector>
#include <array>
#include "Camera.h"
#include "data structs.h"

struct LineCBuffer
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
};

struct Vertex
{
	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT4 colour;
};

class Spline
{
private:
	ID3D11DeviceContext* context;
	ID3D11InputLayout* splineLayout;
	ID3D11Device* device;
	
	std::array<DirectX::XMFLOAT3, 101> linePoints;

	DirectX::XMFLOAT4 colour;

	//Each spline holds own buffers because they are all unique buffers
	//Or rather; no point in having two identical splines in the same location
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* constBuffer;

	const int NUM_POINTS = 101;
public:
	Spline();
	Spline(std::vector<DirectX::XMFLOAT3> controlPoints, ID3D11DeviceContext* context, ID3D11Device* device, ID3D11InputLayout* layout, DirectX::XMFLOAT4 colour = DirectX::XMFLOAT4(0,0,0,1));
	Spline(const Spline& other);
	~Spline();

private:
	void generateLine(const std::vector<DirectX::XMFLOAT3>& controlPoints);
	void createBuffers();

	DirectX::XMFLOAT3 lerp(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, float u);

public:
	void Draw(ID3D11PixelShader* pShader, ID3D11VertexShader* vShader, Camera& cam, bool partOfSplineBatch);

	inline std::array<DirectX::XMFLOAT3, 101> getLinePoints() { return linePoints; }
	inline void changeColour(DirectX::XMFLOAT4 newColour)
	{
		colour = newColour;
		createBuffers();
	}

	Spline& operator=(Spline other)
	{
		std::swap(context, other.context);
		context->AddRef();

		std::swap(splineLayout, other.splineLayout);
		splineLayout->AddRef();

		std::swap(device, other.device);
		device->AddRef();

		std::swap(colour, other.colour);
		std::swap(linePoints, other.linePoints);

		std::swap(vertexBuffer, other.vertexBuffer);
		vertexBuffer->AddRef();

		std::swap(indexBuffer, other.indexBuffer);
		indexBuffer->AddRef();

		std::swap(constBuffer, other.constBuffer);
		constBuffer->AddRef();

		return *this;
	}
};
