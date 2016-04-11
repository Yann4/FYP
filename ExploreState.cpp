#include "ExploreState.h"

using namespace DirectX;

ExploreState::ExploreState(Controller* owner, std::stack<State*>* longTerm, std::stack<State*>* immediate, Graph* navGraph, Blackboard* blackboard):
	State(owner), longTermStack(longTerm), immediateStack(immediate), navGraph(navGraph), blackboard(blackboard)
{
	destination = navGraph->getNearestUnvisitedLocation(owner->position);

	if (destination.x == owner->position.x && destination.z == owner->position.z)
	{
		graphFullyExplored = true;
		if (!navGraph->isBusy())
		{
			navGraph->setGraphUnvisited();
		}
	}
	else
	{
		graphFullyExplored = false;
	}

	pushedRoute = false;
}

ExploreState::ExploreState(const ExploreState& other)
{
	owner = other.owner;
	navGraph = other.navGraph;
	blackboard = other.blackboard;
	destination = other.destination;

	longTermStack = other.longTermStack;
	immediateStack = other.immediateStack;
	stackSize = other.stackSize;
	graphFullyExplored = other.graphFullyExplored;
	pushedRoute = other.pushedRoute;
}

ExploreState::~ExploreState()
{
	navGraph = nullptr;
	longTermStack = nullptr;
	immediateStack = nullptr;
}

void ExploreState::Update(double deltaTime, std::vector<BoundingBox>& objects)
{
	if (graphFullyExplored && !navGraph->isBusy())
	{
		navGraph->setGraphUnvisited();
		destination = navGraph->getNearestUnvisitedLocation(owner->position);
	}
	else if (!pushedRoute && !graphFullyExplored)
	{
		immediateStack->push(new RouteToState(owner, immediateStack, navGraph, blackboard, destination));
		
		stackSize = immediateStack->size() + 1;
		pushedRoute = true;
	}
}

Priority ExploreState::shouldEnter()
{
	if (longTermStack->empty() && !graphFullyExplored)
	{
		return LONG_TERM;
	}

	return NONE;
}

bool ExploreState::shouldExit()
{
	return immediateStack->size() < stackSize;
}

Priority ExploreState::Exit(State** toPush)
{
	*toPush = new ExploreState(owner, longTermStack, immediateStack, navGraph, blackboard);
	return LONG_TERM;
}