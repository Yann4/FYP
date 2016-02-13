#include "Collision.h"

bool Collision::testAxis(DirectX::XMFLOAT3 axis, float minA, float maxA, float minB, float maxB, MTV& mtv)
{
	using namespace DirectX;

	XMVECTOR axisV = XMLoadFloat3(&axis);
	float axisLSq = XMVectorGetX(XMVector3Dot(axisV, axisV));
	
	if (axisLSq < 1.0e-8f)
	{
		return true;
	}

	float d0 = maxB - minA; //Left side
	float d1 = maxA - minB; //Right side

	if (d0 <= 0.0f || d1 <= 0.0f)
	{
		return false;
	}

	float overlap = (d0 < d1) ? d0 : -d1; //Where is the overlap

	XMVECTOR separation = axisV * (overlap / axisLSq);

	float sepLSq = XMVectorGetX(XMVector3Dot(separation, separation));

	if (sepLSq < mtv.magnitude)
	{
		mtv.magnitude = sepLSq;
		XMStoreFloat3(&mtv.axis, separation);
	}

	return true;
}

DirectX::XMFLOAT3 rotateAxis(DirectX::XMFLOAT3 axis, DirectX::XMFLOAT3 rotateBy)
{
	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationX(rotateBy.x) * DirectX::XMMatrixRotationY(rotateBy.y) * DirectX::XMMatrixRotationZ(rotateBy.z);
	DirectX::XMVECTOR ax = DirectX::XMLoadFloat3(&axis);

	ax = DirectX::XMVector3Transform(ax, rotation);
	ax = DirectX::XMVector3Normalize(ax);
	DirectX::XMStoreFloat3(&axis, ax);
	return axis;
}

bool Collision::boundingBoxCollision(const AABB& a, const AABB& b, MTV& minTranslationVector)
{
	MTV mtv(DirectX::XMFLOAT3(0, 0, 0), D3D11_FLOAT32_MAX);

	DirectX::XMFLOAT3 x = DirectX::XMFLOAT3(1, 0, 0); //Check x axis
	DirectX::XMFLOAT3 y = DirectX::XMFLOAT3(0, 1, 0); //Check y axis
	DirectX::XMFLOAT3 z = DirectX::XMFLOAT3(0, 0, 1); //Check z axis	
	
	if (!testAxis(x, a.centre.x - a.size.x / 2, a.centre.x + a.size.x / 2, b.centre.x - b.size.x / 2, b.centre.x + b.size.x / 2, mtv))
	{
		return false;
	}
	
	if (!testAxis(y, a.centre.y - a.size.y / 2, a.centre.y + a.size.y / 2, b.centre.y - b.size.y / 2, b.centre.y + b.size.y / 2, mtv))
	{
		return false;
	}

	if (!testAxis(z, a.centre.z - a.size.z / 2, a.centre.z + a.size.z / 2, b.centre.z - b.size.z / 2, b.centre.z + b.size.z / 2, mtv))
	{
		return false;
	}
	
	DirectX::XMStoreFloat3(&mtv.axis, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&mtv.axis)));
	mtv.magnitude *= 1.001f; //Small multiplier to make it not look weird.
	minTranslationVector = mtv;
	return true;
}