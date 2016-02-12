#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <directxmath.h>
#include <DirectXCollision.h>
#include <vector>
#include <algorithm>

#include "Node.h"

class Graph
{
private:
	std::vector<Node> graphNodes;

	bool graphUpToDate;

	ID3D11DeviceContext* context;
	ID3D11Device* device;
	ID3D11Buffer* constBuffer;
	ID3D11Buffer* objBuffer;
	MeshData* nodeMesh;

public:
	Graph();
	Graph(ID3D11DeviceContext* context, ID3D11Device* device, ID3D11Buffer* constBuffer, ID3D11Buffer* objBuffer, MeshData* mesh);

	void giveNode(Node newNode);
	void giveNode(DirectX::XMFLOAT3 position);

	void calculateGraph(std::vector<DirectX::BoundingBox> objects);

	//Node* getNearestNode(DirectX::XMFLOAT3 position);

	void DrawGraph(ID3D11PixelShader* ConnectionPShader, ID3D11VertexShader* ConnectionVShader, ID3D11PixelShader* NodePShader, ID3D11VertexShader* NodeVShader, Frustum& frustum, Camera& cam);

	inline bool needsRecalculating() { return !graphUpToDate; }
};