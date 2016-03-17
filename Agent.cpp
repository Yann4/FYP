#include "Agent.h"

using namespace Steering;
using namespace DirectX;

Agent::Agent() : GameObject()
{
	facing = XMFLOAT3(0, 0, 1);
	navGraph = nullptr;
}

Agent::Agent(ID3D11DeviceContext* devContext, ID3D11Buffer* constantBuffer, ID3D11Buffer* objectBuffer, MeshData* mesh, Graph* graph, DirectX::XMFLOAT3 pos) :
	GameObject(devContext, constantBuffer, objectBuffer, mesh, pos)
{
	navGraph = graph;
	facing = XMFLOAT3(1, 0, 0);
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

	move(forceToUnitsMoved(force, deltaTime));

	UpdateMatrix();

	return facing;
}

DirectX::XMFLOAT3 Agent::forceToUnitsMoved(XMFLOAT3 force, double delta)
{
	XMVECTOR v = XMLoadFloat3(&force);
	v *= delta;

	XMFLOAT3 vel;
	XMStoreFloat3(&vel, v);

	return vel;
}

void Agent::move(XMFLOAT3 moveBy)
{
	XMVECTOR heading = XMLoadFloat3(&moveBy);

	if (XMVectorGetX(XMVector3LengthSq(heading)) == 0)
	{
		return;
	}
	
	heading = XMVector3Normalize(heading);

	XMVECTOR oldPos = XMLoadFloat3(&position);

	setTranslation(moveBy.x, moveBy.y, moveBy.z);
	
	XMVECTOR newPos = XMLoadFloat3(&position);
	heading = newPos - oldPos;
	heading = XMVector3Normalize(heading);

	XMVECTOR prevHeading = XMLoadFloat3(&facing);

	XMVECTOR angle = XMVector3Dot(heading, prevHeading);
	float theta = acosf(XMVectorGetX(angle));

	theta = theta - rotation.y;
	
	XMFLOAT3 yAx = XMFLOAT3(0, 1, 0);
	XMVECTOR yAxis = XMLoadFloat3(&yAx);
	
	heading = XMVector3Rotate(prevHeading, XMQuaternionRotationAxis(yAxis, theta));
	heading = XMVector3Normalize(heading);
	XMStoreFloat3(&facing, heading);
	
	setRotation(0, theta, 0);
}