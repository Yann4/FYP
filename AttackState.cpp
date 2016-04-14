#include "AttackState.h"

using namespace Steering;
using namespace DirectX;

AttackState::AttackState() : State()
{
	blackboard = nullptr;
}

AttackState::AttackState(Controller* owner, Blackboard* blackboard) : State(owner), blackboard(blackboard)
{
}

AttackState::~AttackState()
{
	blackboard = nullptr;
}

void AttackState::Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects)
{
	Data<XMFLOAT3> playerPos = blackboard->getPlayerPosition();
	
	XMVECTOR player = XMLoadFloat3(&playerPos.info);
	XMVECTOR agent = XMLoadFloat3(&owner->position);
	
	if (XMVectorGetX(XMVector3LengthSq(player - agent)) < tagDistance)
	{
		blackboard->tagPlayer();
	}
	
	XMFLOAT3 playerPosition = playerPos.info;
	playerPosition.y = owner->position.y;

	XMFLOAT3 arrive = arriveForce(owner->position, playerPosition, 0.5f);
	XMFLOAT3 obstacleAvoid = obstacleAvoidForce(objects, owner->position, owner->facing);
	XMFLOAT3 separate = separationForce(owner->position, blackboard->agentPositions(owner->agentID));

	owner->force = aggregateForces(XMFLOAT3(0, 0, 0), arrive, obstacleAvoid, separate);
}

Priority AttackState::shouldEnter()
{
	if (owner->canSeePlayer && !blackboard->hasPlayerBeenTagged() && !blackboard->isAgentAlone(owner->agentID) && !owner->attackingPlayer)
	{
		owner->attackingPlayer = true;
		return IMMEDIATE;
	}

	return NONE;
}

bool AttackState::shouldExit()
{
	return blackboard->hasPlayerBeenTagged() || !owner->canSeePlayer;
}

Priority AttackState::Exit(State** toPush)
{
	owner->attackingPlayer = false;
	return NONE;
}