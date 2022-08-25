#include "BoxComponent.h"
#include "Game.h"
#include "Actor.h"
#include "PointLineComponent.h"
#include "../GameObject/GeometryData.h"
#include "../MATRIX.h"

BoxComponent::BoxComponent(Actor* owner, int order)
	:ColliderComponent(owner, order)
{

}