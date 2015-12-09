#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <directxmath.h>

#include <stack>

#include "data structs.h"
#include "Camera.h"
#include "Frustum.h"
#include "Light.h"

class GameObject
{
protected:
	ID3D11DeviceContext* context;
	ID3D11Buffer* frameConstBuffer;
	ID3D11Buffer* objectConstBuffer;

	MeshData* mesh;

	std::stack<DirectX::XMFLOAT4X4> translations;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;

	DirectX::XMFLOAT4X4 objMatrix;
	DirectX::XMFLOAT3 position;

public:
	GameObject();
	GameObject(ID3D11DeviceContext* devContext, ID3D11Buffer* constantBuffer, ID3D11Buffer* objectBuffer, MeshData* mesh, DirectX::XMFLOAT3 pos);
	~GameObject();

	void Update(float deltaTime);
	void Draw(ID3D11PixelShader* pShader, ID3D11VertexShader* vShader, Frustum& frustum, Camera& cam);

	void setScale(float x, float y, float z);
	void setRotation(float x, float y, float z);
	void setTranslation(float x, float y, float z);
	void UpdateMatrix();

	void moveFromCollision(float x, float y, float z);

	inline DirectX::XMFLOAT3 Pos(){ return position; };
	inline DirectX::XMFLOAT3 Size(){ return scale; };
	inline DirectX::XMFLOAT3 Rotation() { return rotation; };
};