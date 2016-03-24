#include "ExploreState.h"

using namespace DirectX;

ExploreState::ExploreState(Controller* owner, std::stack<State*>* longTerm, std::stack<State*>* immediate, Graph* navGraph): State(owner), longTermStack(longTerm), immediateStack(immediate), navGraph(navGraph)
{
	destination = navGraph->getNearestUnvisitedLocation(owner->position);

	if (destination.x == owner->position.x && destination.z == owner->position.z)
	{
		graphFullyExplored = true;
	}
	else
	{
		graphFullyExplored = false;
	}

	pushedRoute = false;
}

ExploreState::~ExploreState()
{
	navGraph = nullptr;
	longTermStack = nullptr;
	immediateStack = nullptr;
}

void ExploreState::Update(double deltaTime, std::vector<BoundingBox>& objects)
{
	if (!pushedRoute && !graphFullyExplored)
	{
		longTermStack->push(new RouteToState(owner, immediateStack, navGraph, destination));
		
		stackSize = immediateStack->size();
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
	*toPush = new ExploreState(owner, longTermStack, immediateStack, navGraph);
	return LONG_TERM;
}