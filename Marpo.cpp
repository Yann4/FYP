#include "Marpo.h"

using std::stack;

Marpo::Marpo()
{
	longTerm = stack<State*>();
	immediate = stack<State*>();
	reactionary = stack<State*>();

	owner = nullptr;
}

void Marpo::Initialise(Controller* owner)
{
	this->owner = owner;
	immediate.push(new TravelToPositionState(owner, DirectX::XMFLOAT3(2, 1, 2)));
}

void Marpo::Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects)
{
	checkForStatesToPush();

	stack<State*>* currentStack = getTopStack();

	//If there's a State to run
	if (!currentStack->empty())
	{
		State* currentState = currentStack->top();

		//Run the update
		currentState->Update(deltaTime, objects);
		
		//Check if it's finished
		if (currentState->shouldExit())
		{
			//Some states return other states to be run.
			//Check if this is one of these, and update appropriately
			State* st = nullptr;
			Priority prio = currentState->Exit(st);
			
			delete currentState;
			currentStack->pop();

			pushWithPriority(st, prio);
		}
	}
}

void Marpo::checkForStatesToPush()
{
	for (unsigned int i = 0; i < states.size(); i++)
	{
		Priority prio = states.at(i)->shouldEnter();
		
		pushWithPriority(states.at(i), prio);
	}
}

void Marpo::pushWithPriority(State* state, Priority prio)
{
	if (prio != NONE)
	{
		switch (prio)
		{
		case LONG_TERM:
			longTerm.push(state);
			break;
		case IMMEDIATE:
			immediate.push(state);
			break;
		case REACTIONARY:
			reactionary.push(state);
			break;
		}
	}
}

stack<State*>* Marpo::getTopStack()
{
	if (!reactionary.empty())
	{
		return &reactionary;
	}

	if (!immediate.empty())
	{
		return &immediate;
	}

	return &longTerm;
}