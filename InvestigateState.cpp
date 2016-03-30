#include "InvestigateState.h"
using namespace DirectX;

InvestigateState::InvestigateState(Controller* owner, Blackboard* blackboard, std::stack<State*>* immediate, Graph* navGraph) : State(owner), blackboard(blackboard), immediate(immediate), navGraph(navGraph)
{
	disturbanceFound = false;
}

InvestigateState::InvestigateState(const InvestigateState& other)
{
	owner = other.owner;
	blackboard = other.blackboard;
	disturbanceLocation = other.disturbanceLocation;
	noises = other.noises;
	disturbanceFound = other.disturbanceFound;
	immediate = other.immediate;
	navGraph = other.navGraph;
}

InvestigateState::~InvestigateState()
{
	blackboard = nullptr;
}

void InvestigateState::Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects)
{
	if (noises.empty())
	{
		return;
	}

	if (noises.size() == 1)
	{
		disturbanceLocation = noises.at(0)->position;
		noises.at(0)->agentsInvestigating.push_back(owner->agentID);
		disturbanceFound = true;
	}
	else
	{
		//Get the noise closest to the last known player location, if that value has merit
		Data<XMFLOAT3> playerPos = blackboard->getPlayerPosition();
		if (playerPos.confidence > 0)
		{
			XMVECTOR playerPosition = XMLoadFloat3(&playerPos.info);
			XMFLOAT3 nearest = noises.at(0)->position;
			float nearestDist = D3D11_FLOAT32_MAX;
			unsigned int index;

			for (unsigned int i = 0; i < noises.size(); i++)
			{
				XMVECTOR nPos = XMLoadFloat3(&noises.at(i)->position);
				float dist = XMVectorGetX(XMVector3LengthEst(nPos - playerPosition));

				if (dist < nearestDist)
				{
					nearestDist = dist;
					nearest = noises.at(i)->position;
					index = i;
				}
			}

			disturbanceLocation = nearest;
			noises.at(index)->agentsInvestigating.push_back(owner->agentID);
			disturbanceFound = true;
		}
		else
		{
			//Otherwise, get the nearest to us

			XMVECTOR agentPosition = XMLoadFloat3(&owner->position);
			XMFLOAT3 nearest = noises.at(0)->position;
			float nearestDist = D3D11_FLOAT32_MAX;
			unsigned int index;

			for (unsigned int i = 0; i < noises.size(); i++)
			{
				XMVECTOR nPos = XMLoadFloat3(&noises.at(i)->position);
				float dist = XMVectorGetX(XMVector3LengthEst(nPos - agentPosition));

				if (dist < nearestDist)
				{
					nearestDist = dist;
					nearest = noises.at(i)->position;
					index = i;
				}
			}

			disturbanceLocation = nearest;
			disturbanceFound = true;
			noises.at(index)->agentsInvestigating.push_back(owner->agentID);
		}
	}
}

Priority InvestigateState::shouldEnter()
{
	noises = blackboard->getSoundsWithinRange(owner->position, hearingRange);
	
	if (!noises.empty())
	{
		for (unsigned int i = 0; i < noises.size(); i++)
		{
			for (unsigned int j = 0; j < noises.at(i)->agentsInvestigating.size(); j++)
			{
				if (noises.at(i)->agentsInvestigating.at(j) == owner->agentID)
				{
					return NONE;
				}
			}
		}

		return IMMEDIATE;
	}
	else
	{
		return NONE;
	}
}

bool InvestigateState::shouldExit()
{
	return disturbanceFound;
}

Priority InvestigateState::Exit(State** toPush)
{
	*toPush = new RouteToState(owner, immediate, navGraph, disturbanceLocation);
	return IMMEDIATE;
}