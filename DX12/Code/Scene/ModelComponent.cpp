#include "ModelComponent.h"
#include "Actor.h"
#include "Game.h"
#include "../GameObject/ObjModel.h"
ModelComponent::ModelComponent(Actor* actor, int drawOrder)
	:ObjectComponent(actor, drawOrder), mFileName(nullptr)
{
}

void ModelComponent::Update()
{
	SetObjModel();
}

void ModelComponent::SetObjModel()
{
	if (strlen(mFileName) != 0)
	{
		mObjectData.position = mOwner->GetPosition();
		mObjectData.rotation = mOwner->GetRotation();
		mObjectData.scale = mOwner->GetScale();
		mObjectData.color = mOwner->GetColor();
		ObjModel::GetInstance()->SetObjModelMetaData(mObjectData.entryPos, mObjectData.position, mObjectData.rotation, mObjectData.scale, mObjectData.color, mFileName);
	}
}

void ModelComponent::DeleteObjModel()
{
	ObjModel::GetInstance()->ObjModelDeleate(mObjectData.entryPos, mFileName);
}

std::vector<VECTOR> ModelComponent::GetModelVertex()
{
	return ObjModel::GetInstance()->GetVertex(mFileName);
}