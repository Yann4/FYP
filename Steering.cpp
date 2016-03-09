#include "Steering.h"

using namespace DirectX;
using std::vector;
using std::array;

XMFLOAT3 Steering::seekForce(XMFLOAT3 position, XMFLOAT3 destination)
{
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR dest = XMLoadFloat3(&destination);

	XMVECTOR force = dest - pos;
	XMFLOAT3 steerForce;
	XMStoreFloat3(&steerForce, force);

	return steerForce;
}

XMFLOAT3 Steering::arriveForce(XMFLOAT3 position, XMFLOAT3 destination, float radius)
{
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR dest = XMLoadFloat3(&destination);
	
	XMVECTOR distSqV = XMVector3LengthSq(dest - pos);
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, distSqV);
	float distSq = temp.x;

	XMVECTOR force = dest - pos;

	if (distSq > pow(radius, 2))
	{
		XMFLOAT3 steerForce;
		XMStoreFloat3(&steerForce, force);

		return steerForce;
	}

	float dist = sqrtf(distSq);
	force *= (dist / radius);

	XMFLOAT3 steerForce;
	XMStoreFloat3(&steerForce, force);

	return steerForce;
}

XMFLOAT3 Steering::normalOfIntersection(BoundingBox box, XMFLOAT3 position, XMFLOAT3 direction)
{
	//Fetching corners of the bounding box so the normal can be calculated
	array<XMFLOAT3, 8> corners;
	box.GetCorners(&(corners[0]));

	//Point of intersection
	XMVECTOR poi = XMLoadFloat3(&position);

	array<BBVertex, 3> triangle;
	std::fill(triangle.begin(), triangle.end(), BBVertex());

	for (unsigned int j = 0; j < corners.size(); j++)
	{
		XMVECTOR corner = XMLoadFloat3(&corners.at(j));
		XMVECTOR dist = XMVector3LengthSq(poi - corner);
		XMFLOAT3 distFl;
		XMStoreFloat3(&distFl, dist);

		unsigned int furthestInd = 0;
		for (unsigned int i = 1; i < triangle.size(); i++)
		{
			if (triangle.at(i).distFromCollision > triangle.at(furthestInd).distFromCollision)
			{
				furthestInd = i;
			}
		}

		if (triangle.at(furthestInd).distFromCollision > distFl.x)
		{
			triangle.at(furthestInd).distFromCollision = distFl.x;
			triangle.at(furthestInd).vertex = corners.at(j);
		}
	}

	XMVECTOR a = XMLoadFloat3(&triangle.at(0).vertex);
	XMVECTOR b = XMLoadFloat3(&triangle.at(1).vertex);
	XMVECTOR c = XMLoadFloat3(&triangle.at(2).vertex);

	XMVECTOR normal = XMVector3Normalize(XMVector3Cross(a - b, a - c));
	XMFLOAT3 norm;
	XMStoreFloat3(&norm, normal);

	norm.x = -norm.x;
	norm.z = -norm.z;

	return norm;
}

