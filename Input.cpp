#include "Input.h"

using std::string;

Input::Input()
{
	usedKeys = std::vector<KeyEvent>();
}

Input::Input(string keyMapFileName)
{
	usedKeys = std::vector<KeyEvent>();
	initialiseKeys(keyMapFileName);
}

//Reads in keymap from file. The keymap has to be of a specific format
//i.e {CHAR} {EVENT} {TOGGLE}
//e.g W WALK_FORWARDS FALSE
//'#' is the comment line character.
void Input::initialiseKeys(string dataFile)
{
	std::ifstream file(dataFile);
	string line;

	string token1, token2, token3;

	std::istringstream iss;

	while (std::getline(file, line))
	{
		iss = std::istringstream(line);

		if (!(iss >> token1 >> token2 >> token3))
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
			bool toggle = false;
			if (token3 == "TRUE")
			{
				toggle = true;
			}

			usedKeys.push_back(KeyEvent(token1.at(0), tokenEvent, toggle));
			
		}
		
	}
}

//Helper function for the initialiseKeys() function
//The keymap file should have the Event enum name included with a character.
//This needs to be converted from being a string to being an Event value
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
	else if (eventName == "YAW_RIGHT")
	{
		return YAW_RIGHT;
	}
	else if (eventName == "YAW_LEFT")
	{
		return YAW_LEFT;
	}
	else if (eventName == "PITCH_UP")
	{
		return PITCH_UP;
	}
	else if (eventName == "PITCH_DOWN")
	{
		return PITCH_DOWN;
	}
	else if (eventName == "TOGGLE_WIREFRAME")
	{
		return TOGGLE_WIREFRAME;
	}
	else if (eventName == "TOGGLE_FLASHLIGHT")
	{
		return TOGGLE_FLASHLIGHT;
	}
	else
	{
		return NO_SUCH_EVENT;
	}
}

//The result of the input shouldn't be handled here, so here each event
//is passed to an eventHandler function higher up the hierarchy. This
//just abstracts the event itself and sends a signal to the appropriate function.
void Input::handleInput(void(*handleEvent)(Event e))
{
	for (KeyEvent e : usedKeys)
	{
		if (e.isToggle)
		{
			if (GetAsyncKeyState(e.key) & 0x0001)
			{
				handleEvent(e.eventToFire);
			}
		}
		else
		{
			if (GetAsyncKeyState(e.key) & 0x8000)
			{
				handleEvent(e.eventToFire);
			}
		}
	}
}