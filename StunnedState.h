#pragma once

#include "State.h"
#include <DirectXMath.h>

class StunnedState : public State
{
private:
	double stunDurationRemaining;

public:
	StunnedState(Controller* owner, double stunDuration);

	void Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects);
	Priority shouldEnter();
	bool shouldExit();
	Priority Exit(State** toPush);
};