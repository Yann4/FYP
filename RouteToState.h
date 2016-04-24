#pragma once

#include "State.h"
#include "TravelToPositionState.h"

#include "Graph.h"

#include <stack>
#include <DirectXMath.h>

class RouteToState : public State
{
private:
	Graph* navGraph;
	Blackboard* blackboard;
	DirectX::XMFLOAT3 targetDestination;
	std::stack<DirectX::XMFLOAT3> path;

	std::stack<State*>* immediateStack;

	const float distThreshold = 0.5f;

public:
	RouteToState();
	RouteToState(Controller* owner, std::stack<State*>* immediateStack, Graph* navGraph, Blackboard* blackboard, DirectX::XMFLOAT3 destination);
	~RouteToState();

	void Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects);
	Priority shouldEnter();
	bool shouldExit();
	Priority Exit(State** toPush);

	const std::string StateName() { return "RouteTo"; }

private:
	bool atNextNode();
	void getPath();
	bool haveValidPath(std::vector<DirectX::BoundingBox>& objects);
};