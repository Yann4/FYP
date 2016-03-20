#include "Steering.h"

using namespace DirectX;
using std::vector;
using std::array;
using std::stack;

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

	XMVECTOR ab = b - a;
	XMVECTOR ac = c - a;
	XMVECTOR bc = c - b;

	//Find hypotenuse of triangle - ab, ac or bc
	float abLenSq = XMVectorGetX(XMVector3LengthSq(ab));
	float acLenSq = XMVectorGetX(XMVector3LengthSq(ac));
	float bcLenSq = XMVectorGetX(XMVector3LengthSq(bc));
	
	XMVECTOR hypotenuse;
	XMVECTOR point;
	if (abLenSq > acLenSq)
	{
		if (abLenSq > bcLenSq)
		{
			//AB is hypotenuse
			point = a;
			hypotenuse = ab;
		}
		else
		{
			//BC is hypotenuse
			point = b;
			hypotenuse = bc;
		}
	}
	else if (acLenSq > bcLenSq)
	{
		//AC is hypotenuse
		point = a;
		hypotenuse = ac;
	}
	else
	{
		//BC is hypotenuse
		point = b;
		hypotenuse = bc;
	}

	//The midpoint of the face - the centre of the box will always give a vector pointing outside of the box
	XMVECTOR faceMidpoint = point + (hypotenuse / 2);
	XMVECTOR boxCentre = XMLoadFloat3(&box.Center);

	XMVECTOR normal = XMVector3Normalize(faceMidpoint - boxCentre);
	XMFLOAT3 norm;
	XMStoreFloat3(&norm, normal);

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
	XMVECTOR lWhisker = XMVector3Normalize(XMVector3Rotate(fWhisker, XMQuaternionRotationNormal(rotationAxis, whiskerAngleRad)));
	XMVECTOR rWhisker = XMVector3Normalize(XMVector3Rotate(fWhisker, XMQuaternionRotationNormal(rotationAxis, -whiskerAngleRad)));

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
		
		//No point in checking the y axis as the agents only operate on the xz plane
		float boxRadius = fmaxf(box.Extents.x, box.Extents.z);

		if (XMVectorGetX(dSq) > pow(whiskerLength + boxRadius, 2))
		{
			continue;
		}

		//We're only taking into account the nearest box
		//This could wind out being a problem
		if (XMVectorGetX(dSq) < nearestBoxDistSq)
		{
			nearestBoxDistSq = XMVectorGetX(dSq);
			nearestBox = box;
		}
	}

	float dist = -1.0f;
	float nearestDist = -1.0f;

	XMVECTOR intersectionWhisker;

	//Check intersections with all of the whiskers, but the whisker with the most imminent collision is the one that will be used
	if (nearestBox.Intersects(pos, fWhisker, dist))
	{
		if (dist < whiskerLength)
		{
			intersectionWhisker = fWhisker;
			nearestDist = dist;
		}
	}

	if (nearestBox.Intersects(pos, lWhisker, dist))
	{
		if (dist < whiskerLength)
		{
			if (dist < nearestDist)
			{
				intersectionWhisker = lWhisker;
				nearestDist = dist;
			}
			else if (nearestDist == -1.0f)
			{
				intersectionWhisker = lWhisker;
				nearestDist = dist;
			}
		}
	}

	if (nearestBox.Intersects(pos, rWhisker, dist))
	{
		if (dist < whiskerLength)
		{
			if (dist < nearestDist)
			{
				intersectionWhisker = lWhisker;
				nearestDist = dist;
			}
			else if (nearestDist == -1.0f)
			{
				intersectionWhisker = lWhisker;
				nearestDist = dist;
			}
		}
	}

	if (nearestDist != -1.0f)
	{
		//Gets the point on the surface of the bounding box
		XMVECTOR poi = pos + (nearestDist * intersectionWhisker);

		XMFLOAT3 pointOfIntersection, whisker;
		XMStoreFloat3(&pointOfIntersection, poi);
		XMStoreFloat3(&whisker, intersectionWhisker);

		//Calculate force
		XMFLOAT3 normal = normalOfIntersection(nearestBox, pointOfIntersection, whisker);
		XMVECTOR norm = XMLoadFloat3(&normal);

		//Scale the force proportionally to the distance from the intersection

		//Get 0-1 percentage scale factor
		float scale = (whiskerLength - nearestDist) / whiskerLength;

		//Do the scale
		const float scaleFactor = 3.0f;
		scale *= scaleFactor;
		norm = norm * scale;

		//Return force
		XMStoreFloat3(&steeringForce, norm);
		return steeringForce;
	}

	return steeringForce;
}

XMFLOAT3 Steering::pathFollowing(XMFLOAT3 position, stack<XMFLOAT3>& path)
{
	if (path.empty())
	{
		return XMFLOAT3(0, 0, 0);
	}

	XMVECTOR pos = XMLoadFloat3(&position);
	const float radius = 0.5f;

	XMFLOAT3 next = path.top();
	XMVECTOR target = XMLoadFloat3(&next);

	if (radius > XMVectorGetX(XMVector3Length(target - pos)))
	{
		path.pop();
		return pathFollowing(position, path);
	}

	return seekForce(position, next);
}

XMFLOAT3 Steering::aggregateForces(XMFLOAT3 seek, XMFLOAT3 arrive, XMFLOAT3 obstacleAvoid)
{
	float seekTweak = 0.7f;
	float arriveTweak = 0.7f;
	float oaTweak = 0.9f;

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