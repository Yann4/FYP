#pragma once
#include "AgentController.h"
#include "Blackboard.h"

#include "State.h"
#include "TravelToPositionState.h"
#include "RouteToState.h"
#include "ExploreState.h"
#include "InvestigateState.h"
#include "StunnedState.h"
#include "HideState.h"

#include <stack>
#include <array>
#include <vector>

#include <DirectXCollision.h>

class Marpo
{
private:
	Controller* owner;
	Graph* navGraph;
	Blackboard* blackboard;

	std::stack<State*> longTerm;
	std::stack<State*> immediate;
	std::stack<State*> reactionary;

public:
	Marpo();

	void Initialise(Controller* owner, Graph* graph, Blackboard* bb);

	void Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects);

	inline bool initialised() { return owner != nullptr; }

private:
	void pushWithPriority(State* state, Priority prio);
	void checkForStatesToPush();
	std::stack<State*>* getTopStack();
};
