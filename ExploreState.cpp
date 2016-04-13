#include "ExploreState.h"

using namespace DirectX;

ExploreState::ExploreState() : State()
{
	navGraph = nullptr;
	blackboard = nullptr;
	longTermStack = nullptr;
	immediateStack = nullptr;
	destination = XMFLOAT3(0, -999, 0);
}

ExploreState::ExploreState(Controller* owner, std::stack<State*>* longTerm, std::stack<State*>* immediate, Graph* navGraph, Blackboard* blackboard):
	State(owner), longTermStack(longTerm), immediateStack(immediate), navGraph(navGraph), blackboard(blackboard), destination(XMFLOAT3())
{
	destination = XMFLOAT3(0, -999, 0);
	invalidDestination = true;
}

ExploreState::~ExploreState()
{
	navGraph = nullptr;
	longTermStack = nullptr;
	immediateStack = nullptr;
}

void ExploreState::Update(double deltaTime, std::vector<BoundingBox>& objects)
{
	if (navGraph->fullyVisited() && !navGraph->isBusy())
	{
		navGraph->setGraphUnvisited();
		setDestNode();
	}
	
	if (invalidDestination)
	{
		setDestNode();
		return;
	}

	if (!atDestNode())
	{
		immediateStack->push(new RouteToState(owner, immediateStack, navGraph, blackboard, destination));
	}
	else
	{
		setDestNode();
	}
}

Priority ExploreState::shouldEnter()
{
	if (longTermStack->empty())
	{
		return LONG_TERM;
	}

	return NONE;
}

bool ExploreState::shouldExit()
{
	return false;
}

Priority ExploreState::Exit(State** toPush)
{
	return NONE;
}

bool ExploreState::atDestNode()
{

	XMVECTOR agentPos = XMLoadFloat3(&owner->position);
	XMVECTOR nextPos = XMLoadFloat3(&destination);

	float distance = XMVectorGetX(XMVector3LengthEst(agentPos - nextPos));

	if (distance < distThreshold)
	{
		blackboard->explored(owner->agentID);
		return true;
	}

	return false;
}

void ExploreState::setDestNode()
{
	if (navGraph->isBusy())
	{
		invalidDestination = true;
		return;
	}

	
	auto dests = navGraph->getUnvisitedLocations();

	bool set = false;
	for (unsigned int i = 0; i < dests.size(); i++ ) 
	{
		if (blackboard->shouldExplore(dests.at(i)))
		{
			blackboard->setExploring(dests.at(i), owner->agentID);
			destination = dests.at(i);
			invalidDestination = false;
			set = true;
		}
	}

	if (!set)
	{
		invalidDestination = true;
	}
}