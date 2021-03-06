#include "Node.h"
using namespace DirectX;
using std::vector;

Node::Node()
{
	position = XMFLOAT3(0, 0, 0);
	neighbours = vector<Connection*>();
	g_score = 0;
	h_score = 0;
	parent = nullptr;

	context = nullptr;
	device = nullptr;
	splineInputLayout = nullptr;
	id = 0;

	visited = false;
}

Node::Node(DirectX::XMFLOAT3 position, ID3D11DeviceContext* context, ID3D11Device* device, ID3D11Buffer* constBuffer, ID3D11Buffer* objBuffer, MeshData* mesh, ID3D11InputLayout* splineLayout, unsigned int id) :
	position(position), context(context), device(device), splineInputLayout(splineLayout), id(id)
{
	neighbours = vector<Connection*>();
	object = GameObject(context, constBuffer, objBuffer, mesh, position);
	object.setScale(0.1f, 0.1f, 0.1f);
	object.UpdateMatrix();
	g_score = 0;
	h_score = 0;
	parent = nullptr;

	context->AddRef();
	device->AddRef();
	splineInputLayout->AddRef();

	visited = false;
}

Node::~Node()
{
	clearConnections();

	if (context) context->Release();
	if (device) device->Release();
	if (splineInputLayout) splineInputLayout->Release();
}

bool Node::giveArc(Node& other, std::vector<BoundingBox>& objects)
{
	//If we are checking against this node, we shouldn't put an arc there
	if (other.ID() == id)
	{
		return false;
	}

	//If we already have a connection to that node, we should skip adding an extra connection
	for (Connection* a : neighbours)
	{
		if (a->end->ID() == other.ID())
		{
			return false;
		}
	}

	//If there isn't line of sight to the other node, there shouldn't be a connection
	if (!hasVisionOf(other, objects))
	{
		return false;
	}

	XMVECTOR myPos, otherPos;
	myPos = XMLoadFloat3(&position);
	XMFLOAT3 otherObjectPosition = other.Position();
	otherPos = XMLoadFloat3(&otherObjectPosition);
	
	XMVECTOR direction = otherPos - myPos;
	XMFLOAT3 length;
	XMStoreFloat3(&length, XMVector3Length(direction));
	
	//Hook the connection up both ways
	neighbours.push_back(new Connection(this, &other, (int)length.x, context, device, splineInputLayout));
	other.acceptArc(new Connection(&other, this, (int)length.x, context, device, splineInputLayout));

	return true;
}

void Node::acceptArc(Connection* arc)
{
	neighbours.push_back(arc);
}

bool Node::hasVisionOf(Node& other, std::vector<BoundingBox>& objects)
{
	XMVECTOR myPos = XMLoadFloat3(&position);

	for (unsigned int i = 0; i < objects.size(); i++)
	{
		XMFLOAT3 otherPosition = other.Position();
		XMVECTOR otherPos = XMLoadFloat3(&otherPosition);
	
		XMVECTOR direction = XMVector3Normalize(myPos - otherPos);
		
		if (XMVector3Equal(direction, XMVectorSet(0, 0, 0, 0)))
		{
			continue;
		}

		float len;

		if (objects.at(i).Intersects(otherPos, direction, len))
		{
			if (len < XMVectorGetX(XMVector3Length(myPos - otherPos)))
			{
				return false;
			}
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

bool Node::removeConnectionTo(Node* node)
{
	for (unsigned int i = 0; i < neighbours.size(); i++)
	{
		if (node->Position().x == neighbours.at(i)->end->Position().x && node->Position().y == neighbours.at(i)->end->Position().y && node->Position().z == neighbours.at(i)->end->Position().z)
		{
			removeNeighbourAt(i);
			return true;
		}
	}
	return false;
}

void Node::DrawNode(ID3D11PixelShader* NodePShader, ID3D11VertexShader* NodeVShader, Frustum& frustum, Camera& cam)
{
	object.Draw(NodePShader, NodeVShader, frustum, cam);
}

void Node::DrawConnections(ID3D11PixelShader* ConnectionPShader, ID3D11VertexShader* ConnectionVShader, Camera& cam)
{
	for (Connection* c : neighbours)
	{
		c->Draw(ConnectionPShader, ConnectionVShader, cam);
	}
}

void Node::clearConnections()
{
	for (unsigned int i = 0; i < neighbours.size(); i++)
	{
		delete neighbours[i];
	}
	neighbours.clear();
}