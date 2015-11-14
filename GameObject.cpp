#include "GameObject.h"

using namespace DirectX;

GameObject::GameObject()
{
	context = nullptr;
	constBuffer = nullptr;

	mesh = nullptr;

	XMStoreFloat4x4(&objMatrix, XMMatrixIdentity());
	translations = std::stack<XMFLOAT4X4>();

	position = DirectX::XMFLOAT3(0, 0, 0);
	rotation = XMFLOAT3(0, 0, 0);
	scale = XMFLOAT3(0, 0, 0);
}

GameObject::GameObject(ID3D11DeviceContext* devContext, ID3D11Buffer* constantBuffer, MeshData* mesh, DirectX::XMFLOAT3 pos)
	:context(devContext), constBuffer(constantBuffer), mesh(mesh), position(pos)
{
	XMStoreFloat4x4(&objMatrix, XMMatrixIdentity());
	translations = std::stack<XMFLOAT4X4>();
	rotation = XMFLOAT3(0, 0, 0);
	scale = mesh->size;

	setTranslation(pos.x, pos.y, pos.z);
	UpdateMatrix();
}

GameObject::~GameObject()
{
	context = nullptr;
	constBuffer = nullptr;
	mesh = nullptr;
}


#pragma region Transformations
//All transformations are pushed onto a stack and then resolved at the end of the Update function
//This is to reduce the number of operations done on the world matrix
void GameObject::setScale(float x, float y, float z)
{
	XMFLOAT4X4 temp;
	XMStoreFloat4x4(&temp, XMMatrixScaling(x, y, z));
	translations.push(temp);

	
	XMVECTOR sc = XMLoadFloat3(&scale);
	sc = XMVector3Transform(sc, XMMatrixScaling(x, y, z));
	XMStoreFloat3(&scale, sc);
}

void GameObject::setRotation(float x, float y, float z)
{
	XMFLOAT4X4 temp;

	XMFLOAT3 pos = position;
	XMStoreFloat4x4(&temp, XMMatrixTranslation(pos.x, pos.y, pos.z));
	translations.push(temp);

	XMStoreFloat4x4(&temp, XMMatrixRotationX(x) * XMMatrixRotationY(y) * XMMatrixRotationZ(z));
	rotation.x += x;
	rotation.y += y;
	rotation.z += z;
	translations.push(temp);

	XMStoreFloat4x4(&temp, XMMatrixTranslation(-pos.x, -pos.y, -pos.z));
	translations.push(temp);
}

void GameObject::setTranslation(float x, float y, float z)
{
	XMFLOAT4X4 temp;
	XMStoreFloat4x4(&temp, XMMatrixTranslation(x, y, z));
	translations.push(temp);
}

void GameObject::UpdateMatrix()
{
	if (!translations.empty())
	{
		XMMATRIX tr;
		XMFLOAT4X4 temp = translations.top();
		translations.pop();
		tr = XMLoadFloat4x4(&temp);

		while (!translations.empty())
		{
			temp = translations.top();
			XMMATRIX tempMat = XMLoadFloat4x4(&temp);
			tr *= tempMat;
			translations.pop();
		}
		XMMATRIX p = XMLoadFloat4x4(&objMatrix);
		p *= tr;
		XMStoreFloat4x4(&objMatrix, p);
	}
}
#pragma endregion

//UpdateMatrix() should be called at the end of Update, as it flushes any transformations to the world matrix
void GameObject::Update(float deltaTime)
{
	UpdateMatrix();
}

void GameObject::Draw(ID3D11PixelShader* pShader, ID3D11VertexShader* vShader, Frustum& frustum, Camera& cam, Light& light)
{
	if (!frustum.checkSphere(position, max(max(scale.x, scale.y), scale.z)))
	{
		return;
	}

	context->VSSetShader(vShader, nullptr, 0);
	context->VSSetConstantBuffers(0, 1, &constBuffer);
	context->PSSetShader(pShader, nullptr, 0);
	context->PSSetConstantBuffers(0, 1, &constBuffer);

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

	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(worldMat);
	cb.mView = XMMatrixTranspose(viewMat);
	cb.mProjection = XMMatrixTranspose(projectionMat);

	cb.diffuseLight = light.diffuse;
	cb.diffuseMtl = mesh->material.diffuse;

	cb.ambientLight = light.ambient;
	cb.ambientMtl = mesh->material.ambient;

	cb.specularLight = light.specular;
	cb.specularMtl = mesh->material.specular;
	cb.specularPower = light.specularPower;
	cb.cameraPosW = XMFLOAT3(cam.getPosition().x, cam.getPosition().y, cam.getPosition().z);

	cb.lightVecW = light.lightVecW;

	context->UpdateSubresource(constBuffer, 0, nullptr, &cb, 0, 0);

	context->DrawIndexed(mesh->numIndices, 0, 0);
}