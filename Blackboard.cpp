#include "Blackboard.h"

using namespace DirectX;
using std::vector;

Blackboard::Blackboard()
{
	playerPosition = Data<XMFLOAT3>(XMFLOAT3(0, 0, 0), 0, 0);
	agentLocations = vector<XMFLOAT3>(0);
	scaredAgents = vector<bool>(0);
}

Blackboard::Blackboard(unsigned int numAgents)
{
	playerPosition = Data<XMFLOAT3>(XMFLOAT3(0, 0, 0), 0, 0);
	agentLocations = vector<XMFLOAT3>(numAgents);
	scaredAgents = vector<bool>(numAgents);
	std::fill(scaredAgents.begin(), scaredAgents.end(), false);
}

void Blackboard::Update(double deltaTime)
{
	UpdateDataConfidence(deltaTime);
	UpdateSoundFalloff(deltaTime);
}

void Blackboard::UpdateSoundFalloff(double deltaTime)
{
	for (unsigned int i = 0; i < noises.size(); i++)
	{
		noises.at(i).soundFalloff(deltaTime);
		
		if (noises.at(i).volume <= 0)
		{
			noises.erase(noises.begin() + i);
			i--;
		}
	}
}

void Blackboard::UpdateDataConfidence(double deltaTime)
{
	playerPosition.Update(deltaTime);
}

vector<Sound*> Blackboard::getSoundsWithinRange(XMFLOAT3 agentPosition, float hearingRange)
{
	vector<Sound*> noisesInRange = vector<Sound*>();

	XMVECTOR agentPos = XMLoadFloat3(&agentPosition);

	for (unsigned int i = 0; i < noises.size(); i++)
	{
		XMVECTOR soundPos = XMLoadFloat3(&noises.at(i).position);

		if (XMVectorGetX(XMVector3LengthEst(soundPos - agentPos)) < hearingRange + noises.at(i).volume)
		{
			noisesInRange.push_back(&noises.at(i));
		}
	}

	return noisesInRange;
}

bool Blackboard::isAgentAlone(unsigned int agentIndex)
{
	const float aloneRadius = 3.0f;

	XMVECTOR examinedPos = XMLoadFloat3(&agentLocations.at(agentIndex));

	for (unsigned int i = 0; i < agentLocations.size(); i++)
	{
		if (i == agentIndex)
		{
			continue;
		}

		XMVECTOR otherPos = XMLoadFloat3(&agentLocations.at(i));

		if (XMVectorGetX(XMVector3LengthSq(otherPos - examinedPos)) < powf(aloneRadius, 2.0f))
		{
			return false;
		}
	}

	return true;
}

bool Blackboard::isAgentScared(unsigned int agentIndex)
{
	return scaredAgents.at(agentIndex);
}

void Blackboard::setAgentScaredState(unsigned int agentIndex, bool isScared)
{
	scaredAgents.at(agentIndex) = isScared;
}