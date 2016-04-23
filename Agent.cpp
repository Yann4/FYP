#include "Agent.h"

using namespace Steering;
using namespace DirectX;

Agent::Agent() : GameObject()
{
	facing = XMFLOAT3(1, 0, 0);
	velocity = XMFLOAT3(0, 0, 0);
	navGraph = nullptr;
	blackboard = nullptr;
	handle = Controller(position, facing, 0);
}

Agent::Agent(ID3D11DeviceContext* devContext, ID3D11Buffer* constantBuffer, ID3D11Buffer* objectBuffer, MeshData* mesh, Graph* graph, Blackboard* blackboard, DirectX::XMFLOAT3 pos, unsigned int ID) :
	GameObject(devContext, constantBuffer, objectBuffer, mesh, pos), blackboard(blackboard), agentID(ID)
{
	navGraph = graph;
	facing = XMFLOAT3(1, 0, 0);
	velocity = XMFLOAT3(0, 0, 0);
	handle = Controller(position, facing, ID);
}

Agent::~Agent()
{
	navGraph = nullptr;
	blackboard = nullptr;
}

void Agent::Update(double deltaTime, std::vector<BoundingBox>& objects)
{
	updateController();

	fsm.Update(deltaTime, objects);

	forceToVelocity(handle.force, deltaTime);
	move(deltaTime);

	UpdateMatrix();

	blackboard->setAgentPosition(agentID, position);

	Data<XMFLOAT3> playerPos = blackboard->getPlayerPosition();

	if (playerPos.confidence > 70)
	{
		XMVECTOR pPos = XMLoadFloat3(&playerPos.info);
		XMVECTOR aPos = XMLoadFloat3(&position);

		XMVECTOR ap = pPos - aPos;
		const float panicRadius = 2.0f;

		if (XMVectorGetX(XMVector3LengthSq(ap)) > powf(panicRadius, 2.0f) || !blackboard->isAgentAlone(agentID))
		{
			blackboard->setAgentScaredState(agentID, false);
		}
	}
}

bool Agent::canSeePlayer(XMFLOAT3 playerPosition, std::vector<BoundingBox>& objects)
{
	XMVECTOR player = XMLoadFloat3(&playerPosition);
	XMVECTOR me = XMLoadFloat3(&position);
	float distToPlayer = XMVectorGetX(XMVector3Length(player - me));
	
	if (distToPlayer > viewDistance)
	{
		handle.canSeePlayer = false;
		return false;
	}

	XMVECTOR look = XMLoadFloat3(&facing);
	XMMATRIX projmat = XMMatrixPerspectiveFovLH(fieldOfView, 0.5f, 0.0f, viewDistance);
	XMMATRIX viewMat = XMMatrixLookAtLH(me, me + look, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	XMVECTOR det;
	
	BoundingFrustum viewCone = BoundingFrustum();
	viewCone.CreateFromMatrix(viewCone, projmat);
	viewCone.Transform(viewCone, XMMatrixInverse(&det, viewMat));

	if (viewCone.Contains(player) == CONTAINS)
	{
		XMVECTOR toPlayer = player - me;
		toPlayer = XMVector3Normalize(toPlayer);

		for (unsigned int i = 0; i < objects.size(); i++)
		{
			float toIntersection;
			
			if (objects.at(i).Intersects(me, toPlayer, toIntersection))
			{
				if (toIntersection < distToPlayer)
				{
					handle.canSeePlayer = false;
					return false;
				}

				blackboard->updatePlayerPosition(playerPosition);
				handle.canSeePlayer = true;
				return true;
			}
		}

		blackboard->updatePlayerPosition(playerPosition);
		handle.canSeePlayer = true;
		return true;
	}

	handle.canSeePlayer = false;
	return false;
}

void Agent::updateController()
{
	handle.facing = facing;
	handle.force = XMFLOAT3(0, 0, 0);
	handle.position = position;

	if (!fsm.initialised())
	{
		fsm.Initialise(&handle, navGraph, blackboard);
	}
}

void Agent::forceToVelocity(XMFLOAT3 force, double delta)
{
	XMVECTOR vel = XMLoadFloat3(&velocity);

	XMVECTOR acceleration = XMLoadFloat3(&force);

	vel += acceleration * delta;

	if (XMVectorGetX(XMVector3Length(vel)) > maxSpeed)
	{
		vel = XMVector3Normalize(vel);
		vel *= maxSpeed;
	}

	XMStoreFloat3(&velocity, vel);
}

void Agent::move(double delta)
{
	XMVECTOR displacement = XMLoadFloat3(&velocity);
	displacement *= (float)delta;

	XMVECTOR heading = XMVector3Normalize(displacement);
	
	if (XMVectorGetX(XMVector3LengthSq(heading)) == 0)
	{
		return;
	}
	
	heading = XMVector3Normalize(heading);
	XMStoreFloat3(&facing, heading);
	XMVECTOR oldPos = XMLoadFloat3(&position);
	
	setTranslation(XMVectorGetX(displacement), XMVectorGetY(displacement), XMVectorGetZ(displacement));
	
	XMVECTOR newPos = XMLoadFloat3(&position);
	heading = newPos - oldPos;
	heading = XMVector3Normalize(heading);

	XMFLOAT3 look(objMatrix._13, objMatrix._23, objMatrix._33);
	XMVECTOR prevHeading = XMLoadFloat3(&look);

	XMFLOAT3 defaultForwards = XMFLOAT3(1, 0, 0);
	XMVECTOR fw = XMLoadFloat3(&defaultForwards);
	fw += newPos;

	XMVECTOR dotProd = XMVector3Dot(heading, fw);
	float theta = acosf(max(-1.0f, min(XMVectorGetX(dotProd), 1.0f)));
	
	setRotation(0, -rotation.y, 0);
	setRotation(0, theta, 0);
}

void Agent::stun()
{
	handle.getStunned = true;
	velocity = XMFLOAT3(0, 0, 0);
}