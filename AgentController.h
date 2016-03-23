#pragma once

#include <DirectXMath.h>
class Controller
{
public:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 force;
	DirectX::XMFLOAT3 facing;

public:
	Controller() {};
	
	Controller(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 facing) : position(position), facing(facing)
	{
		force = DirectX::XMFLOAT3(0, 0, 0);
	}
};
