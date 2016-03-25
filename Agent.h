#pragma once

#include <stack>

#include "GameObject.h"
#include "Steering.h"
#include "Graph.h"
#include "Marpo.h"
#include "AgentController.h"
#include "Blackboard.h"

class Agent : public GameObject
{
private:
	DirectX::XMFLOAT3 facing;
	
	DirectX::XMFLOAT3 velocity;

	Graph* navGraph;

	Marpo fsm;
	Controller handle;

	Blackboard* blackboard;

	const float viewDistance = 5.0f;
	const float fieldOfView = XM_PIDIV4;

public:
	Agent();
	Agent(ID3D11DeviceContext* devContext, ID3D11Buffer* constantBuffer, ID3D11Buffer* objectBuffer, MeshData* mesh, Graph* graph, Blackboard* blackboard, DirectX::XMFLOAT3 pos);
	~Agent();

	DirectX::XMFLOAT3 Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects);
	
	bool canSeePlayer(DirectX::XMFLOAT3 playerPosition, std::vector<DirectX::BoundingBox>& objects);
	
	void updateController();

	inline DirectX::XMFLOAT3 getFacing() { return facing; }
private:
	void forceToVelocity(DirectX::XMFLOAT3 force, double delta);
	void move(double delta);
};