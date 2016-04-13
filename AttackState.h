#pragma once

#include "State.h"

#include "Blackboard.h"
#include "Steering.h"
#include <DirectXMath.h>

class AttackState : public State
{
private:
	Blackboard* blackboard;
	const float tagDistance = 0.5f;

public:
	AttackState();
	AttackState(Controller* owner, Blackboard* blackboard);

	~AttackState();

	void Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects);
	Priority shouldEnter();
	bool shouldExit();
	Priority Exit(State** toPush);
};