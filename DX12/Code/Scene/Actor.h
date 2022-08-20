#pragma once
#include "../VECTOR2.h"
#include "../VECTOR.h"
#include <vector>
#include <string>
#include "../Input/DInput.h"
enum class ActorState
{
	EActive,
	EPause,
	EDead
};

class Actor
{
// �R���|�[�l���g�֘A
protected:
	// ��̃R���|�[�l���g

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
	
	std::vector<std::wstring> mUse2DComponent;		// �A�N�^�[�ʂɎg�p����R���|�[�l���g���w��
	std::vector<std::wstring> mUse3DComponent;		// �A�N�^�[�ʂɎg�p����R���|�[�l���g���w��
public:
	// �Q�b�^�[�Z�b�^�[
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
	std::vector<class ImageComponent*> GetImageComponent() { return mImage; }
	void SetImageComponent(std::vector<class ImageComponent*> image) { mImage = image; }

	// ObjectCreate�N���X�Ŏg���Q�b�^�[/�Z�b�^�[
	const class Component* GetComponent(std::wstring componentName) const;
	void SetComponent(std::wstring componentName);
	void DeleateComponent(std::wstring componentName, int num = 0);
	
	std::vector<std::wstring> GetUse2DComponent();
	std::vector<std::wstring> GetUse3DComponent();
// �A�N�^�[��Transform���
private:
	//Actor�̏��
	ActorState mState;
	//�g�����X�t�H�[��
	VECTOR mPosition;
	VECTOR mRotation;
	VECTOR mScale;

	VECTOR mColor;
	float mAlpha; // a�l

	// �i�s�x�N�g��
	VECTOR mProgressVector;
	// �p���x
	VECTOR mAngularVelocity;
	// �W�����vFlg
	bool mJump;                     
	// �_�b�V��Flg
	bool mDash;
	//Game�N���X��public�����o�ɃA�N�Z�X����|�C���^
	class Game* mGame;
	//component�z��
	std::vector<class Component*> mComponents;
public:
	Actor(class Game* game);
	virtual ~Actor();

	void ProcessInput();
	virtual void ActorInput() {}

	void Update();
	virtual void UpdateActor() {};

	// �Q�b�^�[�E�Z�b�^�[
	ActorState GetState() const { return mState; }
	void SetState(ActorState state) { mState = state; }

	// �ʒu�E��]�E�X�P�[���E�F
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
	
	// �i�s�x�N�g��
	const VECTOR& GetProgressVector() const { return mProgressVector; }
	void SetProgressVector(const VECTOR& vec) { mProgressVector = vec; }
	// �p���x
	const VECTOR& GetAngularVelocity() const { return mAngularVelocity; }
	void SetAngularVelocity(const VECTOR& angle) { mAngularVelocity = angle; }
	// �W�����v
	const bool& GetJumpFlg() const { return mJump; }
	void SetJumpFlg(const bool& flg) { mJump = flg; }
	// �_�b�V��
	const bool& GetDashFlg() const { return mDash; }
	void SetDashFlg(const bool& flg) { mDash = flg; }

	class Game* GetGame() { return mGame; }

	void AddComponent(class Component*);
	void RemoveComponent(class Component*);
};

