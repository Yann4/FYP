#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <directxmath.h>
#include <DirectXCollision.h>
#include <vector>

#include "Node.h"

class Graph
{
private:
	std::vector<Node> graphNodes;

	bool graphUpToDate;

public:
	Graph();

	void giveNode(Node newNode);
	void giveNode(DirectX::XMFLOAT3 position);

	void calculateGraph(std::vector<DirectX::BoundingBox> objects);

	Node* getNearestNode(DirectX::XMFLOAT3 position);
};