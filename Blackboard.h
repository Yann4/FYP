#pragma once

#include <DirectXMath.h>
#include <vector>

template <typename T>
struct Data
{
	float confidence; //% Confidence in the info value
	float condifenceDepreciation; //Depreciation per second in confidence
	T info;
	
	Data() :confidence(0), condifenceDepreciation(0)
	{}

	Data(T info, int confidence = 100, int depreciation = 1) : confidence(confidence), condifenceDepreciation(depreciation), info(info)
	{}

	void Update(double deltaTime)
	{
		confidence -= condifenceDepreciation * (condifenceDepreciation * deltaTime);
	}
};

struct Sound
{
	DirectX::XMFLOAT3 position;
	float volume; // Volume is the number of units the sound can travel
	std::vector<unsigned int> agentsInvestigating;

	Sound() : position(DirectX::XMFLOAT3(0, 0, 0)), volume(0) {}
	Sound(DirectX::XMFLOAT3 position, float volume) : position(position), volume(volume) {}
	
	//Returns true if sound is now silent, false otherwise
	bool soundFalloff(double deltaTime)
	{
		float falloffFactor = 1.0f;
		volume -= falloffFactor * deltaTime;
		if (volume >= 0)
		{
			return false;
		}

		return true;
	}
};

class Blackboard
{
private:
	Data<DirectX::XMFLOAT3> playerPosition;
	std::vector<Sound> noises;

public:
	Blackboard();

	void Update(double deltaTime);

	//Information relevant to player position
	inline void updatePlayerPosition(DirectX::XMFLOAT3 position, float confidence = 100.0f) {
		playerPosition.info = position; playerPosition.confidence = confidence;
	}

	inline Data<DirectX::XMFLOAT3> getPlayerPosition() {
		return playerPosition;
	};

	//Information relevant to noises
	inline void noiseMade(DirectX::XMFLOAT3 position, float volume) {
		noises.push_back(Sound(position, volume));
	}

	std::vector<Sound*> getSoundsWithinRange(DirectX::XMFLOAT3 agentPosition, float hearingRange);

private:
	void UpdateSoundFalloff(double deltaTime);
	void UpdateDataConfidence(double deltaTime);
};
