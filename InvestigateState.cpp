#include "InvestigateState.h"
using namespace DirectX;

InvestigateState::InvestigateState() : State()
{
	blackboard = nullptr;
	disturbanceLocation = XMFLOAT3(0, 0, 0);
	disturbanceFound = false;
	immediate = nullptr;
	navGraph = nullptr;
}

InvestigateState::InvestigateState(Controller* owner, Blackboard* blackboard, std::stack<State*>* immediate, Graph* navGraph) : State(owner), blackboard(blackboard), immediate(immediate), navGraph(navGraph)
{
	disturbanceFound = false;
}

InvestigateState::~InvestigateState()
{
	blackboard = nullptr;
}

void InvestigateState::Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects)
{
	/*if (!disturbanceFound)
	{
		getDisturbance();
	}*/
}

Priority InvestigateState::shouldEnter()
{
	/*std::vector<Sound*> noises = blackboard->getSoundsWithinRange(owner->position, hearingRange);
	
	for (unsigned int i = 0; i < noises.size(); i++)
	{
		if (std::find(noises.at(i)->agentsInvestigating.begin(), noises.at(i)->agentsInvestigating.end(), owner->agentID) != noises.at(i)->agentsInvestigating.end())
		{
			return NONE;
		}
	}

	if (!noises.empty())
	{
		getDisturbance();
		return IMMEDIATE;
	}

	return NONE;*/

	if (disturbanceFound)
	{
		return IMMEDIATE;
	}
	return NONE;
}

bool InvestigateState::shouldExit()
{
	return disturbanceFound;
}

Priority InvestigateState::Exit(State** toPush)
{
	*toPush = new RouteToState(owner, immediate, navGraph, blackboard, disturbanceLocation);
	return IMMEDIATE;
}

void InvestigateState::getDisturbance()
{
	std::vector<Sound*> noises = blackboard->getSoundsWithinRange(owner->position, hearingRange);

	if (noises.empty())
	{
		return;
	}

	std::random_device device;
	std::mt19937 engine(device());
	std::uniform_int_distribution<int> distr(0, noises.size() - 1);
	unsigned int index = distr(engine);

	disturbanceLocation = noises.at(index)->position;
	noises.at(index)->agentsInvestigating.push_back(owner->agentID);
	disturbanceFound = true;
}

void InvestigateState::onNotify(const DirectX::XMFLOAT3& entity, ActionEvent e)
{
	if (owner == nullptr)
	{
		return;
	}

	if (e == PLAYER_SEEN)
	{
		XMVECTOR noise = XMLoadFloat3(&entity);
		XMVECTOR pos = XMLoadFloat3(&owner->position);

		if (XMVectorGetX(XMVector3LengthEst(noise - pos)) <= hearingRange)
		{
			disturbanceLocation = entity;
			disturbanceFound = true;
		}
	}
}