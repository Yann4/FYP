#include "Agent.h"

Agent::Agent() : GameObject()
{
	
}

Agent::Agent(ID3D11DeviceContext* devContext, ID3D11Buffer* constantBuffer, ID3D11Buffer* objectBuffer, MeshData* mesh, DirectX::XMFLOAT3 pos) :
	GameObject(devContext, constantBuffer, objectBuffer, mesh, pos)
{

}

Agent::~Agent()
{

}

void Agent::Update(float deltaTime)
{

}