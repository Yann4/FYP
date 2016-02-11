#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <directxmath.h>
#include <DirectXCollision.h>
#include <vector>
#include "Connection.h"
#include "GameObject.h"

class Connection;
class Node
{
private:
	DirectX::XMFLOAT3 position;
	std::vector<Connection> neighbours;
	ID3D11DeviceContext* context;
	ID3D11Device* device;
	GameObject object;
public:
	Node();
	Node(DirectX::XMFLOAT3 position, ID3D11DeviceContext* context, ID3D11Device* device, ID3D11Buffer* constBuffer, ID3D11Buffer* objBuffer, MeshData* mesh);

	inline std::vector<Connection> getNeighbours() { return neighbours; }
	inline DirectX::XMFLOAT3 Position() { return position; }
	float distanceFrom(DirectX::XMFLOAT3 pos);

	bool giveArc(Node& other, std::vector<DirectX::BoundingBox>& objects);
	void acceptArc(Connection arc);

	void Draw(ID3D11PixelShader* ConnectionPShader, ID3D11VertexShader* ConnectionVShader, ID3D11PixelShader* NodePShader, ID3D11VertexShader* NodeVShader, Frustum& frustum, Camera& cam);

private:
	bool hasVisionOf(Node& other, std::vector<DirectX::BoundingBox>& objects);
};
