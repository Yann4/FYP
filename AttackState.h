#pragma once

#include "State.h"

#include "Blackboard.h"
#include "Steering.h"
#include <DirectXMath.h>

class AttackState : public State
{
private:
	Blackboard* blackboard;
	bool attackedPlayer;
	const float tagDistance = 0.5f;

public:
	AttackState(Controller* owner, Blackboard* blackboard);
	AttackState(const AttackState& other);
	~AttackState();

	void Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects);
	Priority shouldEnter();
	bool shouldExit();
	Priority Exit(State** toPush);
};