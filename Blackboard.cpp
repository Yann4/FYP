#include "Blackboard.h"

using namespace DirectX;
using std::vector;

Blackboard::Blackboard()
{
	playerPosition = Data<XMFLOAT3>(XMFLOAT3(0, 0, 0), 0, 0);
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