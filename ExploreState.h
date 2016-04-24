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
	Blackboard* blackboard;
	DirectX::XMFLOAT3 destination;

	std::stack<State*>* longTermStack;
	std::stack<State*>* immediateStack;

	const float distThreshold = 0.5f;

	bool invalidDestination;

public:
	ExploreState();
	ExploreState(Controller* owner, std::stack<State*>* longTerm, std::stack<State*>* immediate, Graph* navGraph, Blackboard* blackboard);
	~ExploreState();

	void Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects);
	Priority shouldEnter();
	bool shouldExit();
	Priority Exit(State** toPush);

	const std::string StateName() { return "Explore"; }
private:
	bool atDestNode();
	void setDestNode();
};