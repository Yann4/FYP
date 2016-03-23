#pragma once
#include "AgentController.h"

#include "State.h"
#include "TravelToPositionState.h"

#include <stack>
#include <array>
#include <vector>

#include <DirectXCollision.h>
class Marpo
{
private:
	Controller* owner;

	std::stack<State*> longTerm;
	std::stack<State*> immediate;
	std::stack<State*> reactionary;

	std::array<State*, 0> states;

public:
	Marpo();
	void Initialise(Controller* owner);

	void Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects);

	inline bool initialised() { if (owner == nullptr) { return false; } return true; }

private:
	void pushWithPriority(State* state, Priority prio);
	void checkForStatesToPush();
	std::stack<State*>* getTopStack();
};
