#include "Connection.h"

Connection::Connection(Node* start, Node* end, int cost, ID3D11DeviceContext* context, ID3D11Device* device): start(start), end(end), cost(cost)
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
	spline = Spline(cp, context, device);
}

void Connection::Draw(ID3D11PixelShader* pShader, ID3D11VertexShader* vShader, Camera& cam)
{
	spline.Draw(pShader, vShader, cam);
}