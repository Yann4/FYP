#pragma once
#include <directxmath.h>
#include <DirectXCollision.h>
#include <vector>
#include <array>
#include <stack>
#include <algorithm>
#include <limits>

#define NOMINMAX

namespace Steering
{
	//Used in calculations for obstacle avoidance
	struct BBVertex
	{
		DirectX::XMFLOAT3 vertex;
		float distFromCollision;

		BBVertex()
		{
			vertex = DirectX::XMFLOAT3(0, 0, 0);
			distFromCollision = (std::numeric_limits<float>::max)();
		}

		BBVertex(DirectX::XMFLOAT3 position, float distance) : vertex(position), distFromCollision(distance)
		{}
	};

	//AggregateForces() will truncate to a maximum of this value
	constexpr float MaxForce = 0.0001f;

	//Returns the force seeking from the current position to the destination
	DirectX::XMFLOAT3 seekForce(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 destination);

	//Returns a force that weakens proportionally to the distance the position is from the destination, providerd itis within the radius
	DirectX::XMFLOAT3 arriveForce(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 destination, float radius);

	//Projects whiskers out from the position and uses them to generate a force steering away from obstacles
	DirectX::XMFLOAT3 obstacleAvoidForce(std::vector<DirectX::BoundingBox>& objects, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 forwards);

	//Returns a force moving towards a target, intended for use with a stack of targets
	//Modifies the stack reference passed in
	DirectX::XMFLOAT3 pathFollowing(DirectX::XMFLOAT3 position, std::stack<DirectX::XMFLOAT3>& path);

	//Aggregates steering forces to a single force using predetermined tweaker values
	DirectX::XMFLOAT3 aggregateForces(DirectX::XMFLOAT3 seek = DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3 arrive = DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3 obstacleAvoid = DirectX::XMFLOAT3(0, 0, 0));

	DirectX::XMFLOAT3 normalOfIntersection(DirectX::BoundingBox box, DirectX::XMFLOAT3 position);
};
