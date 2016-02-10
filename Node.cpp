#include "Node.h"
using namespace DirectX;
using std::vector;

Node::Node()
{
	position = XMFLOAT3(0, 0, 0);
	neighbours = vector<Arc>();
}

Node::Node(XMFLOAT3 position) : position(position)
{
	neighbours = vector<Arc>();
}

bool Node::giveArc(Node& other, std::vector<BoundingBox>& objects)
{
	for (Arc a : neighbours)
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
	
	neighbours.push_back(Arc(this, &other, length.x));
	other.acceptArc(Arc(&other, this, length.x));

	return true;
}

void Node::acceptArc(Arc arc)
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