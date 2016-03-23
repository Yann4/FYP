#pragma once

enum Priority { LONG_TERM, IMMEDIATE, REACTIONARY, NONE };

#include "AgentController.h"
#include <DirectXCollision.h>
#include <vector>

class State
{
protected:
	Controller* owner;
public:
	State();
	State(Controller* owner);

	virtual ~State();

	virtual void Update(double deltaTime, std::vector<DirectX::BoundingBox>& objects);

	virtual Priority shouldEnter();
	virtual bool shouldExit();

	//Returns priority of state to be pushed if required
	//If NONE, `toPush` hasn't been altered
	virtual Priority Exit(State* toPush);
};
