#include "TravelToPositionState.h"

using namespace DirectX;
using namespace Steering;

TravelToPositionState::TravelToPositionState() : State()
{
	blackboard = nullptr;
}

TravelToPositionState::TravelToPositionState(Controller* owner, XMFLOAT3 position, Blackboard* blackboard) : State(owner), position(position), blackboard(blackboard)
{
	invalidPath = false;
}

TravelToPositionState::~TravelToPositionState()
{
	blackboard = nullptr;
}

void TravelToPositionState::Update(double deltaTime, std::vector<BoundingBox>& objects)
{
	if (!validTarget(objects))
	{
		invalidPath = true;
	}

	XMFLOAT3 arrive = arriveForce(owner->position, position, 0.5f);
	XMFLOAT3 oa = obstacleAvoidForce(objects, position, owner->facing);
	XMFLOAT3 separate = separationForce(owner->position, blackboard->agentPositions(owner->agentID));

	XMFLOAT3 force = aggregateForces(XMFLOAT3(0, 0, 0), arrive, oa, separate);
	owner->force = force;
}

Priority TravelToPositionState::shouldEnter()
{
	return IMMEDIATE;
}

bool TravelToPositionState::shouldExit()
{
	if (invalidPath)
	{
		return true;
	}

	XMVECTOR pos = XMLoadFloat3(&owner->position);

	XMVECTOR dest = XMLoadFloat3(&position);

	float distance = XMVectorGetX(XMVector3LengthEst(dest - pos));

	if (distance < distThreshold)
	{
		return true;
	}

	return false;
}

Priority TravelToPositionState::Exit(State** toPush)
{
	return NONE;
}

bool TravelToPositionState::validTarget(std::vector<DirectX::BoundingBox>& objects)
{
	XMVECTOR next = XMLoadFloat3(&position);

	XMVECTOR ourPos = XMLoadFloat3(&owner->position);

	XMVECTOR toNode = next - ourPos;

	float distToNext = XMVectorGetX(toNode);
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		float dist;
		if (objects.at(i).Intersects(ourPos, XMVector3Normalize(toNode), dist) && dist < distToNext)
		{
			return false;
		}
	}

	if (distToNext > 10.0f)
	{
		return false;
	}

	return true;
}