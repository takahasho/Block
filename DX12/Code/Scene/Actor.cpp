#include "Actor.h"
#include "Game.h"
#include "PlayerInputComponent.h"
#include "ModelComponent.h"
#include "BoxComponent.h"
#include "CylinderComponent.h"
#include "ModelMeshComponent.h"
#include "RigidBodyComponent.h"
#include "BoxInputScript.h"
#include "CapsuleComponent.h"
#include "ImageComponent.h"

Actor::Actor(Game* game)
	: mGame(game)
	, mObj()
	, mBox()
	, mModel()
	, mCylinder()
	, mCapsule()
	, mModelMesh()
	, mPlayerInput()
	, mBoxInput()
	, mRigidBody()
	, mImage()
	, mManager()
	, mState(ActorState::EActive)
	, mRotation(VECTOR(0.0f, 0.0f, 0.0f))
	, mScale(VECTOR(1.0f, 1.0f, 1.0f))
	, mColor(VECTOR(1.0f, 1.0f, 1.0f))
	, mAlpha(1)
	, mColliderType(ColliderType::TypeNone)
{
	mGame->AddActor(this);
}

Actor::~Actor()
{
	mGame->RemoveActor(this);

	while (!mComponents.empty())
	{
		delete mComponents.back();
	}
}

void Actor::ProcessInput()
{
	if (mState == ActorState::EActive)
	{
		for (auto comp : mComponents)
		{
			comp->ProcessInput();
		}

		ActorInput();
	}
}

void Actor::Update()
{
	if (mState == ActorState::EActive)
	{
		for (auto component : mComponents)
		{
			component->Update();
		}
		UpdateActor();
	}
}

void Actor::AddComponent(Component* component)
{
	// ソート済み配列の挿入場所を探す
	// (自分より順番の大きい最初の要素を探す)
	int myOrder = component->GetUpdateOrder();
	auto iter = mComponents.begin();
	for (; iter != mComponents.end(); ++iter)
	{
		if (myOrder < (*iter)->GetUpdateOrder())
		{
			break;
		}
	}
	
	// 探し出した要素の前に自分を挿入
	mComponents.insert(iter, component);
}

void Actor::RemoveComponent(Component* component)
{
	auto iter = std::find(mComponents.begin(), mComponents.end(), component);
	if (iter != mComponents.end())
	{
		mComponents.erase(iter);
	}
}

std::vector<std::wstring> Actor::GetUse2DComponent()
{ 
	return mUse2DComponent; 
}

std::vector<std::wstring> Actor::GetUse3DComponent()
{
	return mUse3DComponent;
}

// ObjectCreateクラスで使うゲッター/セッター
const Component* Actor::GetComponent(std::wstring componentName) const 
{
	if (componentName._Equal(_T("PlayerInput")))
	{
		return (Component*)mPlayerInput;
	}
	else if (componentName._Equal(_T("MeshComponent")))
	{
		return (Component*)mObj;
	}
	else if (componentName._Equal(_T("ModelComponent")))
	{
		return (Component*)mModel;
	}
	else if (componentName._Equal(_T("BoxCollider")))
	{
		return (Component*)mBox;
	}
	else if (componentName._Equal(_T("CylinderCollider")))
	{
		return (Component*)mCylinder;
	}
	else if (componentName._Equal(_T("CapsuleCollider")))
	{
		return (Component*)mCapsule;
	}
	else if (componentName._Equal(_T("ModelMeshCollider")))
	{
		return (Component*)mModelMesh;
	}
	else if (componentName._Equal(_T("RigidBody")))
	{
		return (Component*)mRigidBody;
	}
	else if (componentName._Equal(_T("ImageComponent")))
	{
		if ((int)mImage.size() != 0)
		{
			return (Component*)mImage[0];
		}
	}
	else if (componentName._Equal(_T("BoxInput")))
	{
		return (Component*)mBoxInput;
	}
	else if (componentName._Equal(_T("GameManager")))
	{
		return (Component*)mManager;
	}
	
	return nullptr;
}

void Actor::SetComponent(std::wstring componentName)
{
	if (componentName._Equal(_T("PlayerInput")))
	{
		if (!mPlayerInput)
		{
			mPlayerInput = new PlayerInputComponent(this);
		}
	}
	else if (componentName._Equal(_T("MeshComponent")))
	{
		if (!mObj)
		{
			mObj = new ObjectComponent(this);
		}
	}
	else if (componentName._Equal(_T("ModelComponent")))
	{
		if (!mModel)
		{
			mModel = new ModelComponent(this);
		}
	}
	else if (componentName._Equal(_T("BoxCollider")))
	{
		if (!mBox)
		{
			mBox = new BoxComponent(this);
		}
	}
	else if (componentName._Equal(_T("CylinderCollider")))
	{
		if (!mCylinder)
		{
			mCylinder = new CylinderComponent(this);
		}
	}
	else if (componentName._Equal(_T("CapsuleCollider")))
	{
		if (!mCapsule)
		{
			mCapsule = new CapsuleComponent(this);
		}
	}
	else if (componentName._Equal(_T("ModelMeshCollider")))
	{
		if (!mModelMesh && mModel)
		{
			mModelMesh = new ModelMeshComponent(this);
		}
	}
	else if (componentName._Equal(_T("RigidBody")))
	{
		if (!mRigidBody)
		{
			mRigidBody = new RigidBodyComponent(this);
		}
	}
	else if (componentName._Equal(_T("ImageComponent")))
	{
		mImage.push_back(new ImageComponent(this));		
	}
	else if (componentName._Equal(_T("BoxInput")))
	{
		if (!mBoxInput)
		{
			mBoxInput = new BoxInputComponent(this);
		}
	}
}

void Actor::DeleateComponent(std::wstring componentName, int num)
{

	if (componentName._Equal(_T("PlayerInput")))
	{
		if (mPlayerInput)
		{
			delete mPlayerInput;
			mPlayerInput = NULL;
		}
	}
	else if (componentName._Equal(_T("MeshComponent")))
	{
		if (mObj)
		{
			mObj->DeleteObject();
			delete mObj;
			mObj = NULL;
		}
	}
	else if (componentName._Equal(_T("ModelComponent")))
	{
		if (mModel)
		{
			mModel->DeleteObjModel();
			delete mModel;
			mModel = NULL;
		}
	}
	else if (componentName._Equal(_T("BoxCollider")))
	{
		if (mBox)
		{
			delete mBox;
			mBox = NULL;
		}
	}
	else if (componentName._Equal(_T("CylinderCollider")))
	{
		if (mCylinder)
		{
			delete mCylinder;
			mCylinder = NULL;
		}
	}
	else if (componentName._Equal(_T("CapsuleCollider")))
	{
		if (mCapsule)
		{
			delete mCapsule;
			mCapsule = NULL;
		}
	}
	else if (componentName._Equal(_T("ModelMeshCollider")))
	{
		if (mModelMesh)
		{
			delete mModelMesh;
			mModelMesh = NULL;
		}
	}
	else if (componentName._Equal(_T("RigidBody")))
	{
		if (mRigidBody)
		{
			delete mRigidBody;
			mRigidBody = NULL;
		}
	}
	else if (componentName._Equal(_T("ImageComponent")))
	{
		auto iter = std::find(mImage.begin(), mImage.end(), mImage[num]);
		delete mImage[num];
		mImage.erase(iter);
	}
	else if (componentName._Equal(_T("BoxInput")))
	{
		if (mBoxInput)
		{
			delete mBoxInput;
			mBoxInput = NULL;
		}
	}
}