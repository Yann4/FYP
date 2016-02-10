#include "Graph.h"

using std::vector;
using namespace DirectX;

Graph::Graph()
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
	graphNodes.push_back(Node(position));
	graphUpToDate = false;
}

void Graph::calculateGraph(vector<BoundingBox> objects)
{
	if (graphUpToDate)
	{
		return;
	}

	constexpr int searchRadius = 50;

	for (int i = 0; i < graphNodes.size(); i++)
	{
		for (int j = i + 1; j < graphNodes.size(); j++)
		{
			if (graphNodes.at(i).distanceFrom(graphNodes.at(j).Position()) > searchRadius)
			{
				continue;
			}

			graphNodes.at(i).giveArc(graphNodes.at(j), objects);
		}
	}

	graphUpToDate = true;
}