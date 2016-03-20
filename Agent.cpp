#include "Agent.h"

using namespace Steering;
using namespace DirectX;

Agent::Agent() : GameObject()
{
	facing = XMFLOAT3(1, 0, 0);
	velocity = XMFLOAT3(0, 0, 0);
	navGraph = nullptr;
}

Agent::Agent(ID3D11DeviceContext* devContext, ID3D11Buffer* constantBuffer, ID3D11Buffer* objectBuffer, MeshData* mesh, Graph* graph, DirectX::XMFLOAT3 pos) :
	GameObject(devContext, constantBuffer, objectBuffer, mesh, pos)
{
	navGraph = graph;
	facing = XMFLOAT3(1, 0, 0);
	velocity = XMFLOAT3(0, 0, 0);
}

Agent::~Agent()
{
	navGraph = nullptr;
}

XMFLOAT3 Agent::Update(double deltaTime, std::vector<BoundingBox>& objects)
{
	if (path.empty())
	{
		path = navGraph->findPath(position, XMFLOAT3(4, 0, -4));
	}

	XMFLOAT3 seek = pathFollowing(position, path);
	XMFLOAT3 oa = obstacleAvoidForce(objects, position, facing);

	XMFLOAT3 force = aggregateForces(seek, XMFLOAT3(0, 0, 0), oa);

	forceToVelocity(force, deltaTime);
	move(deltaTime);

	UpdateMatrix();

	velocity = XMFLOAT3(0, 0, 0);

	return facing;
}

void Agent::forceToVelocity(XMFLOAT3 force, double delta)
{
	XMVECTOR vel = XMLoadFloat3(&velocity);

	XMVECTOR acceleration = XMLoadFloat3(&force);

	vel += acceleration * delta;

	XMStoreFloat3(&velocity, vel);
}

void Agent::move(double delta)
{
	XMVECTOR displacement = XMLoadFloat3(&velocity);
	displacement *= delta;

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

	XMVECTOR prevHeading = XMLoadFloat3(&facing);

	XMFLOAT3 defaultForwards = XMFLOAT3(1, 0, 0);
	XMVECTOR fw = XMLoadFloat3(&defaultForwards);

	XMVECTOR angle = XMVector3Dot(heading, fw);
	float theta = acosf(XMVectorGetX(angle));
	
	setRotation(0, -rotation.y, 0);
	setRotation(0, theta, 0);
}