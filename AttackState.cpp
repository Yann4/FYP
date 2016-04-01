#include "AttackState.h"

using namespace Steering;
using namespace DirectX;

AttackState::AttackState(Controller* owner, Blackboard* blackboard) : State(owner), blackboard(blackboard)
{
	attackedPlayer = false;
}

AttackState::AttackState(const AttackState& other)
{
	owner = other.owner;
	blackboard = other.blackboard;
	attackedPlayer = other.attackedPlayer;
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
		attackedPlayer = true;
	}
	
	XMFLOAT3 arrive = arriveForce(owner->position, playerPos.info, 0.5f);
	XMFLOAT3 obstacleAvoid = obstacleAvoidForce(objects, owner->position, owner->facing);

	owner->force = aggregateForces(XMFLOAT3(0, 0, 0), arrive, obstacleAvoid);
}

Priority AttackState::shouldEnter()
{
	//If is not alone and can see the player && player hasn't been tagged
	if (owner->canSeePlayer && !blackboard->hasPlayerBeenTagged() && !blackboard->isAgentAlone(owner->agentID))
	{
		return IMMEDIATE;
	}

	return NONE;
}

bool AttackState::shouldExit()
{
	return attackedPlayer;
}

Priority AttackState::Exit(State** toPush)
{
	blackboard->tagPlayer();
	return NONE;
}