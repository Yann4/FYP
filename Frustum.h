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

	void constructFrustum(float screenDepth, float fov, float ar, float znear, float zfar, const DirectX::XMMATRIX& viewMat);
	DirectX::XMMATRIX calculateProjection(float fovY, float aspectRatio, float znear, float zfar);

	bool checkSphere(DirectX::XMFLOAT3 centre, float radius);
};