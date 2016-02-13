#include "Surface.h"

using namespace DirectX;
using std::vector;
using std::array;

Surface::Surface()
{
	context = nullptr;

	vertexBuffer = nullptr;
	indexBuffer = nullptr;
	constBuffer = nullptr;
}

/*
Needs to be defined from top to bottom
Example order of vectors

------ 0
------ 1
------ 2
*/
Surface::Surface(vector<Spline>& splines, ID3D11DeviceContext* context, ID3D11Device* device) : context(context)
{
	context->AddRef();

	vector<Vertex> vertices;
	vector<WORD> indices;

	int count = 0;

	for (auto spline : splines)
	{
		auto splinePoints = spline.getLinePoints();

		for (auto point : splinePoints)
		{
			vertices.push_back(Vertex());
			vertices.at(count).position = XMFLOAT4(point.x, point.y, point.z, 1.0f);
			vertices.at(count).colour = XMFLOAT4(0.0f, 0.0f, 0.545f, 1.0f);
			count++;
		}
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &(vertices)[0];

	device->CreateBuffer(&bd, &InitData, &vertexBuffer);
	
	WORD pointsInSpline = 101;
	
	for (WORD topLeftIndex = 0; topLeftIndex < vertices.size() - pointsInSpline - 1; topLeftIndex++)
	{
		//Triangle 1
		indices.push_back(topLeftIndex);
		indices.push_back(topLeftIndex + pointsInSpline);
		indices.push_back(topLeftIndex + 1);

		//Triangle 2
		indices.push_back(topLeftIndex + pointsInSpline);
		indices.push_back(topLeftIndex + pointsInSpline + 1);
		indices.push_back(topLeftIndex + 1);
	}

	indexCount = indices.size();

	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;
	bd.ByteWidth = sizeof(WORD) * indices.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &(indices)[0];
	device->CreateBuffer(&bd, &InitData, &indexBuffer);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(LineCBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, nullptr, &constBuffer);
}

Surface::~Surface()
{
	if (context) context->Release();
	if (vertexBuffer) vertexBuffer->Release();
	if (indexBuffer) indexBuffer->Release();
	if (constBuffer) constBuffer->Release();
}

void Surface::Draw(ID3D11PixelShader* pShader, ID3D11VertexShader* vShader, Camera& cam)
{
	context->VSSetShader(vShader, nullptr, 0);
	context->PSSetShader(pShader, nullptr, 0);

	LineCBuffer lCB;
	lCB.world = XMMatrixIdentity();
	lCB.view = XMMatrixTranspose(cam.getView());
	lCB.projection = XMMatrixTranspose(cam.getProjection());

	context->UpdateSubresource(constBuffer, 0, nullptr, &lCB, 0, 0);

	context->VSSetConstantBuffers(0, 1, &constBuffer);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	context->DrawIndexed(indexCount, 0, 0);
}