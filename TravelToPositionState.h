#pragma once

#include "State.h"
#include "Steering.h"

#include "Blackboard.h"
#include <DirectXMath.h>

class TravelToPositionState : public State
{
private:
	DirectX::XMFLOAT3 position;
	Blackboard* blackboard;
public:
	TravelToPositionState(Controller* owner, DirectX::XMFLOAT3 destination, Blackboard* blackboard);
	
	void Update (double deltaTime, std::vector<DirectX::BoundingBox>& objects);
	Priority shouldEnter();
	bool shouldExit();
	Priority Exit(State** toPush);

public:
	const float distThreshold = 0.5f;
};
