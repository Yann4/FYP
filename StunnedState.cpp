#include "StunnedState.h"

StunnedState::StunnedState() : State()
{
}

StunnedState::StunnedState(Controller* owner, double stunDuration) : State(owner), stunDurationRemaining(stunDuration)
{
	
}

void StunnedState::Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects)
{
	owner->force = DirectX::XMFLOAT3(0, 0, 0);
	stunDurationRemaining -= deltaTime;
}

Priority StunnedState::shouldEnter()
{
	if (owner->getStunned && !owner->isStunned)
	{
		owner->isStunned = true;
		return REACTIONARY;
	}
	return NONE;
}

bool StunnedState::shouldExit()
{
	return stunDurationRemaining <= 0;
}

Priority StunnedState::Exit(State** toPush)
{
	owner->getStunned = false;
	owner->getStunned = false;
	return NONE;
}