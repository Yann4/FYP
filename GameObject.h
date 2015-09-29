#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <directxmath.h>

#include <stack>

#include "data structs.h"
#include "Camera.h"

class GameObject
{
protected:
	ID3D11DeviceContext* context;
	ID3D11Buffer* constBuffer;

	MeshData* mesh;

	std::stack<DirectX::XMFLOAT4X4> translations;
	DirectX::XMFLOAT3 rotation;

	DirectX::XMFLOAT4X4 objMatrix;
	DirectX::XMFLOAT3 position;

public:
	GameObject();
	GameObject(ID3D11DeviceContext* devContext, ID3D11Buffer* constantBuffer, MeshData* mesh, DirectX::XMFLOAT3 pos);
	~GameObject();

	void Update(float deltaTime);
	void Draw(ID3D11PixelShader* pShader, ID3D11VertexShader* vShader, Camera& cam);

protected:
	void setScale(float x, float y, float z);
	void setRotation(float x, float y, float z);
	void setTranslation(float x, float y, float z);
	void UpdateMatrix();
};