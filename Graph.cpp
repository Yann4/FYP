#include "Graph.h"

using std::vector;
using std::find;
using namespace DirectX;

Graph::Graph() : context(nullptr), device(nullptr),
constBuffer(nullptr), objBuffer(nullptr), nodeMesh(nullptr), splineInputLayout(nullptr)
{
	graphNodes = vector<Node*>();
	graphUpToDate = true;
	colourConnectionsRed = false;
}

Graph::Graph(ID3D11DeviceContext* context, ID3D11Device* device, ID3D11Buffer* constBuffer, ID3D11Buffer* objBuffer, MeshData* mesh, ID3D11InputLayout* splineInputLayout): context(context), device(device),
constBuffer(constBuffer), objBuffer(objBuffer), nodeMesh(mesh), splineInputLayout(splineInputLayout)
{
	graphNodes = vector<Node*>();
	graphUpToDate = true;
	colourConnectionsRed = false;

	context->AddRef();
	device->AddRef();
	constBuffer->AddRef();
	objBuffer->AddRef();
	splineInputLayout->AddRef();

	top_id = 0;
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
	graphNodes.push_back(new Node(position, context, device, constBuffer, objBuffer, nodeMesh, splineInputLayout, top_id++));
	graphUpToDate = false;
}

void Graph::calculateGraph(vector<BoundingBox>& objects)
{
	if (graphUpToDate)
	{
		return;
	}

	//Remove/Consolidate as many nodes as possible
	trimNodeList(objects);

	constexpr int searchRadius = 10;

	//Remove all existing connections
	for (auto node : graphNodes)
	{
		node->clearConnections();
	}

	vector<unsigned int> nodesChecked;
	//For each node, a check against every other node needs to be performed
	for (unsigned int i = 0; i < graphNodes.size(); i++)
	{
		for (unsigned int j = 0; j < graphNodes.size(); j++)
		{
			nodesChecked.push_back(j);

			if (i == j)
			{
				continue;
			}

			//If the other node is outside of the initial search radius, continue
			if (graphNodes.at(i)->distanceFrom(graphNodes.at(j)->Position()) > searchRadius)
			{
				continue;
			}

			//If made it here, the view checks need to be performed.
			graphNodes.at(i)->giveArc(*graphNodes.at(j), objects);
		}

		//At minimum, the nodes will need to be connected to the other two corners of the box
		//and ideally one other node
		if (graphNodes.at(i)->getNeighbours().size() <= 3)
		{
			for (unsigned int j = 0; j < graphNodes.size(); j++)
			{
				//If we've already checked against this indexed node, don't check it again
				if (find(nodesChecked.begin(), nodesChecked.end(), j) != nodesChecked.end())
				{
					continue;
				}

				//Perform line of sight checks
				graphNodes.at(i)->giveArc(*graphNodes.at(j), objects);
			}
		}
		nodesChecked.clear();
	}

	trimConnections();
	graphUpToDate = true;
}

void Graph::trimNodeList(std::vector<DirectX::BoundingBox>& objects)
{
	float nearbyRadiusSq = pow(1.0f, 2);
	for (unsigned int i = 0; i < graphNodes.size(); i++)
	{
		bool nodeErased = false;
		XMFLOAT3 cPos = graphNodes.at(i)->Position();
		XMVECTOR checkedPos = XMLoadFloat3(&cPos);

		BoundingBox nodeBox = BoundingBox(cPos, XMFLOAT3(0.05f, 0.05f, 0.05f));

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

		for (unsigned int j = 0; j < graphNodes.size(); j++)
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

				//Move one node
				graphNodes.at(i)->moveNode(newPos);
				
				//Delete the other
				delete graphNodes[j];
				graphNodes.erase(graphNodes.begin() + j);

				i = 0;
				break;
			}
		}
	}
}

void Graph::trimConnections()
{
	const float overlapRad = 0.5f;

	//This is the maximum cost that a connection can have and
	//be overlooked in the trimming process. It's to allow short,
	//sensible arcs
	const float maxCostOfFreePass = 2.0f;

	//If a connection is this length or longer, there's probably a better path
	const float minCostOfInstantDel = 7.0f;

	for (unsigned int i = 0; i < graphNodes.size(); i++)
	{
		//If a connection comes within overlapRad distance of a node that it is not the connected node, delete the connection
		vector<Connection*>* neighs = graphNodes.at(i)->getNeighboursRef();
		for (unsigned int j = 0; j < neighs->size(); j++)
		{
			XMFLOAT3 start, end;
			XMVECTOR x1, x2;

			start = neighs->at(j)->start->Position();
			end = neighs->at(j)->end->Position();

			XMFLOAT2 s = XMFLOAT2(start.x, start.z);
			XMFLOAT2 e = XMFLOAT2(end.x, end.z);

			x1 = XMLoadFloat2(&s);
			x2 = XMLoadFloat2(&e);

			XMVECTOR denominator = XMVector3Length(x2 - x1);

			for (unsigned int k = 0; k < graphNodes.size(); k++)
			{
				if (graphNodes.at(k)->ID() == neighs->at(j)->start->ID() || graphNodes.at(k)->ID() == neighs->at(j)->end->ID())
				{
					continue;
				}

				XMFLOAT3 nPos = graphNodes.at(k)->Position();
				XMFLOAT2 node = XMFLOAT2(nPos.x, nPos.z);
				XMVECTOR p = XMLoadFloat2(&node);

				//Formula taken from http://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
				//to get the distance of a point from a line BETWEEN two points
				const float lenSq = XMVectorGetX(XMVector2LengthSq(x2 - x1));
				
				if (lenSq == 0.0f)
				{
					continue;
				}

				const float t = fmaxf(0.0f, fminf(1.0f, XMVectorGetX(XMVector2Dot(p - x1, x2 - x1)) / lenSq));
				XMVECTOR projection = x1 + t * (x2 - x1);
				float dist = XMVectorGetX(XMVector2Length(projection - p));

				if (dist < overlapRad)
				{
					//Node is close to line
					if (colourConnectionsRed)
					{
						neighs->at(j)->setColour(XMFLOAT3(1,0,0));
					}
					else
					{
						neighs->at(j)->end->removeConnectionTo(neighs->at(j)->start);
						graphNodes.at(i)->removeNeighbourAt(j);
					}
					break;
				}
			}
		}
	}
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

/*
PATHFINDING
*/

//Heuristics
float Graph::heuristic(XMFLOAT3 start, XMFLOAT3 end)
{
	return euclideanHeuristicSq(start, end);
}

float Graph::euclideanHeuristic(XMFLOAT3 start, XMFLOAT3 end)
{
	XMVECTOR sv, ev;
	sv = XMLoadFloat3(&start);
	ev = XMLoadFloat3(&end);

	XMFLOAT3 dist;
	XMStoreFloat3(&dist, XMVector3Length(sv - ev));

	return abs(dist.x);
}

float Graph::euclideanHeuristicSq(XMFLOAT3 start, XMFLOAT3 end)
{
	XMVECTOR sv, ev;
	sv = XMLoadFloat3(&start);
	ev = XMLoadFloat3(&end);

	XMFLOAT3 dist;
	XMStoreFloat3(&dist, XMVector3LengthSq(sv - ev));

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

	path.push(start->Position());

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