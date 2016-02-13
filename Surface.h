#pragma once

#include <windows.h>
#include <d3d11_1.h>

#include "Spline.h"
#include "Camera.h"

#include <vector>
#include <array>

class Surface
{
private:
	ID3D11DeviceContext* context;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* constBuffer;

	UINT indexCount;

public:
	Surface();
	Surface(std::vector<Spline>& splines, ID3D11DeviceContext* context, ID3D11Device* device);
	~Surface();

	void Draw(ID3D11PixelShader* pShader, ID3D11VertexShader* vShader, Camera& cam);
};
