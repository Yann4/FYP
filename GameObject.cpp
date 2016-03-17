#include "GameObject.h"

using namespace DirectX;

GameObject::GameObject()
{
	context = nullptr;
	frameConstBuffer = nullptr;
	objectConstBuffer = nullptr;

	mesh = nullptr;

	XMStoreFloat4x4(&objMatrix, XMMatrixIdentity());
	transformations = std::stack<XMFLOAT4X4>();

	position = DirectX::XMFLOAT3(0, 0, 0);
	rotation = XMFLOAT3(0, 0, 0);
	scale = XMFLOAT3(0, 0, 0);
}

GameObject::GameObject(ID3D11DeviceContext* devContext, ID3D11Buffer* frameConstantBuffer, ID3D11Buffer* objectBuffer, MeshData* mesh, DirectX::XMFLOAT3 pos)
	:context(devContext), frameConstBuffer(frameConstantBuffer), objectConstBuffer(objectBuffer), mesh(mesh), position(XMFLOAT3(0,0,0))
{
	XMStoreFloat4x4(&objMatrix, XMMatrixIdentity());
	transformations = std::stack<XMFLOAT4X4>();
	rotation = XMFLOAT3(0, 0, 0);
	scale = XMFLOAT3(1,1,1);

	setTranslation(pos.x, pos.y, pos.z);
	UpdateMatrix();
}

GameObject::~GameObject()
{
	mesh = nullptr;
}


#pragma region Transformations
//All transformations are pushed onto a stack and then resolved at the end of the Update function
//This is to reduce the number of operations done on the world matrix
void GameObject::setScale(float x, float y, float z)
{
	XMFLOAT4X4 temp;

	XMStoreFloat4x4(&temp, XMMatrixTranslation(position.x, position.y, position.z));
	transformations.push(temp);

	XMStoreFloat4x4(&temp, XMMatrixScaling(x, y, z));
	transformations.push(temp);
	
	XMStoreFloat4x4(&temp, XMMatrixTranslation(-position.x, -position.y, -position.z));
	transformations.push(temp);
	
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;
}

void GameObject::setRotation(float x, float y, float z)
{
	XMFLOAT4X4 temp;

	XMStoreFloat4x4(&temp, XMMatrixTranslation(position.x, position.y, position.z));
	transformations.push(temp);
	
	rotation.x += x;
	rotation.y += y;
	rotation.z += z;

	if (rotation.x >= XM_2PI)
	{
		rotation.x -= XM_2PI;
	}
	else if (rotation.x <= -XM_2PI)
	{
		rotation.x += XM_2PI;
	}

	if (rotation.y >= XM_2PI)
	{
		rotation.y -= XM_2PI;
	}
	else if (rotation.y <= -XM_2PI)
	{
		rotation.y += XM_2PI;
	}

	if (rotation.z >= XM_2PI)
	{
		rotation.z -= XM_2PI;
	}
	else if (rotation.z <= -XM_2PI)
	{
		rotation.z += XM_2PI;
	}

	XMStoreFloat4x4(&temp, XMMatrixRotationX(x) * XMMatrixRotationY(y) * XMMatrixRotationZ(z));
	transformations.push(temp);

	XMStoreFloat4x4(&temp, XMMatrixTranslation(-position.x, -position.y, -position.z));
	transformations.push(temp);
}

void GameObject::setTranslation(float x, float y, float z)
{
	XMFLOAT4X4 temp;
	XMStoreFloat4x4(&temp, XMMatrixTranslation(x, y, z));
	transformations.push(temp);

	XMStoreFloat3(&position, XMVector3Transform(XMLoadFloat3(&position), XMMatrixTranslation(x, y, z)));
}

void GameObject::UpdateMatrix()
{
	if (!transformations.empty())
	{
		XMMATRIX tr;
		XMFLOAT4X4 temp = transformations.top();
		transformations.pop();
		tr = XMLoadFloat4x4(&temp);

		while (!transformations.empty())
		{
			temp = transformations.top();
			XMMATRIX tempMat = XMLoadFloat4x4(&temp);
			tr *= tempMat;
			transformations.pop();
		}
		XMMATRIX p = XMLoadFloat4x4(&objMatrix);
		p *= tr;
		XMStoreFloat4x4(&objMatrix, p);
	}
}
#pragma endregion

void GameObject::moveFromCollision(float x, float y, float z)
{
	if (objectMoves)
	{
		setTranslation(x, y, z);
		UpdateMatrix();
	}
}

BoundingBox GameObject::getBoundingBox()
{
	XMFLOAT3 size = Size();
	return BoundingBox(position, XMFLOAT3(size.x / 2, size.y / 2, size.z / 2));
}

//UpdateMatrix() should be called at the end of Update, as it flushes any transformations to the world matrix
void GameObject::Update(float deltaTime)
{
	if (!objectMoves)
	{
		return;
	}

	if (!onGround)
	{
		//Gravity
		setTranslation(0, -9.81f * (deltaTime / 1000.0f), 0);
	}

	UpdateMatrix();
}

void GameObject::Draw(ID3D11PixelShader* pShader, ID3D11VertexShader* vShader, Frustum& frustum, Camera& cam)
{
	if (shouldBeFrustumCulled && !frustum.checkSphere(position, max(max(scale.x, scale.y), scale.z)))
	{
		return;
	}

	context->VSSetShader(vShader, nullptr, 0);
	context->VSSetConstantBuffers(0, 1, &frameConstBuffer);
	context->VSSetConstantBuffers(1, 1, &objectConstBuffer);

	context->PSSetShader(pShader, nullptr, 0);
	context->PSSetConstantBuffers(0, 1, &frameConstBuffer);
	context->PSSetConstantBuffers(1, 1, &objectConstBuffer);


	context->PSSetShaderResources(0, 1, &mesh->textureRV);
	context->PSSetShaderResources(1, 1, &mesh->specularRV);
	context->PSSetShaderResources(2, 1, &mesh->normalMapRV);

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &mesh->vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(mesh->indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	XMMATRIX worldMat = XMLoadFloat4x4(&objMatrix);
	XMMATRIX viewMat = cam.getView();
	XMMATRIX projectionMat = cam.getProjection();

	objectCB cb;
	cb.projection = XMMatrixTranspose(projectionMat);
	cb.view = XMMatrixTranspose(viewMat);
	cb.world = XMMatrixTranspose(worldMat);

	if (mesh->textureRV == nullptr)
	{
		cb.useTextures = 0;
	}
	else
	{
		cb.useTextures = 1;
	}

	for (unsigned int i = 0; i < mesh->parts.size(); i++)
	{
		if (mesh->parts.at(i).material == nullptr)
		{
			cb.material = CBMaterial(mesh->material.specular, mesh->material.ambient, mesh->material.diffuse);
		}
		else
		{
			cb.material = CBMaterial(mesh->parts.at(i).material->specular, mesh->parts.at(i).material->ambient, mesh->parts.at(i).material->diffuse);
		}
		context->UpdateSubresource(objectConstBuffer, 0, nullptr, &cb, 0, 0);

		context->DrawIndexed(mesh->parts.at(i).endIndex - mesh->parts.at(i).startIndex, mesh->parts.at(i).startIndex, 0);
	}
}