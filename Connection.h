#pragma once
#include <Windows.h>
#include <d3d11.h>
#include "Node.h"
#include "Spline.h"

class Node;

class Connection
{
private:
	Spline spline;
public:
	Node* start;
	Node* end;
	int cost;

	Connection() :start(nullptr), end(nullptr), cost(-1) {};
	Connection(Node* start, Node* end, int cost, ID3D11DeviceContext* context, ID3D11Device* device);

	void Draw(ID3D11PixelShader* pShader, ID3D11VertexShader* vShader, Camera& cam);
};