#include "Node.h"
using namespace DirectX;
using std::vector;

Node::Node()
{
	position = XMFLOAT3(0, 0, 0);
	neighbours = vector<Connection>();
	g_score = 0;
	h_score = 0;
	parent = nullptr;
}

Node::Node(DirectX::XMFLOAT3 position, ID3D11DeviceContext* context, ID3D11Device* device, ID3D11Buffer* constBuffer, ID3D11Buffer* objBuffer, MeshData* mesh) : position(position), context(context), device(device)
{
	neighbours = vector<Connection>();
	object = GameObject(context, constBuffer, objBuffer, mesh, position);
	object.setScale(0.1f, 0.1f, 0.1f);
	object.UpdateMatrix();
	g_score = 0;
	h_score = 0;
	parent = nullptr;
}

bool Node::giveArc(Node& other, std::vector<BoundingBox>& objects)
{
	for (Connection a : neighbours)
	{
		if (a.end == &other)
		{
			return false;
		}
	}

	if (!hasVisionOf(other, objects))
	{
		return false;
	}

	XMVECTOR myPos, otherPos;
	myPos = XMLoadFloat3(&position);
	otherPos = XMLoadFloat3(&other.Position());
	
	XMVECTOR direction = otherPos - myPos;
	XMFLOAT3 length;
	XMStoreFloat3(&length, XMVector3Length(direction));
	
	neighbours.push_back(Connection(this, &other, length.x, context, device));
	other.acceptArc(Connection(&other, this, length.x, context, device));

	return true;
}

void Node::acceptArc(Connection arc)
{
	neighbours.push_back(arc);
}

bool Node::hasVisionOf(Node& other, std::vector<BoundingBox>& objects)
{
	XMVECTOR myPos = XMLoadFloat3(&position);

	for (BoundingBox box : objects)
	{
		XMVECTOR otherPos = XMLoadFloat3(&other.Position());
	
		XMVECTOR direction = otherPos - myPos;
		XMFLOAT3 length;
		XMStoreFloat3(&length, XMVector3Length(direction));

		if (box.Intersects(myPos, direction, length.x))
		{
			return false;
		}
	}

	return true;
}

float Node::distanceFrom(XMFLOAT3 pos)
{
	XMVECTOR myPos, otherPos;
	myPos = XMLoadFloat3(&position);
	otherPos = XMLoadFloat3(&pos);

	XMVECTOR direction = otherPos - myPos;
	XMFLOAT3 length;
	XMStoreFloat3(&length, XMVector3Length(direction));
	
	return length.x;
}

void Node::Draw(ID3D11PixelShader* ConnectionPShader, ID3D11VertexShader* ConnectionVShader, ID3D11PixelShader* NodePShader, ID3D11VertexShader* NodeVShader, Frustum& frustum, Camera& cam)
{
	for (Connection c : neighbours)
	{
		c.Draw(ConnectionPShader, ConnectionVShader, cam);
	}

	object.Draw(NodePShader, NodeVShader, frustum, cam);
}

void Node::clearConnections()
{
	neighbours.clear();
}