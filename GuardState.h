#pragma once

#include "State.h"
#include "RouteToState.h"

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <vector>
#include <stack>
#include "Blackboard.h"
#include "Graph.h"

class GuardState : public State
{
private:
	DirectX::XMFLOAT3 locationToGuard;
	std::vector<DirectX::XMFLOAT3> path;

	unsigned int currentDestination;

	Blackboard* blackboard;
	Graph* graph;

	std::stack<State*>* immediate;

public:
	GuardState();
	GuardState(Controller* owner, DirectX::XMFLOAT3 locationToGuard, Blackboard* blackboard, Graph* graph, std::stack<State*>* immediate);
	~GuardState();

	void Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects);
	Priority shouldEnter();
	bool shouldExit();
	Priority Exit(State** toPush);

	const std::string StateName() { return "Guard"; }
private:
	void generatePatrol(std::vector<DirectX::BoundingBox>& objects);
	bool atNextNode();
	unsigned int nearestNodeOnPath();
};
