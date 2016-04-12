#pragma once

#include "Observer.h"
#include "GameObject.h"
#include "Events.h"

#include <vector>
#include <DirectXMath.h>

class Subject
{
private:
	std::vector<Observer*> observers;

public:
	Subject()
	{
		observers = std::vector<Observer*>();
	}
	Subject(const Subject& other)
	{
		observers = other.observers;
	}
	
	inline void addObserver(Observer* observer)
	{
		observers.push_back(observer);
	}

	inline void removeObserver(Observer* observer)
	{
		auto iter = std::find(observers.begin(), observers.end(), observer);
		if (iter != observers.end())
		{
			observers.erase(iter);
		}
	}

	inline void notify(const DirectX::XMFLOAT3& entity, ActionEvent e)
	{
		for (unsigned int i = 0; i < observers.size(); i++)
		{
			observers.at(i)->onNotify(entity, e);
		}
	}
};
