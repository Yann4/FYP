#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <directxmath.h>
#include <array>

namespace Collision
{
	struct AABB
	{
		DirectX::XMFLOAT3 centre;
		DirectX::XMFLOAT3 size;
		DirectX::XMFLOAT3 rotation;

		AABB()
		{
			centre = DirectX::XMFLOAT3(0, 0, 0);
			size = DirectX::XMFLOAT3(0, 0, 0);
			rotation = DirectX::XMFLOAT3(0, 0, 0);
		}

		AABB(DirectX::XMFLOAT3 centre, DirectX::XMFLOAT3 size, DirectX::XMFLOAT3 rotation = DirectX::XMFLOAT3(0, 0, 0)):
			centre(centre), size(size), rotation(rotation)
		{}
	};

	struct MTV
	{
		DirectX::XMFLOAT3 axis;
		float magnitude;
		MTV() : axis(DirectX::XMFLOAT3(0, 0, 0)), magnitude(0){};
		MTV(DirectX::XMFLOAT3 axis, float mag) : axis(axis), magnitude(mag){};
	};

	bool boundingBoxCollision(const AABB& a, const AABB& b, MTV& minTranslationVector);
	bool testAxis(DirectX::XMFLOAT3 axis, float minA, float maxA, float minB, float maxB, MTV& mtv);
};