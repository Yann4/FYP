#pragma once

#include <vector>

#include <string>
#include <fstream>
#include <sstream>

#include <Windows.h>

enum Event
{
	WALK_FORWARDS,
	WALK_BACKWARDS,
	STRAFE_LEFT,
	STRAFE_RIGHT,
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
	bool initialiseKeys(std::string dataFile);
	Event eventValueFromString(std::string eventName);
};