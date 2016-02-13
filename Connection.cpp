#include "Connection.h"

Connection::Connection(Node* start, Node* end, int cost, ID3D11DeviceContext* context, ID3D11Device* device, ID3D11InputLayout* splineLayout): start(start), end(end), cost(cost)
{
	std::vector<DirectX::XMFLOAT3> cp;
	if (start != nullptr)
	{
		cp.push_back(start->Position());
	}

	if (end != nullptr)
	{
		cp.push_back(end->Position());
	}
	spline = new Spline(cp, context, device, splineLayout);
}

Connection::~Connection()
{
	delete spline;
	start = nullptr;
	end = nullptr;
}

void Connection::Draw(ID3D11PixelShader* pShader, ID3D11VertexShader* vShader, Camera& cam)
{
	if (spline != nullptr)
	{
		spline->Draw(pShader, vShader, cam);
	}
}