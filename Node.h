#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <directxmath.h>
#include <DirectXCollision.h>
#include <vector>

class Node
{
public:
	struct Arc
	{
		Node* start;
		Node* end;
		int cost;

		Arc() :start(nullptr), end(nullptr), cost(-1) {};
		Arc(Node* start, Node* end, int cost) : start(start), end(end), cost(cost) {};
	};

private:
	DirectX::XMFLOAT3 position;
	std::vector<Arc> neighbours;


public:
	Node();
	Node(DirectX::XMFLOAT3 position);

	inline std::vector<Arc> getNeighbours() { return neighbours; }
	inline DirectX::XMFLOAT3 Position() { return position; }
	float distanceFrom(XMFLOAT3 pos);

	bool giveArc(Node& other, std::vector<DirectX::BoundingBox>& objects);
	void acceptArc(Arc arc);

private:
	bool hasVisionOf(Node& other, std::vector<DirectX::BoundingBox>& objects);
};
