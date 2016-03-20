#pragma once

#include <stack>

#include "GameObject.h"
#include "Steering.h"
#include "Graph.h"

class Agent : public GameObject
{
private:
	DirectX::XMFLOAT3 facing;
	
	DirectX::XMFLOAT3 velocity;

	Graph* navGraph;
	std::stack<DirectX::XMFLOAT3> path;
public:
	Agent();
	Agent(ID3D11DeviceContext* devContext, ID3D11Buffer* constantBuffer, ID3D11Buffer* objectBuffer, MeshData* mesh, Graph* graph, DirectX::XMFLOAT3 pos);
	~Agent();

	DirectX::XMFLOAT3 Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects);

private:
	void forceToVelocity(DirectX::XMFLOAT3 force, double delta);
	void move(double delta);
};