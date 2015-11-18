#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <directxmath.h>

namespace Collision
{
	struct AABB
	{
		DirectX::XMFLOAT3 centre;
		DirectX::XMFLOAT3 size;

		AABB()
		{
			centre = DirectX::XMFLOAT3(0, 0, 0);
			size = DirectX::XMFLOAT3(0, 0, 0);
		}

		AABB(DirectX::XMFLOAT3 centre, DirectX::XMFLOAT3 size):
			centre(centre), size(size)
		{}
	};

	DirectX::XMFLOAT3 boundingBoxCollision(const AABB& a, const AABB& b)
	{
		bool collision = false;
		DirectX::XMFLOAT3 mtv = DirectX::XMFLOAT3(0, 0, 0);

		//Right side of a on the left of b
		if (a.centre.x + a.size.x > b.centre.x - b.size.x)
		{
			//Left side of a on the right of b
			if (a.centre.x - a.size.x < b.centre.x + b.size.x)
			{
				//Top of a below bottom of b
				if (a.centre.y + a.size.y > b.centre.y - b.size.y)
				{
					//Bottom of a above top of b
				}
			}
		}
	}
};