#pragma once

#include "Camera.h"
#include "Input.h"

#include <DirectXMath.h>
#include <algorithm>

class Player
{
private:	
	Camera camera;

	const float speed = 0.001f;
	const DirectX::XMFLOAT3 size = DirectX::XMFLOAT3(1, 2, 1);
public:
	Player();
	Player(DirectX::XMFLOAT3 position, float windowWidth, float windowHeight);

	void Update();
	void inputUpdate(std::vector<Event> events);

	void moveFromCollision(DirectX::XMFLOAT3 moveBy);

	Camera* getCamera() { return &camera; }

	inline DirectX::XMFLOAT3 Position() { DirectX::XMFLOAT4 p = camera.getPosition(); return DirectX::XMFLOAT3(p.x, p.y, p.z); }
	inline DirectX::XMFLOAT3 Size() { return size; }

	Player& operator=(Player other)
	{
		std::swap(camera, other.camera);
		return *this;
	}
};