#include "Spline.h"
using namespace DirectX;

Spline::Spline()
{
	context = nullptr;
	splineLayout = nullptr;
	vertexBuffer = nullptr;
	indexBuffer = nullptr;
	constBuffer = nullptr;
	colour = XMFLOAT4(0, 0, 0, 1);
}

Spline::Spline(std::vector<XMFLOAT3> controlPoints, ID3D11DeviceContext * context, ID3D11Device* device, ID3D11InputLayout* layout, XMFLOAT4 colour):
	context(context), splineLayout(layout), device(device), colour(colour)
{
	vertexBuffer = nullptr;
	indexBuffer = nullptr;
	constBuffer = nullptr;

	context->AddRef();
	splineLayout->AddRef();
	device->AddRef();

	colour = XMFLOAT4(0, 0, 0, 1);

	generateLine(controlPoints);
	createBuffers();
}

Spline::~Spline()
{
	if (context) context->Release();
	if (splineLayout) splineLayout->Release();
	if (vertexBuffer) vertexBuffer->Release();
	if (indexBuffer) indexBuffer->Release();
	if (constBuffer) constBuffer->Release();
}

void Spline::generateLine(const std::vector<XMFLOAT3>& controlPoints)
{
	std::array<XMFLOAT3, 101>();
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

void Spline::createBuffers()
{
	Vertex vertices[101];
	WORD indices[101];

	for (int i = 0; i < NUM_POINTS; i++)
	{
		Vertex v;
		v.position = XMFLOAT4(linePoints.at(i).x, linePoints.at(i).y, linePoints.at(i).z, 1.0f);
		v.colour = colour;
		vertices[i] = v;
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex) * NUM_POINTS;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	HRESULT hr = device->CreateBuffer(&bd, &InitData, &vertexBuffer);

	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;
	bd.ByteWidth = sizeof(WORD) * NUM_POINTS;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	for (WORD i = 0; i < NUM_POINTS; i++)
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
	hr = device->CreateBuffer(&bd, nullptr, &constBuffer);

	hr = S_OK;
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

void Spline::Draw(ID3D11PixelShader* pShader, ID3D11VertexShader* vShader, Camera& cam, bool partOfSplineBatch)
{
	ID3D11InputLayout* prevLayout = nullptr;

	if (!partOfSplineBatch)
	{
		context->IAGetInputLayout(&prevLayout);

		if (prevLayout != splineLayout)
		{
			context->IASetInputLayout(splineLayout);
		}

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		LineCBuffer lCB;
		lCB.world = XMMatrixIdentity();
		lCB.view = XMMatrixTranspose(cam.getView());
		lCB.projection = XMMatrixTranspose(cam.getProjection());

		context->UpdateSubresource(constBuffer, 0, nullptr, &lCB, 0, 0);
		context->VSSetConstantBuffers(0, 1, &constBuffer);
	}

	context->VSSetShader(vShader, nullptr, 0);
	context->PSSetShader(pShader, nullptr, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	context->DrawIndexed(NUM_POINTS, 0, 0);

	if (!partOfSplineBatch)
	{
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		if (prevLayout != splineLayout)
		{
			context->IASetInputLayout(prevLayout);
		}
	}
}