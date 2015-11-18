#include "Frustum.h"

using std::array;
using namespace DirectX;

Frustum::Frustum()
{
	planes = array<XMFLOAT4, 6>();
}

Frustum::Frustum(const Frustum& other)
{

}

XMMATRIX Frustum::calculateProjection(float fovY, float aspectRatio, float znear, float zfar)
{
	float nearWindHeight = 2.0f * znear * tanf(0.5f * fovY);
	float farWindHeight = 2.0f * zfar * tanf(0.5f * fovY);

	XMMATRIX proj = XMMatrixPerspectiveFovLH(fovY, aspectRatio, znear, zfar);
	return proj;
}

void Frustum::constructFrustum(float screenDepth, float fov, float ar, float znear, float zfar, const XMMATRIX& viewMat)
{
	float zMin, r;
	XMFLOAT4X4 proj, matAsFloat;

	XMMATRIX matrix, projMat;

	XMStoreFloat4x4(&proj, calculateProjection(fov, ar, znear, zfar));
	

	zMin = -proj._43 / proj._33;
	r = screenDepth / (screenDepth - zMin);

	proj._33 = r;
	proj._43 = -r * zMin;

	projMat = XMLoadFloat4x4(&proj);
	matrix = XMMatrixMultiply(viewMat, projMat);
	XMStoreFloat4x4(&matAsFloat, matrix);

	//Near plane
	planes[0].x = matAsFloat._14 + matAsFloat._13;
	planes[0].y = matAsFloat._24 + matAsFloat._23;
	planes[0].z = matAsFloat._34 + matAsFloat._33;
	planes[0].w = matAsFloat._44 + matAsFloat._43;
	XMStoreFloat4(&planes[0], XMPlaneNormalize(XMLoadFloat4(&planes[0])));

	//Far plane
	planes[1].x = matAsFloat._14 - matAsFloat._13;
	planes[1].y = matAsFloat._24 - matAsFloat._23;
	planes[1].z = matAsFloat._34 - matAsFloat._33;
	planes[1].w = matAsFloat._44 - matAsFloat._43;
	XMStoreFloat4(&planes[1], XMPlaneNormalize(XMLoadFloat4(&planes[1])));

	//Left plane
	planes[2].x = matAsFloat._14 + matAsFloat._11;
	planes[2].y = matAsFloat._24 + matAsFloat._21;
	planes[2].z = matAsFloat._34 + matAsFloat._31;
	planes[2].w = matAsFloat._44 + matAsFloat._41;
	XMStoreFloat4(&planes[2], XMPlaneNormalize(XMLoadFloat4(&planes[2])));

	//Right plane
	planes[3].x = matAsFloat._14 - matAsFloat._11;
	planes[3].y = matAsFloat._24 - matAsFloat._21;
	planes[3].z = matAsFloat._34 - matAsFloat._31;
	planes[3].w = matAsFloat._44 - matAsFloat._41;
	XMStoreFloat4(&planes[3], XMPlaneNormalize(XMLoadFloat4(&planes[3])));

	//Far plane
	planes[4].x = matAsFloat._14 - matAsFloat._12;
	planes[4].y = matAsFloat._24 - matAsFloat._22;
	planes[4].z = matAsFloat._34 - matAsFloat._32;
	planes[4].w = matAsFloat._44 - matAsFloat._42;
	XMStoreFloat4(&planes[4], XMPlaneNormalize(XMLoadFloat4(&planes[4])));

	//Top plane
	planes[5].x = matAsFloat._14 + matAsFloat._12;
	planes[5].y = matAsFloat._24 + matAsFloat._22;
	planes[5].z = matAsFloat._34 + matAsFloat._32;
	planes[5].w = matAsFloat._44 + matAsFloat._42;
	XMStoreFloat4(&planes[5], XMPlaneNormalize(XMLoadFloat4(&planes[5])));
}

bool Frustum::checkSphere(XMFLOAT3 centre, float radius)
{
	for (int i = 0; i < 6; i++)
	{
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&planes[i]), XMLoadFloat3(&centre))) < -radius)
		{
			return false;
		}
	}
	return true;
}