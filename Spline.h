#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <directxmath.h>
#include <vector>

class Spline
{
private:
	ID3D11DeviceContext* context;

	std::vector<DirectX::XMFLOAT3> controlPoints;
	std::vector<DirectX::XMFLOAT3> linePoints;

public:
	Spline();
	Spline(std::vector<DirectX::XMFLOAT3> controlPoints, ID3D11DeviceContext* context);

private:
	void generateLine();
};
