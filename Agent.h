#pragma once

#include <stack>

#include "GameObject.h"
#include "Steering.h"
#include "Graph.h"
#include "Marpo.h"
#include "AgentController.h"

class Agent : public GameObject
{
private:
	DirectX::XMFLOAT3 facing;
	
	DirectX::XMFLOAT3 velocity;

	Graph* navGraph;

	Marpo fsm;
	Controller handle;

public:
	Agent();
	Agent(ID3D11DeviceContext* devContext, ID3D11Buffer* constantBuffer, ID3D11Buffer* objectBuffer, MeshData* mesh, Graph* graph, DirectX::XMFLOAT3 pos);
	~Agent();

	void Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects);
	
	void updateController();

	inline DirectX::XMFLOAT3 getFacing() { return facing; }
private:
	void forceToVelocity(DirectX::XMFLOAT3 force, double delta);
	void move(double delta);
};