#pragma once

#include <Windows.h>
#include <d3d11_1.h>
#include <directxmath.h>

#include <array>

class Frustum
{
private:
	std::array<DirectX::XMFLOAT4, 6> planes;

public:
	Frustum();
	Frustum(const Frustum&);

	void constructFrustum(float screenDepth, const DirectX::XMMATRIX& projection, const DirectX::XMMATRIX& view);

	bool checkSphere(DirectX::XMFLOAT3 centre, float radius);
};