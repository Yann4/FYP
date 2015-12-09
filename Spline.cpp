#include "Spline.h"
using namespace DirectX;

Spline::Spline()
{
	context = nullptr;
	controlPoints = std::vector<XMFLOAT3>();
	linePoints = std::vector<XMFLOAT3>(100);
}

Spline::Spline(std::vector<XMFLOAT3> controlPoints, ID3D11DeviceContext * context, ID3D11Device* device): controlPoints(controlPoints), context(context)
{
	linePoints = std::vector<XMFLOAT3>(101);
	generateLine();
	createBuffers(device);
}

void Spline::generateLine()
{
	unsigned int count = 0;
	for (float perc = 0; perc <= 1; perc += 0.01f)
	{
		std::vector<XMFLOAT3> lerpVector = controlPoints;
		while (lerpVector.size() != 1)
		{
			std::vector<XMFLOAT3> temp = std::vector<XMFLOAT3>();

			for (unsigned int i = 0; i < lerpVector.size() - 1; i++)
			{
				temp.push_back(lerp(lerpVector.at(i), lerpVector.at(i + 1), perc));
			}
			lerpVector = temp;
		}
		linePoints.at(count) = lerpVector.at(0);
		count++;
	}
}

void Spline::createBuffers(ID3D11Device* device)
{
	Vertex vertices[100];
	for (int i = 0; i < 100; i++)
	{
		Vertex v;
		v.position = linePoints.at(0);
		v.colour = XMFLOAT4(0, 0, 0, 1.0);
		vertices[i] = v;
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * linePoints.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	device->CreateBuffer(&bd, &InitData, &vertexBuffer);

	bd.ByteWidth = sizeof(WORD) * linePoints.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	WORD indices[100];
	for (WORD i = 0; i < 100; i++)
	{
		indices[i] = i;
	}

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indices;
	device->CreateBuffer(&bd, &InitData, &indexBuffer);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(LineCBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, nullptr, &constBuffer);
}

XMFLOAT3 Spline::lerp(XMFLOAT3 a, XMFLOAT3 b, float u)
{
	XMVECTOR aV = XMLoadFloat3(&a);
	XMVECTOR bV = XMLoadFloat3(&b);
	aV *= (1 - u);
	bV *= u;

	aV += bV;

	XMStoreFloat3(&a, aV);
	return a;
}

void Spline::Draw(ID3D11PixelShader* pShader, ID3D11VertexShader* vShader, Camera& cam)
{
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	LineCBuffer lCB;
	lCB.world = XMMatrixIdentity();
	lCB.view = cam.getView();
	lCB.projection = cam.getProjection();

	context->VSSetShader(vShader, nullptr, 0);
	context->VSSetConstantBuffers(0, 1, &constBuffer);

	context->PSSetShader(pShader, nullptr, 0);
	context->PSSetConstantBuffers(0, 1, &constBuffer);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	context->UpdateSubresource(constBuffer, 0, nullptr, &lCB, 0, 0);

	context->DrawIndexed(100, 0, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}