#include "StunnedState.h"

StunnedState::StunnedState(Controller* owner, double stunDuration) : State(owner), stunDurationRemaining(stunDuration)
{}

void StunnedState::Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects)
{
	stunDurationRemaining -= deltaTime;
}

Priority StunnedState::shouldEnter()
{
	return REACTIONARY;
}

bool StunnedState::shouldExit()
{
	return stunDurationRemaining <= 0;
}

Priority StunnedState::Exit(State** toPush)
{
	return NONE;
}