#pragma once

#include "State.h"
#include "TravelToPositionState.h"

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <vector>
#include <stack>
#include "Blackboard.h"

class GuardState : public State
{
private:
	DirectX::XMFLOAT3 locationToGuard;
	std::vector<DirectX::XMFLOAT3> path;

	unsigned int currentDestination;

	Blackboard* blackboard;

public:
	GuardState(Controller* owner, DirectX::XMFLOAT3 locationToGuard, Blackboard* blackboard);
	GuardState(const GuardState& other);
	~GuardState();

	void Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects);
	Priority shouldEnter();
	bool shouldExit();
	Priority Exit(State** toPush);

private:
	void generatePatrol(std::vector<DirectX::BoundingBox>& objects);
	bool atNextNode();
};
