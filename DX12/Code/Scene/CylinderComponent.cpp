#include "CylinderComponent.h"
#include "Actor.h"
#include <DirectXMath.h>
#include "PointLineComponent.h"
#include "../MATRIX.h"

CylinderComponent::CylinderComponent(Actor* owner, int order)
	:ColliderComponent(owner, order)
{
}