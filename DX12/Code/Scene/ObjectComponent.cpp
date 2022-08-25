#include "ObjectComponent.h"
#include "Actor.h"
#include "Game.h"
#include "../GameObject/Mesh.h"
ObjectComponent::ObjectComponent(Actor* actor, int drawOrder)
	:Component(actor)
	, mObjectData()
	, mDrawOrder(drawOrder)
	, mObjectType(ObjectType::TypeNone)
{
}
// デストラクター
ObjectComponent::~ObjectComponent()
{
	
}

void ObjectComponent::Update()
{
	SetObject();
}

void ObjectComponent::SetObject()
{
	if (mObjectType != ObjectType::TypeNone)
	{
		mObjectData.position = mOwner->GetPosition();
		mObjectData.rotation = mOwner->GetRotation();
		mObjectData.scale = mOwner->GetScale();
		mObjectData.color = mOwner->GetColor();
		if (mObjectType == ObjectType::Box)
		{
			Mesh::GetInstance()->SetBoxMetaData(mObjectData.entryPos, mObjectData.position, mObjectData.rotation, mObjectData.scale, mObjectData.color);
		}
		else if (mObjectType == ObjectType::Cylinder)
		{
			Mesh::GetInstance()->SetCylinderMetaData(mObjectData.entryPos, mObjectData.position, mObjectData.rotation, mObjectData.scale, mObjectData.color);
		}
		else if (mObjectType == ObjectType::Sphere)
		{
			Mesh::GetInstance()->SetSphereMetaData(mObjectData.entryPos, mObjectData.position, mObjectData.rotation, mObjectData.scale, mObjectData.color);
		}
	}
}

void ObjectComponent::DeleteObject()
{
	if (mObjectType == ObjectType::Box)
	{
		Mesh::GetInstance()->BoxDeleate(mObjectData.entryPos);
	}
	else if (mObjectType == ObjectType::Cylinder)
	{
		Mesh::GetInstance()->CylinderDeleate(mObjectData.entryPos);
	}
	else if (mObjectType == ObjectType::Sphere)
	{
		Mesh::GetInstance()->SphereDeleate(mObjectData.entryPos);
	}
}