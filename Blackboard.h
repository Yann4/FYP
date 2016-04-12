#pragma once

#include "Subject.h"

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

	Data(T info, float confidence = 100.0f, float depreciation = 1.0f) : confidence(confidence), condifenceDepreciation(depreciation), info(info)
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
		volume -= falloffFactor * (float)deltaTime;
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
	std::vector<DirectX::XMFLOAT3> agentLocations;
	std::vector<bool> scaredAgents;
	bool playerTagged;

	DirectX::XMFLOAT3 exitLocation;
	std::vector<unsigned int> agentsGuardingExit;

	Subject subject;

public:
	Blackboard();
	Blackboard(const Blackboard& other);
	Blackboard(unsigned int numAgents);

	void Update(double deltaTime);

	//Information relevant to player position
	inline void updatePlayerPosition(DirectX::XMFLOAT3 position, float confidence = 100.0f) {
		playerPosition.info = position; playerPosition.confidence = confidence;
		if (confidence = 100.0f)
		{
			subject.notify(position, PLAYER_SEEN);
		}
	}

	inline Data<DirectX::XMFLOAT3> getPlayerPosition() {
		return playerPosition;
	};

	//Information relevant to noises
	inline void noiseMade(DirectX::XMFLOAT3 position, float volume) {
		noises.push_back(Sound(position, volume));
	}

	std::vector<Sound*> getSoundsWithinRange(DirectX::XMFLOAT3 agentPosition, float hearingRange);

	//Information relevant to agent location
	inline void setAgentPosition(unsigned int agentIndex, DirectX::XMFLOAT3 position){
		agentLocations.at(agentIndex) = position;
	}

	bool isAgentAlone(unsigned int agentIndex);

	//Gets a vector of the agent locations, without the one that is passed in, if it isn't -1
	std::vector<DirectX::XMFLOAT3> agentPositions(int discounting = -1);

	//Information relevant to scared state
	bool isAgentScared(unsigned int agentIndex);
	void setAgentScaredState(unsigned int agentIndex, bool isScared);

	//Information relevant to attacking the player
	inline bool hasPlayerBeenTagged() { return playerTagged; }
	inline void tagPlayer() { playerTagged = true; }

	//Information relevant to guarding the objective
	inline unsigned int numAgentsGuarding() { return agentsGuardingExit.size(); }
	inline bool isAgentGuarding(unsigned int agentIndex) { return std::find(agentsGuardingExit.begin(), agentsGuardingExit.end(), agentIndex) != agentsGuardingExit.end(); }
	void flipAgentGuarding(unsigned int agentIndex);

	inline void setExitLocation(DirectX::XMFLOAT3 exitLoc) { exitLocation = exitLoc; }
	inline DirectX::XMFLOAT3 getExitLocation() { return exitLocation; }

	inline void registerObserver(Observer* observer)
	{
		subject.addObserver(observer);
	}
	inline void deregisterObserver(Observer* observer) 
	{
		subject.removeObserver(observer); 
	}
private:
	void UpdateSoundFalloff(double deltaTime);
	void UpdateDataConfidence(double deltaTime);
};
