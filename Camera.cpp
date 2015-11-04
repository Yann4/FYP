#include "Camera.h"

using namespace DirectX;

Camera::Camera() : position(XMFLOAT4(0, 0, 0, 0)), lookAt(XMFLOAT4(0, 0, 0, 0)), right(XMFLOAT4(0, 0, 0, 0)), up(XMFLOAT4(0,0,0,0)),
nearZ(0.01f), farZ(10), aspect(0.5f), yFov(XM_PIDIV2), nearWindHeight(0), farWindHeight(0)
{
	XMStoreFloat4x4(&view, XMMatrixIdentity());
	setLens(yFov, aspect, nearZ, farZ);
}

Camera::Camera(float fovY, float aspect, float znear, float zfar) : position(XMFLOAT4(0, 0, -3, 0)), lookAt(XMFLOAT4(0, 0, 1, 0)), right(XMFLOAT4(1, 0, 0, 0)),
up(XMFLOAT4(0, 1, 0, 0))
{
	setLens(fovY, aspect, znear, zfar);
	UpdateViewMatrix();
}

//Sets up the projection matrix
void Camera::setLens(float fovY, float aspect, float znear, float zfar)
{
	nearZ = znear;
	farZ = zfar;
	this->aspect = aspect;
	yFov = fovY;

	nearWindHeight = 2.0f * nearZ * tanf(0.5f * yFov);
	farWindHeight = 2.0f * farZ * tanf(0.5f * yFov);

	XMMATRIX proj = XMMatrixPerspectiveFovLH(yFov, aspect, nearZ, farZ);
	XMStoreFloat4x4(&projection, proj);
}

#pragma region Movement
void Camera::Strafe(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat4(&right);
	XMVECTOR p = XMLoadFloat4(&position);
	XMStoreFloat4(&position, XMVectorMultiplyAdd(s, r, p));
}

void Camera::Walk(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat4(&lookAt);
	XMVECTOR p = XMLoadFloat4(&position);
	XMStoreFloat4(&position, XMVectorMultiplyAdd(s, l, p));
}

void Camera::Pitch(float delta)
{
	XMMATRIX r = XMMatrixRotationAxis(XMLoadFloat4(&right), delta);

	XMStoreFloat4(&up, XMVector3TransformNormal(XMLoadFloat4(&up), r));
	XMStoreFloat4(&lookAt, XMVector3TransformNormal(XMLoadFloat4(&lookAt), r));
}

void Camera::Yaw(float delta)
{
	XMMATRIX r = XMMatrixRotationY(delta);

	XMStoreFloat4(&right, XMVector3TransformNormal(XMLoadFloat4(&right), r));
	XMStoreFloat4(&up, XMVector3TransformNormal(XMLoadFloat4(&up), r));
	XMStoreFloat4(&lookAt, XMVector3TransformNormal(XMLoadFloat4(&lookAt), r));
}
#pragma endregion

//UpdateViewMatrix() should be left in, as it essentially flushes any other update logic done on the camera
//to the view matrix
void Camera::Update()
{
	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	XMVECTOR rightV = XMLoadFloat4(&right);
	XMVECTOR upV = XMLoadFloat4(&up);
	XMVECTOR lookV = XMLoadFloat4(&lookAt);
	XMVECTOR posV = XMLoadFloat4(&position);

	//Normalise right, up and look vectors
	lookV = XMVector4Normalize(lookV);
	upV = XMVector4Normalize(XMVector3Cross(lookV, rightV));
	rightV = XMVector3Cross(upV, lookV);

	float x = -XMVectorGetX(XMVector4Dot(posV, rightV));
	float y = -XMVectorGetX(XMVector4Dot(posV, upV));
	float z = -XMVectorGetX(XMVector4Dot(posV, lookV));

	XMStoreFloat4(&right, rightV);
	XMStoreFloat4(&up, upV);
	XMStoreFloat4(&lookAt, lookV);

	view(0, 0) = right.x;
	view(1, 0) = right.y;
	view(2, 0) = right.z;
	view(3, 0) = x;

	view(0, 1) = up.x;
	view(1, 1) = up.y;
	view(2, 1) = up.z;
	view(3, 1) = y;

	view(0, 2) = lookAt.x;
	view(1, 2) = lookAt.y;
	view(2, 2) = lookAt.z;
	view(3, 2) = z;

	view(0, 3) = 0.0f;
	view(1, 3) = 0.0f;
	view(2, 3) = 0.0f;
	view(3, 3) = 1.0f;
}

XMMATRIX Camera::getView() const
{
	XMMATRIX r = XMLoadFloat4x4(&view);
	return r;
}

XMMATRIX Camera::getProjection() const
{
	return XMLoadFloat4x4(&projection);
}