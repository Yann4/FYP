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
	bool invalidPath;
public:
	TravelToPositionState();
	TravelToPositionState(Controller* owner, DirectX::XMFLOAT3 destination, Blackboard* blackboard);
	~TravelToPositionState();
	
	void Update (double deltaTime, std::vector<DirectX::BoundingBox>& objects);
	Priority shouldEnter();
	bool shouldExit();
	Priority Exit(State** toPush);

	const std::string StateName() { return "TravelToPosition"; }

private:
	bool validTarget(std::vector<DirectX::BoundingBox>& objects);
public:
	const float distThreshold = 0.5f;

};
