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

void Graph::calculateGraph(vector<BoundingBox>& objects)
{
	if (graphUpToDate)
	{
		return;
	}

	trimNodeList(objects);

	constexpr int searchRadius = 20;

	for (auto node : graphNodes)
	{
		node->clearConnections();
	}

	for (unsigned int i = 0; i < graphNodes.size(); i++)
	{
		for (unsigned int j = 0; j < graphNodes.size(); j++)
		{
			if (graphNodes.at(i)->distanceFrom(graphNodes.at(j)->Position()) > searchRadius)
			{
				continue;
			}

			graphNodes.at(i)->giveArc(*graphNodes.at(j), objects);
		}

		if (graphNodes.at(i)->getNeighbours().size() <= 2)
		{
			for (unsigned int j = 0; j < graphNodes.size(); j++)
			{
				graphNodes.at(i)->giveArc(*graphNodes.at(j), objects);
			}
		}
	}

	trimConnections();
	graphUpToDate = true;
}

void Graph::DrawGraph(ID3D11PixelShader* ConnectionPShader, ID3D11VertexShader* ConnectionVShader, ID3D11PixelShader* NodePShader, ID3D11VertexShader* NodeVShader, Frustum& frustum, Camera& cam)
{
	for (Node* n : graphNodes)
	{
		n->DrawNode(NodePShader, NodeVShader, frustum, cam);
	}

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	LineCBuffer lCB;
	lCB.world = XMMatrixIdentity();
	lCB.view = XMMatrixTranspose(cam.getView());
	lCB.projection = XMMatrixTranspose(cam.getProjection());

	context->UpdateSubresource(constBuffer, 0, nullptr, &lCB, 0, 0);
	context->VSSetConstantBuffers(0, 1, &constBuffer);

	ID3D11InputLayout* prevLayout = nullptr;
	context->IAGetInputLayout(&prevLayout);

	context->IASetInputLayout(splineInputLayout);
	for (Node* n : graphNodes)
	{
		n->DrawConnections(ConnectionPShader, ConnectionVShader, cam);
	}
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(prevLayout);
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

void Graph::trimNodeList(std::vector<DirectX::BoundingBox>& objects)
{
	float nearbyRadiusSq = pow(1.0f, 2);
	for (int i = 0; i < graphNodes.size(); i++)
	{
		bool nodeErased = false;
		XMFLOAT3 cPos = graphNodes.at(i)->Position();
		XMVECTOR checkedPos = XMLoadFloat3(&cPos);

		BoundingBox nodeBox = BoundingBox(cPos, XMFLOAT3(0.02f,0.02f,0.02f));

		//If the node is inside a gameObject, delete it because it can't be reached
		for (BoundingBox object : objects)
		{
			if (object.Intersects(nodeBox))
			{
				delete graphNodes[i];
				graphNodes.erase(graphNodes.begin() + i);
				nodeErased = true;
				i--;
				break;
			}
		}

		if (nodeErased)
		{
			continue;
		}

		for (int j = 0; j < graphNodes.size(); j++)
		{
			if (j == i)
			{
				continue;
			}
			XMFLOAT3 compPos = graphNodes.at(j)->Position();
			XMVECTOR comparePos = XMLoadFloat3(&compPos);

			XMVECTOR distSq = XMVector3LengthSq(comparePos - checkedPos);
			XMFLOAT3 dSq;
			XMStoreFloat3(&dSq, distSq);

			//Combine two close nodes into one node
			if (dSq.x < nearbyRadiusSq)
			{
				//Calculate new node location
				XMVECTOR avg = ((comparePos - checkedPos) / 2) + checkedPos;

				XMFLOAT3 newPos;
				XMStoreFloat3(&newPos, avg);

				//Create new node
				giveNode(newPos);

				//Remove old nodes
				delete graphNodes[i];
				delete graphNodes[j];
				
				graphNodes.erase(graphNodes.begin() + i);
				if (j > i)
				{
					graphNodes.erase(graphNodes.begin() + j - 1);
				}
				else
				{
					graphNodes.erase(graphNodes.begin() + j);
				}
				i = 0;
				break;
			}
		}
	}
}

void Graph::trimConnections()
{
	const float overlapRad = 0.75f;

	for (int i = 0; i < graphNodes.size(); i++)
	{
		vector<Connection*>* neighs = graphNodes.at(i)->getNeighboursRef();
		for (int j = 0; j < neighs->size(); j++)
		{
			XMFLOAT3 start, end;
			XMVECTOR sV, eV;

			start = neighs->at(j)->start->Position();
			end = neighs->at(j)->end->Position();
			sV = XMLoadFloat3(&start);
			eV = XMLoadFloat3(&end);

			for (int k = 0; k < graphNodes.size(); k++)
			{
				if (graphNodes.at(k) == neighs->at(j)->start || graphNodes.at(k) == neighs->at(j)->end)
				{
					continue;
				}
				XMFLOAT3 nPos = graphNodes.at(k)->Position();
				XMVECTOR n = XMLoadFloat3(&nPos);

				//Formula taken from http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html to get distance of a point from a line

				XMVECTOR numerator = XMVectorAbs(XMVector3Cross(n - sV, n - eV));
				XMVECTOR denominator = XMVectorAbs(eV - sV);
				XMFLOAT3 num, denom;
				XMStoreFloat3(&num, numerator);
				XMStoreFloat3(&denom, denominator);
				float dist = num.y / denom.z;

				if (dist <= overlapRad)
				{
					//Node is close to line
					neighs->at(j)->makeRed();
					//neighs->at(j)->end->removeConnectionTo(neighs->at(j)->start);
					//graphNodes.at(i)->removeNeighbourAt(j);
					break;
				}
			}
		}
	}
}

/*
PATHFINDING
*/
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