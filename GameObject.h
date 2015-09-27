#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <directxmath.h>

#include <stack>

#include "data structs.h"

class GameObject
{
private:
	ID3D11DeviceContext* context;
	ID3D11Buffer* constBuffer;

	//Mesh data
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int numIndices;

	std::stack<DirectX::XMFLOAT4X4> translations;
	DirectX::XMFLOAT3 rotation;

	DirectX::XMFLOAT4X4 objMatrix;
	DirectX::XMFLOAT3 position;

public:
	GameObject();
	GameObject(ID3D11DeviceContext* devContext, ID3D11Buffer* constantBuffer, ID3D11Buffer* vertexBuff, ID3D11Buffer* indexBuff, int indexCount,
		DirectX::XMFLOAT3 pos);
	~GameObject();

	void Update(float deltaTime);

	void Draw(ID3D11PixelShader* pShader, ID3D11VertexShader* vShader, DirectX::XMFLOAT4X4& view, DirectX::XMFLOAT4X4& projection);

private:
	void setScale(float x, float y, float z);
	void setRotation(float x, float y, float z);
	void setTranslation(float x, float y, float z);
	void UpdateMatrix();
};