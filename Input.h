#pragma once

#include <vector>

#include <string>
#include <fstream>
#include <sstream>

#include <Windows.h>

//A new event value needs to also be declared in the eventValueFromString() function. 
//This is so that when it is read from file it can be properly assigned.
enum Event
{
	WALK_FORWARDS,
	WALK_BACKWARDS,
	STRAFE_LEFT,
	STRAFE_RIGHT,
	YAW_LEFT,
	YAW_RIGHT,
	PITCH_UP,
	PITCH_DOWN,
	TOGGLE_WIREFRAME,
	TOGGLE_FLASHLIGHT,
	NO_SUCH_EVENT = -1,
};

struct KeyEvent
{
	char key;
	Event eventToFire;

	KeyEvent(char key, Event toFire) : key(key), eventToFire(toFire){};
};

class Input
{
private:
	std::vector<KeyEvent> usedKeys;

public:
	Input();
	Input(std::string keyMapFileName);
	void handleInput(void(*handleEvent)(Event e));

private:
	void initialiseKeys(std::string dataFile);
	Event eventValueFromString(std::string eventName);
};