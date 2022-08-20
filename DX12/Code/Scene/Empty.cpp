#include "Empty.h"
#include "Game.h"

#include "ModelComponent.h"
#include "BoxComponent.h"

Empty::Empty(Game* game)
	:Actor(game)
{
	mUse2DComponent.push_back(_T("ImageComponent"));
	mUse2DComponent.push_back(_T("GameManager"));
	mUse3DComponent.push_back(_T("PlayerInput"));
	mUse3DComponent.push_back(_T("MeshComponent"));
	mUse3DComponent.push_back(_T("ModelComponent"));	
	mUse3DComponent.push_back(_T("BoxCollider"));
	mUse3DComponent.push_back(_T("ModelMeshCollider"));
	mUse3DComponent.push_back(_T("RigidBody"));
	mUse3DComponent.push_back(_T("CapsuleCollider"));
}

Empty::~Empty()
{
}

void Empty::ActorInput()
{

}

void Empty::UpdateActor()
{

}