XMFLOAT3 Steering::obstacleAvoidForce(vector<BoundingBox>& objects, XMFLOAT3 position, XMFLOAT3 forwards)
{
	XMFLOAT3 steeringForce = XMFLOAT3(0, 0, 0);

	constexpr float whiskerLength = 0.5f;
	constexpr float whiskerAngleDeg = 45.0f;
	float whiskerAngleRad = whiskerAngleDeg * (XM_PI / 180.0f);
	
	XMVECTOR pos = XMLoadFloat3(&position);

	XMFLOAT3 yAxis = XMFLOAT3(0, 1, 0);
	XMVECTOR rotationAxis = XMLoadFloat3(&yAxis);

	//Unit length whisker vectors
	XMVECTOR fWhisker = XMVector3Normalize(XMLoadFloat3(&forwards));
	XMVECTOR lWhisker = XMVector3Normalize(XMVector3Rotate(fWhisker, XMQuaternionRotationAxis(rotationAxis, whiskerAngleRad)));
	XMVECTOR rWhisker = XMVector3Normalize(XMVector3Rotate(fWhisker, XMQuaternionRotationAxis(rotationAxis, -whiskerAngleRad)));

	BoundingBox nearestBox = BoundingBox();
	float nearestBoxDistSq = (std::numeric_limits<float>::max)();

	for (BoundingBox box : objects)
	{
		XMVECTOR boxPos = XMLoadFloat3(&box.Center);

		/*
		Basically a broad-phase bounding circle collision detection.
		If a generous bounding circle is drawn around the bounding box, 
		and the whisker length is used as the other bounding circle and there
		is no collision, there won't be a whisker intersection, so no point in checking
		*/
		
		XMVECTOR dSq = XMVector3LengthSq(boxPos - pos);
		XMFLOAT3 distSq;
		XMStoreFloat3(&distSq, dSq);
		
		//No point in checking the y axis as the agents only operate on the xz plane
		float boxRadius = fmaxf(box.Extents.x, box.Extents.z);

		if (distSq.x > pow(whiskerLength + boxRadius, 2))
		{
			continue;
		}

		//We're only taking into account the nearest box
		//This could wind out being a problem
		if (distSq.x < nearestBoxDistSq)
		{
			nearestBoxDistSq = distSq.x;
			nearestBox = box;
		}
	}

	float dist = -1.0f;

	if (nearestBox.Intersects(pos, fWhisker, dist))
	{
		if (dist < whiskerLength)
		{
			//Gets the point on the surface of the bounding box
			XMVECTOR poi = pos + (dist * fWhisker);

			XMFLOAT3 pointOfIntersection, forwardWhisker;
			XMStoreFloat3(&pointOfIntersection, poi);
			XMStoreFloat3(&forwardWhisker, fWhisker);

			//Calculate force
			XMFLOAT3 normal = normalOfIntersection(nearestBox, pointOfIntersection, forwardWhisker);
			XMVECTOR norm = XMLoadFloat3(&normal);
			//norm *= dist;
			
			//Return force
			XMStoreFloat3(&steeringForce, norm);
			return steeringForce;
		}
	}

	if (nearestBox.Intersects(pos, lWhisker, dist))
	{
		if (dist < whiskerLength)
		{
			//Gets the point on the surface of the bounding box
			XMVECTOR poi = pos + (dist * lWhisker);

			XMFLOAT3 pointOfIntersection, leftWhisker;
			XMStoreFloat3(&leftWhisker, lWhisker);

			//Calculate force
			XMFLOAT3 normal = normalOfIntersection(nearestBox, pointOfIntersection, leftWhisker);
			XMVECTOR norm = XMLoadFloat3(&normal);
			//norm *= dist;

			//Return force
			XMStoreFloat3(&steeringForce, norm);
			return steeringForce;
		}
	}
	
	if (nearestBox.Intersects(pos, rWhisker, dist))
	{
		if (dist < whiskerLength)
		{
			//Gets the point on the surface of the bounding box
			XMVECTOR poi = pos + (dist * rWhisker);

			XMFLOAT3 pointOfIntersection, rightWhisker;
			XMStoreFloat3(&pointOfIntersection, poi);
			XMStoreFloat3(&rightWhisker, rWhisker);

			//Calculate force
			XMFLOAT3 normal = normalOfIntersection(nearestBox, pointOfIntersection, rightWhisker);
			XMVECTOR norm = XMLoadFloat3(&normal);
			//norm *= dist;

			//Return force
			XMStoreFloat3(&steeringForce, norm);
			return steeringForce;
		}
	}

	return steeringForce;
}

XMFLOAT3 Steering::aggregateForces(XMFLOAT3 seek, XMFLOAT3 arrive, XMFLOAT3 obstacleAvoid)
{
	float seekTweak = 0.5f;
	float arriveTweak = 0.7f;
	float oaTweak = 1.0f;

	XMVECTOR seekV = XMLoadFloat3(&seek);
	XMVECTOR arriveV = XMLoadFloat3(&arrive);
	XMVECTOR oaV = XMLoadFloat3(&obstacleAvoid);

	XMVECTOR resultant = (seekV * seekTweak) + (arriveV * arriveTweak) + (oaV * oaTweak);

	if (XMVectorGetX(XMVector3Length(resultant)) > MaxForce)
	{
		resultant = XMVector3Normalize(resultant) * MaxForce;
	}

	XMFLOAT3 force;
	XMStoreFloat3(&force, resultant);

	return force;
}