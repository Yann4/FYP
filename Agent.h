#pragma once
#include "GameObject.h"

class Agent : public GameObject
{
public:
	Agent();
	Agent(ID3D11DeviceContext* devContext, ID3D11Buffer* constantBuffer, ID3D11Buffer* objectBuffer, MeshData* mesh, DirectX::XMFLOAT3 pos);
	~Agent();

	void Update(float deltaTime);
};