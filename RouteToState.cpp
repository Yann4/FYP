#include "RouteToState.h"

using namespace DirectX;

RouteToState::RouteToState(Controller* owner, std::stack<State*>* immediateStack, Graph* navGraph, XMFLOAT3 destination) : State(owner),
	immediateStack(immediateStack), navGraph(navGraph), targetDestination(destination)
{
	while (navGraph->isBusy())
	{
		Sleep(2);
	}

	path = navGraph->findPath(owner->position, targetDestination);

	navGraph->visitLocation(owner->position);

	if (!path.empty())
	{
		immediateStack->push(new TravelToPositionState(owner, path.top()));
	}
}

RouteToState::~RouteToState()
{
	immediateStack = nullptr;
	navGraph = nullptr;
}

void RouteToState::Update(double deltaTime, std::vector<BoundingBox>& objects)
{
	if (atNextNode())
	{
		path.pop();
		
		navGraph->visitLocation(owner->position);

		if (!path.empty())
		{
			immediateStack->push(new TravelToPositionState(owner, path.top()));
		}
	}
}

Priority RouteToState::shouldEnter()
{
	return LONG_TERM;
}

bool RouteToState::shouldExit()
{
	return path.empty();
}

Priority RouteToState::Exit(State** toPush)
{
	navGraph->visitLocation(owner->position);
	return NONE;
}

bool RouteToState::atNextNode()
{
	if (path.empty())
	{
		return false;
	}

	XMVECTOR agentPos = XMLoadFloat3(&owner->position);
	XMVECTOR nextPos = XMLoadFloat3(&path.top());

	float distance = XMVectorGetX(XMVector3LengthEst(agentPos - nextPos));

	return distance < distThreshold;
}