#include "State.h"

State::State()
{
}

State::State(Controller* owner) : owner(owner)
{
}

State::~State()
{
	
}

void State::Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects)
{}

Priority State::shouldEnter()
{
	return NONE;
}

bool State::shouldExit()
{
	return true;
}

Priority State::Exit(State** toPush)
{
	return NONE;
}