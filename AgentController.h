#pragma once

#include <DirectXMath.h>
class Controller
{
public:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 force;
	DirectX::XMFLOAT3 facing;
	unsigned int agentID;
	bool canSeePlayer;
	bool getStunned;
	bool isStunned;
	bool attackingPlayer;
public:
	Controller() {};
	
	Controller(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 facing, unsigned int agentID) : position(position), facing(facing), agentID(agentID)
	{
		force = DirectX::XMFLOAT3(0, 0, 0);
		canSeePlayer = false;
		getStunned = false;
		isStunned = false;
		attackingPlayer = false;
	}
};
