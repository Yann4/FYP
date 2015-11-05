#include "Skybox.h"

Skybox::Skybox()
{
	_pImmediateContext = nullptr;
	_pConstantBuffer = nullptr;

	sphereIBuff = nullptr;
	sphereVBuff = nullptr;

	skySRV = nullptr;

	DSLessEqual = nullptr;
	RSCullNone = nullptr;
	CubesTexSamplerState = nullptr;

	noSphereVert = 0;
	noSphereFace = 0;
	sphereWorld = DirectX::XMMatrixIdentity();
}

void Skybox::init(ID3D11DeviceContext* context, ID3D11Device* _pd3dDevice, LPCTSTR fileName)
{
	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(skyConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	_pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

	_pImmediateContext = context;
	sphereWorld = DirectX::XMMatrixIdentity();

	CreateSphere(_pd3dDevice, 10, 10);

	//Skybox states
	D3D11_RASTERIZER_DESC	wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_SOLID;
	wfdesc.CullMode = D3D11_CULL_NONE;
	wfdesc.CullMode = D3D11_CULL_NONE;
	_pd3dDevice->CreateRasterizerState(&wfdesc, &RSCullNone);

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	_pd3dDevice->CreateDepthStencilState(&dssDesc, &DSLessEqual);

	DirectX::CreateDDSTextureFromFile(_pd3dDevice, fileName, nullptr, &skySRV);

	// Describe the Sample State
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the Sample State
	_pd3dDevice->CreateSamplerState(&sampDesc, &CubesTexSamplerState);
}

Skybox::~Skybox()
{
	_pImmediateContext = nullptr;
	_pConstantBuffer = nullptr;

	if (CubesTexSamplerState) CubesTexSamplerState->Release();
	if (sphereIBuff) sphereIBuff->Release();
	if (sphereVBuff) sphereVBuff->Release();
	if (skySRV) skySRV->Release();
	if (DSLessEqual)DSLessEqual->Release();
	if (RSCullNone) RSCullNone->Release();
}

void Skybox::CreateSphere(ID3D11Device* _pd3dDevice, int LatLines, int LongLines)
{
	HRESULT hr;

	noSphereVert = ((LatLines - 2) * LongLines) + 2;
	noSphereFace = ((LatLines - 3)*(LongLines)* 2) + (LongLines * 2);

	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	std::vector<SimpleVertex> vertices(noSphereVert);

	DirectX::XMVECTOR currVertPos = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	vertices[0].Pos.x = 0.0f;
	vertices[0].Pos.y = 0.0f;
	vertices[0].Pos.z = 1.0f;

	DirectX::XMMATRIX Rotationx, Rotationy;
	for (int i = 0; i < LatLines - 2; ++i)
	{
		spherePitch = (i + 1) * (3.14f / (LatLines - 1));
		Rotationx = DirectX::XMMatrixRotationX(spherePitch);

		for (int j = 0; j < LongLines; ++j)
		{
			sphereYaw = j * (6.28f / (LongLines));
			Rotationy = DirectX::XMMatrixRotationZ(sphereYaw);
			currVertPos = XMVector3TransformNormal(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx * Rotationy));
			currVertPos = DirectX::XMVector3Normalize(currVertPos);
			vertices[i*LongLines + j + 1].Pos.x = DirectX::XMVectorGetX(currVertPos);
			vertices[i*LongLines + j + 1].Pos.y = DirectX::XMVectorGetY(currVertPos);
			vertices[i*LongLines + j + 1].Pos.z = DirectX::XMVectorGetZ(currVertPos);
		}
	}

	vertices[noSphereVert - 1].Pos.x = 0.0f;
	vertices[noSphereVert - 1].Pos.y = 0.0f;
	vertices[noSphereVert - 1].Pos.z = -1.0f;


	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(SimpleVertex) * noSphereVert;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = &vertices[0];
	hr = _pd3dDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &sphereVBuff);


	std::vector<DWORD> indices(noSphereFace * 3);

	int k = 0;
	for (int l = 0; l < LongLines - 1; ++l)
	{
		indices[k] = 0;
		indices[k + 1] = l + 1;
		indices[k + 2] = l + 2;
		k += 3;
	}

	indices[k] = 0;
	indices[k + 1] = LongLines;
	indices[k + 2] = 1;
	k += 3;

	for (int i = 0; i < LatLines - 3; ++i)
	{
		for (int j = 0; j < LongLines - 1; ++j)
		{
			indices[k] = i*LongLines + j + 1;
			indices[k + 1] = i*LongLines + j + 2;
			indices[k + 2] = (i + 1)*LongLines + j + 1;

			indices[k + 3] = (i + 1)*LongLines + j + 1;
			indices[k + 4] = i*LongLines + j + 2;
			indices[k + 5] = (i + 1)*LongLines + j + 2;

			k += 6; // next quad
		}

		indices[k] = (i*LongLines) + LongLines;
		indices[k + 1] = (i*LongLines) + 1;
		indices[k + 2] = ((i + 1)*LongLines) + LongLines;

		indices[k + 3] = ((i + 1)*LongLines) + LongLines;
		indices[k + 4] = (i*LongLines) + 1;
		indices[k + 5] = ((i + 1)*LongLines) + 1;

		k += 6;
	}

	for (int l = 0; l < LongLines - 1; ++l)
	{
		indices[k] = noSphereVert - 1;
		indices[k + 1] = (noSphereVert - 1) - (l + 1);
		indices[k + 2] = (noSphereVert - 1) - (l + 2);
		k += 3;
	}

	indices[k] = noSphereVert - 1;
	indices[k + 1] = (noSphereVert - 1) - LongLines;
	indices[k + 2] = noSphereVert - 2;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(int) * noSphereFace * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = &indices[0];
	_pd3dDevice->CreateBuffer(&indexBufferDesc, &iinitData, &sphereIBuff);

}

void Skybox::Update(Camera* cam)
{
	//Reset sphereWorld
	sphereWorld = DirectX::XMMatrixIdentity();

	//Define sphereWorld's world space matrix
	DirectX::XMMATRIX Scale = DirectX::XMMatrixScaling(5.0f, 5.0f, 5.0f);

	//Make sure the sphere is always centered around camera
	DirectX::XMFLOAT4 pos = cam->getPosition();
	DirectX::XMMATRIX Translation = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	//Set sphereWorld's world space using the transformations
	sphereWorld = Scale * Translation;
}

void Skybox::Draw(ID3D11VertexShader* _pVertexShader, ID3D11PixelShader* _pPixelShader, Camera* cam)
{
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	_pImmediateContext->IASetIndexBuffer(sphereIBuff, DXGI_FORMAT_R32_UINT, 0);
	_pImmediateContext->IASetVertexBuffers(0, 1, &sphereVBuff, &stride, &offset);

	skyConstantBuffer cb;
	DirectX::XMMATRIX world = sphereWorld;
	DirectX::XMMATRIX WVP = sphereWorld * cam->getView() * cam->getProjection();
	cb.projection = XMMatrixTranspose(WVP);

	cb.world = XMMatrixTranspose(world);

	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, NULL, &cb, 0, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetShaderResources(0, 1, &skySRV);
	_pImmediateContext->PSSetSamplers(0, 1, &CubesTexSamplerState);

	_pImmediateContext->VSSetShader(_pVertexShader, 0, 0);
	_pImmediateContext->PSSetShader(_pPixelShader, 0, 0);
	_pImmediateContext->OMSetDepthStencilState(DSLessEqual, 0);
	_pImmediateContext->RSSetState(RSCullNone);
	_pImmediateContext->DrawIndexed(noSphereFace * 3, 0, 0);

}