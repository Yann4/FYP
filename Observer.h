#pragma once
#include "GameObject.h"
#include "Events.h"

#include <DirectXMath.h>
class Observer
{
public:
	virtual ~Observer() {};
	virtual void onNotify(const DirectX::XMFLOAT3& entity, ActionEvent e) = 0;
};
