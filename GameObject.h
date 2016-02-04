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

	std::stack<DirectX::XMFLOAT4X4> transformations;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;

	DirectX::XMFLOAT4X4 objMatrix;
	DirectX::XMFLOAT3 position;

	bool collider = false;
	bool shouldBeFrustumCulled = true;
	bool objectMoves = true;

	bool isGround = false;
	bool onGround = false;

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
	inline DirectX::XMFLOAT3 Size()
	{
		DirectX::XMFLOAT3 size;
		DirectX::XMVECTOR sizeV = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&mesh->size), DirectX::XMMatrixScaling(scale.x, scale.y, scale.z));
		DirectX::XMStoreFloat3(&size, sizeV);
		return size;
	}
	inline DirectX::XMFLOAT3 Rotation() { return rotation; };

	bool getCollider() { return collider; }
	void setCollider(bool isCollider) { collider = isCollider; }

	bool getCullState() { return shouldBeFrustumCulled; }
	void setCullState(bool cull) { shouldBeFrustumCulled = cull; }

	bool getMovementState() { return objectMoves; }
	void setMovementState(bool objectMoves) { GameObject::objectMoves = objectMoves; }

	bool getIsGround() { return isGround; }
	void setIsGround(bool isGround) { GameObject::isGround = isGround; }

	bool getOnGround() { return onGround; }
	void setOnGround(bool onGround) { GameObject::onGround = onGround; }
};