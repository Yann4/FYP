#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <directxmath.h>
#include <vector>
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
	
	DirectX::XMFLOAT4 colour;

	std::vector<DirectX::XMFLOAT3> controlPoints;
	std::vector<DirectX::XMFLOAT3> linePoints;

	//Each spline holds own buffers because they are all unique buffers
	//Or rather; no point in having two identical splines in the same location
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* constBuffer;

	int NUM_POINTS = 101;
public:
	Spline();
	Spline(std::vector<DirectX::XMFLOAT3> controlPoints, ID3D11DeviceContext* context, ID3D11Device* device, ID3D11InputLayout* layout);
	~Spline();

private:
	void generateLine();
	void createBuffers(ID3D11Device* device);

	DirectX::XMFLOAT3 lerp(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, float u);

public:
	void Draw(ID3D11PixelShader* pShader, ID3D11VertexShader* vShader, Camera& cam, bool partOfSplineBatch);

	inline std::vector<DirectX::XMFLOAT3> getLinePoints() { return linePoints; }
	inline void changeColour(DirectX::XMFLOAT4 colour)
	{
		Spline::colour = colour;
		createBuffers(device);
	}
};
