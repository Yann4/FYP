#include "GuardState.h"

using namespace DirectX;

GuardState::GuardState(Controller* owner, DirectX::XMFLOAT3 locationToGuard, Blackboard* blackboard) : 
	State(owner), locationToGuard(locationToGuard), blackboard(blackboard)
{
	currentDestination = 0;
}

GuardState::GuardState(const GuardState& other)
{
	locationToGuard = other.locationToGuard;
	path = other.path;
	blackboard = other.blackboard;
	owner = other.owner;
	currentDestination = other.currentDestination;
}

GuardState::~GuardState()
{
	blackboard = nullptr;
}

void GuardState::Update(double deltaTime, std::vector<BoundingBox>& objects)
{
	if (path.empty())
	{
		generatePatrol(objects);
		blackboard->flipAgentGuarding(owner->agentID);
	}

	if (atNextNode())
	{
		if (++currentDestination >= path.size())
		{
			currentDestination = 0;
		}
	}

	XMFLOAT3 arrive = Steering::arriveForce(owner->position, path.at(currentDestination), 0.5f);
	XMFLOAT3 avoid = Steering::obstacleAvoidForce(objects, owner->position, owner->facing);
	owner->force = Steering::aggregateForces(XMFLOAT3(0, 0, 0), arrive, avoid);
}

Priority GuardState::shouldEnter()
{
	//Shouldn't if exit is being guarded by 2 agents
	if (blackboard->numAgentsGuarding() >= 2)
	{
		return NONE;
	}
	else
	{
		//If we're already guarding it, we probably shouldn't double-guard it
		if (blackboard->isAgentGuarding(owner->agentID))
		{
			return NONE;
		}

		return IMMEDIATE;
	}
}

bool GuardState::shouldExit()
{
	return blackboard->numAgentsGuarding() > 2;
}

Priority GuardState::Exit(State** toPush)
{
	blackboard->flipAgentGuarding(owner->agentID);
	return NONE;
}

void GuardState::generatePatrol(std::vector<BoundingBox>& objects)
{
	const float patrolRadius = 2.0f;
	const unsigned int detail = 8;

	const float anglePerPoint = XM_2PI / 8;
	float currentAngle = 0.0f;
	XMVECTOR yAxis = XMVectorSet(0, 1, 0, 0);

	XMVECTOR centrePoint = XMLoadFloat3(&locationToGuard);
	XMFLOAT3 point;

	XMVECTOR pointOnSurface = XMVectorSet(1, 0, 0, 0);
	
	//Generate point on the surface of a circle
	//Then make sure it's not inside a box
	for (unsigned int i = 0; i < detail; i++)
	{
		float angle = currentAngle;//std::max(-1.0f, std::min(currentAngle * i, 1.0f));
		currentAngle += anglePerPoint;

		pointOnSurface = XMVectorSet(locationToGuard.x + (patrolRadius * cosf(angle)), locationToGuard.y, locationToGuard.z + (patrolRadius * sinf(angle)), 1.0f);
		XMStoreFloat3(&point, pointOnSurface);

		if (angle == XM_PI)
		{
			point.z = locationToGuard.z;
		}
		
		XMVECTOR fromCentre = XMLoadFloat3(&point) - centrePoint;
		fromCentre = XMVector3Normalize(fromCentre);

		for (unsigned int j = 0; j < objects.size(); j++)
		{
			float distance;
			if (objects.at(j).Intersects(centrePoint, fromCentre, distance) && distance < patrolRadius)
			{
				XMStoreFloat3(&point, centrePoint + (pointOnSurface * distance));
				break;
			}
		}

		path.push_back(point);
	}
}

bool GuardState::atNextNode()
{
	XMVECTOR currentPosition = XMLoadFloat3(&owner->position);
	XMVECTOR destination = XMLoadFloat3(&path.at(currentDestination));

	const float threshold = 0.5f;

	if (XMVectorGetX(XMVector3LengthEst(currentPosition - destination)) < threshold)
	{
		return true;
	}

	return false;
}