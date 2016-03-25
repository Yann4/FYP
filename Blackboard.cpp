#include "Blackboard.h"

using namespace DirectX;

Blackboard::Blackboard()
{
	playerPosition = Data<XMFLOAT3>(XMFLOAT3(0, 0, 0), 0, 0);
}

void Blackboard::UpdateDataConfidence(double deltaTime)
{
	playerPosition.Update(deltaTime);
}