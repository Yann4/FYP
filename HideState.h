#pragma once

#include "State.h"
#include "RouteToState.h"

#include "Blackboard.h"
#include "Graph.h"
#include <DirectXMath.h>
#include <stack>

class HideState : public State
{
private:
	DirectX::XMFLOAT3 hidePosition;
	bool hidingSpotFound;
	Blackboard* blackboard;

	const float panicRadius = 2.0f;

	//For constructing the routeToState
	std::stack<State*>* immediate;
	Graph* navGraph;

public:
	HideState(Controller* owner, Blackboard* blackboard, std::stack<State*>* immediate, Graph* navGraph);
	HideState(const HideState& other);
	~HideState();

	void Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects);
	Priority shouldEnter();
	bool shouldExit();
	Priority Exit(State** toPush);
};
