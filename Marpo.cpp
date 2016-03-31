#include "Marpo.h"

using std::stack;

Marpo::Marpo()
{
	longTerm = stack<State*>();
	immediate = stack<State*>();
	reactionary = stack<State*>();

	owner = nullptr;
	navGraph = nullptr;
}

void Marpo::Initialise(Controller* ownerController, Graph* graph, Blackboard* bb)
{
	owner = ownerController;
	navGraph = graph;
	blackboard = bb;
}

void Marpo::Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects)
{
	checkForStatesToPush();

	stack<State*>* currentStack = getTopStack();

	//If there's a State to run
	if (!currentStack->empty())
	{
		State* currentState = currentStack->top();

		if (currentState != nullptr)
		{
			//Run the update
 			currentState->Update(deltaTime, objects);

			//Check if it's finished
			if (currentState->shouldExit())
			{
				//Some states return other states to be run.
				//Check if this is one of these, and update appropriately
				State* st = nullptr;
				Priority prio = currentState->Exit(&st);

				delete currentState;
				currentStack->pop();

				pushWithPriority(st, prio);
			}
		}
	}
}

void Marpo::checkForStatesToPush()
{
	Priority prio;

	//Checking ExploreState
	ExploreState es = ExploreState(owner, &longTerm, &immediate, navGraph);
	prio = es.shouldEnter();
	
	if (prio != NONE)
	{
		pushWithPriority(new ExploreState(owner, &longTerm, &immediate, navGraph), prio);
	}

	//Checking InvestigateState
	InvestigateState is = InvestigateState(owner, blackboard, &immediate, navGraph);
	prio = is.shouldEnter();
	
	if (prio != NONE)
	{
		pushWithPriority(new InvestigateState(is), prio);
	}

	//Checking HideState
	HideState hs = HideState(owner, blackboard, &immediate, navGraph);
	prio = hs.shouldEnter();

	if (prio != NONE)
	{
		pushWithPriority(new HideState(hs), prio);
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