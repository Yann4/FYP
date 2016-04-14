#pragma once

#include "State.h"
#include "RouteToState.h"

#include "Blackboard.h"
#include "Graph.h"
#include "Observer.h"

#include <DirectXMath.h>
#include <vector>
#include <stack>
#include <random>

class InvestigateState : public State, public Observer
{
private:
	Blackboard* blackboard;

	DirectX::XMFLOAT3 disturbanceLocation;
	bool disturbanceFound;

	float hearingRange = 5.0f;

	//For constructing the routeToState
	std::stack<State*>* immediate;
	Graph* navGraph;
public:
	InvestigateState();
	InvestigateState(Controller* owner, Blackboard* blackboard, std::stack<State*>* immediate, Graph* navGraph);
	~InvestigateState();
	
	void Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects);
	Priority shouldEnter();
	bool shouldExit();
	Priority Exit(State** toPush);

	void onNotify(const DirectX::XMFLOAT3& entity, ActionEvent e);
};