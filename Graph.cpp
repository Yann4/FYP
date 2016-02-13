#include "Graph.h"

using std::vector;
using namespace DirectX;

Graph::Graph() : context(nullptr), device(nullptr),
constBuffer(nullptr), objBuffer(nullptr), nodeMesh(nullptr), splineInputLayout(nullptr)
{
	graphNodes = vector<Node*>();
	graphUpToDate = true;
}

Graph::Graph(ID3D11DeviceContext* context, ID3D11Device* device, ID3D11Buffer* constBuffer, ID3D11Buffer* objBuffer, MeshData* mesh, ID3D11InputLayout* splineInputLayout): context(context), device(device),
constBuffer(constBuffer), objBuffer(objBuffer), nodeMesh(mesh), splineInputLayout(splineInputLayout)
{
	graphNodes = vector<Node*>();
	graphUpToDate = true;

	context->AddRef();
	device->AddRef();
	constBuffer->AddRef();
	objBuffer->AddRef();
	splineInputLayout->AddRef();
}

Graph::~Graph()
{
	if (context) context->Release();
	if (device) device->Release();
	if (constBuffer) constBuffer->Release();
	if (objBuffer) objBuffer->Release();
	nodeMesh = nullptr;

	for (unsigned int i = 0; i < graphNodes.size(); i++)
	{
		delete graphNodes[i];
	}
}

void Graph::giveNode(XMFLOAT3 position)
{
	graphNodes.push_back(new Node(position, context, device, constBuffer, objBuffer, nodeMesh, splineInputLayout));
	graphUpToDate = false;
}

void Graph::calculateGraph(vector<BoundingBox> objects)
{
	if (graphUpToDate)
	{
		return;
	}

	constexpr int searchRadius = 20;
	constexpr int maxNeighbours = 30;

	vector<int> checked;
	for (unsigned int i = 0; i < graphNodes.size(); i++)
	{
		graphNodes.at(i)->clearConnections();
		for (unsigned int j = 0; j < graphNodes.size(); j++)
		{
			checked.push_back(j);
			if (graphNodes.at(i)->distanceFrom(graphNodes.at(j)->Position()) > searchRadius)
			{
				continue;
			}

			graphNodes.at(i)->giveArc(*graphNodes.at(j), objects);

			if (graphNodes.at(i)->getNeighbours().size() >= maxNeighbours)
			{
				break;
			}
		}

		if (graphNodes.at(i)->getNeighbours().size() <= 2)
		{
			for (unsigned int j = 0; j < graphNodes.size(); j++)
			{
				if (std::find(checked.begin(), checked.end(), j) != checked.end())
				{
					graphNodes.at(i)->giveArc(*graphNodes.at(j), objects);
				}

				if (graphNodes.at(i)->getNeighbours().size() >= maxNeighbours)
				{
					break;
				}
			}
		}
		checked.clear();
	}

	graphUpToDate = true;
}

void Graph::DrawGraph(ID3D11PixelShader* ConnectionPShader, ID3D11VertexShader* ConnectionVShader, ID3D11PixelShader* NodePShader, ID3D11VertexShader* NodeVShader, Frustum& frustum, Camera& cam)
{
	for (Node* n : graphNodes)
	{
		n->Draw(ConnectionPShader, ConnectionVShader, NodePShader, NodeVShader, frustum, cam);
	}
}

Node* Graph::getNearestNode(XMFLOAT3 position)
{
	Node* nearest = nullptr;
	float shortestDist = D3D11_FLOAT32_MAX;
	
	for (unsigned int i = 0; i < graphNodes.size(); i++)
	{
		XMFLOAT3 nodePos = graphNodes.at(i)->Position();
		float lengthSq = pow(position.x - nodePos.x, 2) + pow(position.y - nodePos.y, 2) + pow(position.z - nodePos.z, 2);

		if (lengthSq < shortestDist)
		{
			shortestDist = lengthSq;
			nearest = graphNodes.at(i);
		}
	}

	return nearest;
}

//Euclidean heurstic
float Graph::heuristic(XMFLOAT3 start, XMFLOAT3 end)
{
	XMVECTOR sv, ev;
	sv = XMLoadFloat3(&start);
	ev = XMLoadFloat3(&end);

	XMFLOAT3 dist;
	XMStoreFloat3(&dist, XMVector3Length(sv - ev));

	return abs(dist.x);
}

std::stack<DirectX::XMFLOAT3> Graph::aStar(DirectX::XMFLOAT3 startPos, DirectX::XMFLOAT3 endPos)
{
	Node* start = getNearestNode(startPos);
	Node* end = getNearestNode(endPos);
	if (start == nullptr || end == nullptr)
	{
		return std::stack<XMFLOAT3>();
	}

	start->g_score = 0;
	start->h_score = heuristic(start->Position(), end->Position());

	Node* current = start;

	std::vector<Node*> open;
	std::vector<Node*> closed;

	open.push_back(current);

	while (!open.empty())
	{
		auto iterator = open.begin();
		float lowestF = D3D11_FLOAT32_MAX;
		for (auto iter = open.begin(); iter != open.end(); iter++)
		{
			if ((*iter)->f_score() < lowestF)
			{
				current = *iter;
				iterator = iter;
				lowestF = current->f_score();
			}
		}

		if (current->Position().x == end->Position().x && current->Position().y == end->Position().y && current->Position().z == end->Position().z)
		{
			break;
		}

		open.erase(iterator);
		closed.push_back(current);

		auto neighbours = current->getNeighbours();
		for (auto n : neighbours)
		{
			//If in closed set; continue
			bool inClosed = false;
			for (auto c : closed)
			{
				if (c->Position().x == n->end->Position().x && c->Position().y == n->end->Position().y && c->Position().z == n->end->Position().z)
				{
					inClosed = true;
					break;
				}
			}
			if (inClosed)
			{
				continue;
			}

			float tent_g = current->g_score + n->cost;

			//If not in the open set (new node)
			bool inOpen = false;
			int index = 0;
			for (unsigned int o = 0; o < open.size(); o++)
			{
				if (open.at(o)->Position().x == n->end->Position().x && open.at(o)->Position().y == n->end->Position().y && open.at(o)->Position().z == n->end->Position().z)
				{
					inOpen = true;
					index = o;
					break;
				}
			}
			if (!inOpen)
			{
				n->end->parent = current;
				n->end->g_score = tent_g;
				n->end->h_score = heuristic(n->end->Position(), end->Position());
				open.push_back(n->end);
				continue;
			}
			else if (tent_g >= open.at(index)->g_score)
			{
				continue;
			}

			open.at(index)->parent = current;
			open.at(index)->g_score = tent_g;
			open.at(index)->h_score = heuristic(open.at(index)->Position(), end->Position());
		}
	}

	std::stack<XMFLOAT3> path = std::stack<XMFLOAT3>();

	Node* temp = end;
	while (temp->parent != nullptr)
	{
		path.push(temp->Position());
		temp = temp->parent;
	}

	for (auto n : open)
	{
		n->resetSearchParams();
	}

	for (auto n : closed)
	{
		n->resetSearchParams();
	}

	return path;
}