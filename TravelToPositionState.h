#pragma once

#include "State.h"
#include "Steering.h"

#include <DirectXMath.h>

class TravelToPositionState : public State
{
private:
	DirectX::XMFLOAT3 position;
	const float distThreshold = 0.5f;
public:
	TravelToPositionState(Controller* owner, DirectX::XMFLOAT3 destination);
	
	void Update (double deltaTime, std::vector<DirectX::BoundingBox>& objects);
	Priority shouldEnter();
	bool shouldExit();
	Priority Exit(State* toPush);
};
