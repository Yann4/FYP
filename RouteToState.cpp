#include "RouteToState.h"

using namespace DirectX;

RouteToState::RouteToState() : State()
{
	navGraph = nullptr;
	blackboard = nullptr;
	immediateStack = nullptr;
}

RouteToState::RouteToState(Controller* owner, std::stack<State*>* immediateStack, Graph* navGraph, Blackboard* blackboard, XMFLOAT3 destination) : State(owner),
	immediateStack(immediateStack), navGraph(navGraph), blackboard(blackboard), targetDestination(destination)
{
	getPath();
}

RouteToState::~RouteToState()
{
	immediateStack = nullptr;
	navGraph = nullptr;
	blackboard = nullptr;
}

void RouteToState::Update(double deltaTime, std::vector<BoundingBox>& objects)
{
	if (!haveValidPath(objects))
	{
		getPath();
	}
	else
	{
		if (atNextNode())
		{
			path.pop();

			navGraph->visitLocation(owner->position);

			if (!path.empty())
			{
				immediateStack->push(new TravelToPositionState(owner, path.top(), blackboard));
			}
		}
		else
		{
			getPath();
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

void RouteToState::getPath()
{
	if (!navGraph->isBusy())
	{
		path = navGraph->findPath(owner->position, targetDestination);

		navGraph->visitLocation(owner->position);

		path.push(owner->position);
	}
}

bool RouteToState::haveValidPath(std::vector<BoundingBox>& objects)
{
	//If we have objects in the path, and line of sight to the next position in the path, the path is valid
	//Also, check that we're not closer to the destination than the next position. Perform broad-phase check,
	//and if we are, recalculate and accept that path

	if (path.empty())
	{
		return false;
	}

	if (atNextNode())
	{
		return true;
	}

	XMFLOAT3 nextPos = path.top();
	XMVECTOR next = XMLoadFloat3(&nextPos);

	XMVECTOR ourPos = XMLoadFloat3(&owner->position);

	XMVECTOR toNode = next - ourPos;

	float distToNext = XMVectorGetX(XMVector3Length(toNode));
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		float dist;
		if (objects.at(i).Intersects(ourPos, XMVector3Normalize(toNode), dist) && dist < distToNext)
		{
			return false;
		}
	}

	if (XMVectorGetX(XMVector3Length(ourPos - XMLoadFloat3(&targetDestination))) < distToNext)
	{
		getPath();
	}

	return true;
}