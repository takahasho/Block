#pragma once
#include "../VECTOR2.h"
#include "../VECTOR.h"
#include <vector>
#include <string>
#include "../Input/DInput.h"
// Actor状態
enum class ActorState
{
	EActive,
	EPause,
	EDead
};

// 使用コライダー
enum class ColliderType
{
	Box = 0,
	Cylinder = 1,
	Capsule = 2,
	ModelMesh = 3,
	TypeNone = 4
};

class Actor
{
// コンポーネント関連
protected:
	// 空のコンポーネント

	class PlayerInputComponent* mPlayerInput;
	class ObjectComponent* mObj;
	class ModelComponent* mModel;
	class BoxComponent* mBox;
	class CylinderComponent* mCylinder;
	class CapsuleComponent* mCapsule;
	class ModelMeshComponent* mModelMesh;
	class RigidBodyComponent* mRigidBody;
	std::vector<class ImageComponent*> mImage;
	class GameManager* mManager;
	class BoxInputComponent* mBoxInput;
	
	std::vector<std::wstring> mUse2DComponent;		// アクター別に使用するコンポーネントを指定
	std::vector<std::wstring> mUse3DComponent;		// アクター別に使用するコンポーネントを指定

	ColliderType mColliderType;
public:
	// ゲッターセッター
	class PlayerInputComponent* GetInput() { return mPlayerInput; }
	void SetInputComponent(class PlayerInputComponent* input) { mPlayerInput = input; }
	class  ObjectComponent* GetMeshComponent() { return mObj; }
	void SetMeshComponent(class ObjectComponent* object) { mObj = object; }
	class ModelComponent* GetModelComponent() { return mModel; }
	void SetModelComponent(class ModelComponent* model) { mModel = model; }
	class BoxComponent* GetBoxComponent() { return mBox; }
	void SetBoxComponent(class BoxComponent* box) { mBox = box; }
	class CylinderComponent* GetCylinderComponent() { return mCylinder; }
	void SetCylinderComponent(class CylinderComponent* cylinder) { mCylinder = cylinder; }
	class CapsuleComponent* GetCapsuleComponent() { return mCapsule; }
	void SetCapsuleComponent(class CapsuleComponent* capsule) { mCapsule = capsule; }
	class ModelMeshComponent* GetModelMeshComponent() { return mModelMesh; }
	void SetModelMeshComponent(class ModelMeshComponent* modelMesh) { mModelMesh = modelMesh; }
	class RigidBodyComponent* GetRigidBodyComponent() { return mRigidBody; }
	void SetRigidBodyComponent(class RigidBodyComponent* rigitBody) { mRigidBody = rigitBody; }
	std::vector<class ImageComponent*> GetImageComponent() { return mImage; }
	void SetImageComponent(std::vector<class ImageComponent*> image) { mImage = image; }

	// ObjectCreateクラスで使うゲッター/セッター
	const class Component* GetComponent(std::wstring componentName) const;
	void SetComponent(std::wstring componentName);
	void DeleateComponent(std::wstring componentName, int num = 0);
	
	std::vector<std::wstring> GetUse2DComponent();
	std::vector<std::wstring> GetUse3DComponent();

	ColliderType GetColliderType() { return mColliderType; }
	void SetColliderType(ColliderType colType) { mColliderType = colType; }
// アクターのTransform情報
private:
	//Actorの状態
	ActorState mState;
	//トランスフォーム
	VECTOR mPosition;
	VECTOR mRotation;
	VECTOR mScale;

	VECTOR mColor;
	float mAlpha; // a値

	//Gameクラスのpublicメンバにアクセスするポインタ
	class Game* mGame;
	//component配列
	std::vector<class Component*> mComponents;
public:
	Actor(class Game* game);
	virtual ~Actor();

	void ProcessInput();
	virtual void ActorInput() {}

	void Update();
	virtual void UpdateActor() {};

	// ゲッター・セッター
	ActorState GetState() const { return mState; }
	void SetState(ActorState state) { mState = state; }

	// 位置・回転・スケール・色
	const VECTOR& GetPosition() const { return mPosition; }
	void SetPosition(const VECTOR& pos) { mPosition = pos; }
	const VECTOR& GetRotation() const { return mRotation; }
	void SetRotation(const VECTOR& rotation) { mRotation = rotation; }
	const VECTOR& GetScale() const { return mScale; }
	void SetScale(const VECTOR& scale) { mScale = scale; }
	const VECTOR& GetColor() const { return mColor; }
	void SetColor(const VECTOR& color) { mColor = color; }
	const float& GetAlpha() const { return mAlpha; }
	void SetAlpha(const float& value) { mAlpha = value; }

	class Game* GetGame() { return mGame; }

	void AddComponent(class Component*);
	void RemoveComponent(class Component*);
};

