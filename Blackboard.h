#pragma once

#include <DirectXMath.h>

template <typename T>
struct Data
{
	float confidence; //% Confidence in the info value
	float condifenceDepreciation; //Depreciation per second in confidence
	T info;
	
	Data() :confidence(0), condifenceDepreciation(0), T(T())
	{}

	Data(T info, int confidence = 100, int depreciation = 1) : confidence(confidence), condifenceDepreciation(depreciation), info(info)
	{}

	void Update(double deltaTime)
	{
		confidence -= condifenceDepreciation * (condifenceDepreciation * deltaTime);
	}
};

class Blackboard
{
private:
	Data<DirectX::XMFLOAT3> playerPosition;

public:
	Blackboard();

	void UpdateDataConfidence(double deltaTime);

	//Information relevant to player position
	inline void updatePlayerPosition(DirectX::XMFLOAT3 position, float confidence = 100.0f) {
		playerPosition.info = position; playerPosition.confidence = confidence;
	}

	inline Data<DirectX::XMFLOAT3> getPlayerPosition() {
		return playerPosition;
	};
};
