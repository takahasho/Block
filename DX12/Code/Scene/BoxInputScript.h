#pragma once
#include "MoveComponent.h"

class BoxInputComponent :
	public MoveComponent
{
public:
	BoxInputComponent(class Actor* owner);
	void ProcessInput() override;
};