#include "Input.h"

using std::string;

Input::Input()
{
	usedKeys = std::vector<KeyEvent>();
}

Input::Input(string keyMapFileName)
{
	usedKeys = std::vector<KeyEvent>();

	if (!initialiseKeys(keyMapFileName))
	{
		exit(1);
	}
}

bool Input::initialiseKeys(string dataFile)
{
	std::ifstream file(dataFile);
	string line;

	string token1, token2;

	std::istringstream iss;

	while (std::getline(file, line))
	{
		iss = std::istringstream(line);

		if (!(iss >> token1 >> token2))
		{
			continue;
		}
		if (token1.at(0) == '#')
		{
			continue;
		}
		Event tokenEvent = eventValueFromString(token2);
		if (tokenEvent != NO_SUCH_EVENT)
		{
			usedKeys.push_back(KeyEvent(token1.at(0), tokenEvent));
		}
		
	}
	return true;
}

Event Input::eventValueFromString(string eventName)
{
	if (eventName == "WALK_FORWARDS")
	{
		return WALK_FORWARDS;
	}
	else if (eventName == "WALK_BACKWARDS")
	{
		return WALK_BACKWARDS;
	}
	else if (eventName == "STRAFE_LEFT")
	{
		return STRAFE_LEFT;
	}
	else if (eventName == "STRAFE_RIGHT")
	{
		return STRAFE_RIGHT;
	}
	else
	{
		return NO_SUCH_EVENT;
	}
}

void Input::handleInput(void(*handleEvent)(Event e))
{
	for (KeyEvent e : usedKeys)
	{
		if (GetAsyncKeyState(e.key) & 0x8000)
		{
			handleEvent(e.eventToFire);
		}
	}
}