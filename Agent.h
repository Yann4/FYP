#pragma once
#include "GameObject.h"
#include "Steering.h"

class Agent : public GameObject
{
private:
	DirectX::XMFLOAT3 facing;
public:
	Agent();
	Agent(ID3D11DeviceContext* devContext, ID3D11Buffer* constantBuffer, ID3D11Buffer* objectBuffer, MeshData* mesh, DirectX::XMFLOAT3 pos);
	~Agent();

	void Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects);

private:
	DirectX::XMFLOAT3 forceToUnitsMoved(DirectX::XMFLOAT3 force, double delta);
};