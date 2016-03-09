#include "Agent.h"

using namespace Steering;
using namespace DirectX;

Agent::Agent() : GameObject()
{
	facing = XMFLOAT3(1, 0, 0);
}

Agent::Agent(ID3D11DeviceContext* devContext, ID3D11Buffer* constantBuffer, ID3D11Buffer* objectBuffer, MeshData* mesh, DirectX::XMFLOAT3 pos) :
	GameObject(devContext, constantBuffer, objectBuffer, mesh, pos)
{
	facing = XMFLOAT3(0, 0, 1);
}

Agent::~Agent()
{

}

void Agent::Update(double deltaTime, std::vector<BoundingBox>& objects)
{
	XMFLOAT3 force = aggregateForces(XMFLOAT3(0,0,0),XMFLOAT3(0,0,0),obstacleAvoidForce(objects, position, facing));
	force.z += 0.0001f;

	XMFLOAT3 velocity = forceToUnitsMoved(force, deltaTime);

	setTranslation(velocity.x, velocity.y, velocity.z);
	
	UpdateMatrix();
}

DirectX::XMFLOAT3 Agent::forceToUnitsMoved(XMFLOAT3 force, double delta)
{
	XMVECTOR v = XMLoadFloat3(&force);
	v *= delta;

	XMFLOAT3 vel;
	XMStoreFloat3(&vel, v);

	return vel;
}