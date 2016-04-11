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
	std::vector<Connection*> neighbours;
	ID3D11DeviceContext* context;
	ID3D11Device* device;
	ID3D11InputLayout* splineInputLayout;
	GameObject object;
	unsigned int id;

	bool visited;

public:
	float g_score;
	float h_score;
	Node* parent;
public:
	Node();
	Node(DirectX::XMFLOAT3 position, ID3D11DeviceContext* context, ID3D11Device* device, ID3D11Buffer* constBuffer, ID3D11Buffer* objBuffer, MeshData* mesh, ID3D11InputLayout* splineLayout, unsigned int id);
	~Node();

	inline std::vector<Connection*> getNeighbours() { return neighbours; }
	inline DirectX::XMFLOAT3 Position() { return position; }
	float distanceFrom(DirectX::XMFLOAT3 pos);

	bool giveArc(Node& other, std::vector<DirectX::BoundingBox>& objects);
	void acceptArc(Connection* arc);

	void DrawNode(ID3D11PixelShader* NodePShader, ID3D11VertexShader* NodeVShader, Frustum& frustum, Camera& cam);
	void DrawConnections(ID3D11PixelShader* ConnectionPShader, ID3D11VertexShader* ConnectionVShader, Camera& cam);

	void clearConnections();

	inline float f_score() { return g_score + h_score; }

	inline unsigned int ID() { return id; }

	inline void resetSearchParams(){
		g_score = 0;
		h_score = 0;
		parent = nullptr;
	}

	inline void removeNeighbourAt(unsigned int index)
	{
		if (index < neighbours.size())
		{
			delete neighbours[index];
			neighbours.erase(neighbours.begin() + index);
		}
	}
	bool removeConnectionTo(Node* node);
	inline std::vector<Connection*>* getNeighboursRef() { return &neighbours; }

	inline void moveNode(DirectX::XMFLOAT3 moveTo) { 
		object.setTranslation(-position.x, -position.y, -position.z);
		object.setTranslation(moveTo.x, moveTo.y, moveTo.z);
		position = moveTo;
	}

	inline bool Visited() { return visited; }
	inline void setVisited(bool visitedState = true) { visited = visitedState; }
private:
	bool hasVisionOf(Node& other, std::vector<DirectX::BoundingBox>& objects);
};
