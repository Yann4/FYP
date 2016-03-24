#pragma once

#include "State.h"
#include "RouteToState.h"

#include "Graph.h"

#include <DirectXMath.h>
#include <stack>

class ExploreState : public State
{
private:
	Graph* navGraph;
	DirectX::XMFLOAT3 destination;

	std::stack<State*>* longTermStack;
	std::stack<State*>* immediateStack;

	unsigned int stackSize;

	bool graphFullyExplored;

	bool pushedRoute;
	const float distThreshold = 0.5f;
public:
	ExploreState(Controller* owner, std::stack<State*>* longTerm, std::stack<State*>* immediate, Graph* navGraph);
	~ExploreState();

	void Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects);
	Priority shouldEnter();
	bool shouldExit();
	Priority Exit(State** toPush);
};