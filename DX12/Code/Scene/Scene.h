#pragma once
#include "Actor.h"

class Scene :
    public Actor
{
public:
    Scene(class Game* gmae);
    ~Scene();
    class CameraComponent* GetCamera() { return mCamera; }
    class PointLineComponent* GetLine() { return mLine; }
private:

    class CameraComponent* mCamera;
    class StateComponent* mState;
    class PointLineComponent* mLine;
};