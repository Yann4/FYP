#include "HideState.h"

using namespace DirectX;

HideState::HideState() : State()
{
	blackboard = nullptr;
	immediate = nullptr;
	navGraph = nullptr;
}

HideState::HideState(Controller* owner, Blackboard* blackboard, std::stack<State*>* immediate, Graph* navGraph) : State(owner), blackboard(blackboard), immediate(immediate), navGraph(navGraph)
{
	hidingSpotFound = false;
}

HideState::~HideState()
{
	blackboard = nullptr;
}

void HideState::Update(double deltaTime, std::vector<BoundingBox>& objects)
{
	//Find nearest box to hide behind
	XMVECTOR agentPos = XMLoadFloat3(&owner->position);

	XMFLOAT3 nearestBox;
	float nearestDist = D3D11_FLOAT32_MAX;
	float boxSize = 2.0f;

	for (unsigned int i = 0; i < objects.size(); i++)
	{
		XMVECTOR boxPos = XMLoadFloat3(&objects.at(i).Center);
		float dist = XMVectorGetX(XMVector3LengthEst(boxPos - agentPos));

		if (dist < nearestDist)
		{
			nearestDist = dist;
			nearestBox = objects.at(i).Center;
			boxSize = fmaxf(objects.at(i).Extents.x, objects.at(i).Extents.z);
		}
	}

	//Get vector from player to box (pb)
	XMFLOAT3 playerPos = blackboard->getPlayerPosition().info;
	XMVECTOR player = XMLoadFloat3(&playerPos);

	XMVECTOR boxPos = XMLoadFloat3(&nearestBox);

	XMVECTOR pb = XMVector3NormalizeEst(boxPos - player);

	//Position we want to go to is boxPos + pb
	XMVECTOR hidingSpot = boxPos + (pb * boxSize);
	XMStoreFloat3(&hidePosition, hidingSpot);
	hidingSpotFound = true;
	blackboard->setAgentScaredState(owner->agentID, true);
}

Priority HideState::shouldEnter()
{
	Data<XMFLOAT3> playerPos = blackboard->getPlayerPosition();
	
	if (playerPos.confidence > 70)
	{
		XMVECTOR pPos = XMLoadFloat3(&playerPos.info);
		XMVECTOR aPos = XMLoadFloat3(&owner->position);

		XMVECTOR ap = pPos - aPos;

		float angle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(ap), XMVector3Normalize(XMLoadFloat3(&owner->facing)))));

		if (angle >= XM_PIDIV2)
		{
			return NONE;
		}

		if (!blackboard->isAgentScared(owner->agentID) && XMVectorGetX(XMVector3LengthSq(ap)) < powf(panicRadius, 2.0f) && blackboard->isAgentAlone(owner->agentID))
		{
			return REACTIONARY;
		}
	}

	return NONE;
}

bool HideState::shouldExit()
{
	return hidingSpotFound;
}

Priority HideState::Exit(State** toPush)
{
	*toPush = new RouteToState(owner, immediate, navGraph, blackboard, hidePosition);
	return IMMEDIATE;
}