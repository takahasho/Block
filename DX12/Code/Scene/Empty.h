#pragma once
#include "Actor.h"

class Empty :
    public Actor
{
public:
    Empty(class Game* gmae);
    ~Empty();
    void ActorInput() override;
    void UpdateActor() override;

};

