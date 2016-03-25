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

	void Strafe(float d);
	void Walk(float d);
	void Pitch(float delta);
	void Yaw(float delta);

	DirectX::XMMATRIX getView() const;
	DirectX::XMMATRIX getProjection() const;

	inline DirectX::XMFLOAT4 getPosition() { return position; };
	inline void setPosition(DirectX::XMFLOAT4 pos) { position = pos; UpdateViewMatrix(); }

	inline float viewDistance() { return farZ - nearZ; };
	
	inline DirectX::XMFLOAT3 getForwards() { return DirectX::XMFLOAT3(lookAt.x, lookAt.y, lookAt.z); };
	inline float zNear(){ return nearZ; };
	inline float zFar(){ return farZ; };
	inline float aspectRatio(){ return aspect; };
	inline float fov(){ return yFov; };
	inline float nearHeight(){ return nearWindHeight; };
	inline float farHeight(){ return farWindHeight; };

private:
	void UpdateViewMatrix();
};