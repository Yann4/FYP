#pragma once

#include <windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

class Camera
{
private:
	//Orientation
	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT4 lookAt;
	DirectX::XMFLOAT4 up;
	DirectX::XMFLOAT4 right;

	float nearZ, farZ;
	float aspect;
	float yFov;
	float nearWindHeight, farWindHeight;

	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;

public:
	Camera();
	Camera(float foVY, float aspect, float znear, float zfar);

	void setLens(float foVY, float aspect, float znear, float zfar);
	void Update();

	void Strafe(float d), Walk(float d);
	void Pitch(float delta), Yaw(float delta);

	DirectX::XMMATRIX getView() const;
	DirectX::XMMATRIX getProjection() const;

private:
	void UpdateViewMatrix();
};