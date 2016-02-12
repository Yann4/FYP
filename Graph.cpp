#include "Graph.h"

using std::vector;
using namespace DirectX;

Graph::Graph()
{
	graphNodes = vector<Node>();
	graphUpToDate = true;
}

Graph::Graph(ID3D11DeviceContext* context, ID3D11Device* device, ID3D11Buffer* constBuffer, ID3D11Buffer* objBuffer, MeshData* mesh): context(context), device(device), 
constBuffer(constBuffer), objBuffer(objBuffer), nodeMesh(mesh)
{
	graphNodes = vector<Node>();
	graphUpToDate = true;
}

void Graph::giveNode(Node newNode)
{
	graphNodes.push_back(newNode);
	graphUpToDate = false;
}

void Graph::giveNode(XMFLOAT3 position)
{
	graphNodes.push_back(Node(position, context, device, constBuffer, objBuffer, nodeMesh));
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
	for (int i = 0; i < graphNodes.size(); i++)
	{

		for (unsigned int j = 0; j < graphNodes.size(); j++)
		{
			checked.push_back(j);
			if (graphNodes.at(i).distanceFrom(graphNodes.at(j).Position()) > searchRadius)
			{
				continue;
			}

			graphNodes.at(i).giveArc(graphNodes.at(j), objects);

			if (graphNodes.at(i).getNeighbours().size() >= maxNeighbours)
			{
				break;
			}
		}

		if (graphNodes.at(i).getNeighbours().size() <= 2)
		{
			for (unsigned int j = 0; j < graphNodes.size(); j++)
			{
				if (std::find(checked.begin(), checked.end(), j) != checked.end())
				{
					graphNodes.at(i).giveArc(graphNodes.at(j), objects);
				}

				if (graphNodes.at(i).getNeighbours().size() >= maxNeighbours)
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
	for (Node n : graphNodes)
	{
		n.Draw(ConnectionPShader, ConnectionVShader, NodePShader, NodeVShader, frustum, cam);
	}
}