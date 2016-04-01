#include "Player.h"

using namespace DirectX;

Player::Player()
{
	camera = Camera();
}

Player::Player(XMFLOAT3 position, float windowWidth, float windowHeight)
{
	camera = Camera(XM_PIDIV2, windowWidth / windowHeight, 0.00001f, 100.0f);
	camera.setPosition(XMFLOAT4(position.x, position.y, position.z, 1.0f));
}

void Player::inputUpdate(std::vector<Event> events, double deltaTime)
{
	XMFLOAT4 cameraPos = camera.getPosition();
	XMFLOAT3 position = XMFLOAT3(cameraPos.x, cameraPos.y, cameraPos.z);
	XMVECTOR pos = XMLoadFloat3(&position);

	//Gets forwards vector and flattens it to the XZ plane then renormalises it
	XMFLOAT3 fw = camera.getForwards();
	XMVECTOR forwards = XMLoadFloat3(&fw);
	forwards = XMVector3Normalize(forwards);
	forwards = XMVectorSetY(forwards, 0.0f);
	forwards = XMVector3Normalize(forwards);

	//Gets right vector and normalises it
	//Since it's based off the forwards vector it will already be in XZ
	XMVECTOR right = XMVector3Rotate(forwards, XMQuaternionRotationNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XM_PIDIV2));
	right = XMVector3Normalize(right);

	XMVECTOR vel = forwards * speed * deltaTime;
	XMFLOAT3 v;
	XMStoreFloat3(&v, XMVector3Length(vel));

	for (unsigned int i = 0; i < events.size(); i++)
	{
		switch (events.at(i))
		{
		case WALK_FORWARDS:
			pos += forwards * speed * deltaTime;
			break;
		case WALK_BACKWARDS:
			pos -= forwards * speed * deltaTime;
			break;
		case STRAFE_LEFT:
			pos -= right * speed * deltaTime;
			break;
		case STRAFE_RIGHT:
			pos += right * speed * deltaTime;
			break;
		}
	}

	XMStoreFloat4(&cameraPos, pos);
	camera.setPosition(cameraPos);
}

void Player::Update()
{
	camera.Update();
}

void Player::moveFromCollision(XMFLOAT3 moveBy)
{
	XMFLOAT4 position = camera.getPosition();
	position.x += moveBy.x;
	position.y += moveBy.y;
	position.z += moveBy.z;

	camera.setPosition(position);
}