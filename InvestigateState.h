#pragma once

#include "State.h"
#include "RouteToState.h"

#include "Blackboard.h"
#include "Graph.h"

#include <DirectXMath.h>
#include <vector>
#include <stack>
#include <random>

class InvestigateState : public State
{
private:
	Blackboard* blackboard;

	DirectX::XMFLOAT3 disturbanceLocation;
	bool disturbanceFound;

	const float hearingRange = 5.0f;

	//For constructing the routeToState
	std::stack<State*>* immediate;
	Graph* navGraph;
public:
	InvestigateState(Controller* owner, Blackboard* blackboard, std::stack<State*>* immediate, Graph* navGraph);
	InvestigateState(const InvestigateState& other);
	~InvestigateState();

	void Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects);
	Priority shouldEnter();
	bool shouldExit();
	Priority Exit(State** toPush);

private:
	void getDisturbance();
};