#pragma once
// --------------------- Include File --------------------
#include "ObjectCreateStruct.h"

class ObjectCreate
{
private:
	// mMeshName�̔z��ԍ�
	enum MeshName
	{
		Box,
		Cylinder,
		Sphere
	};
private:
	// ---------------- �����o�ϐ��錾 ----------------
	// --------- �E�B���h�E -------------
	//Game�N���X��public�����o�ɃA�N�Z�X����|�C���^
	class Game* mGame;
	HierarchyWindow mHierarchyWindow;
	InspectorWindow mInspectorWindow;
	AddObjWindow mAddObjWindow;
	AddComponentWindow mAddCW;// CW��ComponentWindow�̗�
	SelectWindow mSelectWindow;

	CreativeUI mPlayButton;
	CreativeUI mTitleButton;
	CreativeUI mSaveButton;

	// --------- �R���|�[�l���g -------------

	std::vector<class Actor*> mActor;	// Actor�N���X�ŕ\��(HierarchyButton�̏��ԂɑΉ�)
	std::map<std::wstring, int> mActorName;	// Actor�̖��O�Ɣԍ��Ή�
	std::vector<std::wstring> mUseComponent;	// �I�𒆂̃I�u�W�F�N�g���g����R���|�[�l���g
	std::vector<StructComponent*> mSelectComponent;	// �\������R���|�[�l���g�i�[
	std::map<std::wstring, int>mObjNum; // ���ꂼ��̃I�u�W�F�N�g�̐�
	std::wstring mModelFolderPath;			// �ǂ̃t�H���_��ǂݍ��ނ��w��
	std::wstring mImageFolderPath;			// �ǂ̃t�H���_��ǂݍ��ނ��w��
	std::vector<std::wstring> mMeshName;	// Mesh�̖��O
	std::vector<std::wstring> mModelName;	// �ǂݍ��񂾃��f���̖��O
	std::vector<std::wstring> mImageName;	// �ǂݍ��񂾉摜�̖��O
	std::vector<std::wstring> mSimpleModelName;	// �\������g���q�����������f����
	std::vector<std::wstring> mSimpleImageName;	// �\������g���q���������摜��
	std::vector<std::string> mFolderModelName; // �t�H���_���ƃ��f�������q����������
	std::vector<std::string> mFolderImageName; // �t�H���_���Ɖ摜�����q����������
	std::vector<VECTOR> mSavePosition;			// Play�O�̈ʒu�ۑ�
	std::vector<VECTOR> mSaveRotation;			// Play�O�̉�]�ۑ�

	HierarchyButton* mSelectButton;	// �I������Ă���{�^��
	HierarchyButton* mPreviousSelectButton;			// �O�̃t���[���ɑI������Ă����I�u�W�F�N�g�{�^��
	int mSelectActorNum;			// �I������Ă���Actor�̔ԍ�
	int mSelectButtonNum;			// �I�����ꂽ�{�^���̔ԍ�
	int mButtonNum;					// �S�{�^���̐�
	int mSelectComponentNum;			// �I��Actor��Component��
	bool mPutOnWindow;					// �J�[�\�����S�E�B���h�E�Əd�Ȃ��Ă��邩
	bool mOpenSelectWindow;					// SelectWindow���\������Ă��邩

	POINT mMousePoint;
	RECT mClientRect;

	int mNumOnlySize = 0; // -��.�ȊO�Ȃ瑝�₷
	const int mColorNumMax = 3;			// �F��
	// �����ϐ�
	float mClientWidth;
	// �c���ϐ�
	float mClientHeight;
	// �}�E�X���W�ϐ�
	float mMousePosX;
	float mMousePosY;

	bool mTitleTrans = false;
	bool mPlayTrans = false;

	// ObjectDrag�֐��p�ϐ�
	// �����������ǂ���
	bool mHit = false;
	// ���ׂĂ���Actor�̔ԍ�
	int mAddNum = 0;
	// imageSize
	float mWidthHalf = 0;
	float mHeightHalf = 0;
	// ���͎���BackSpace����������
	float mInputBackSpace = 0;
	bool mBackSpace = false;
	// Tab�����͂��ꂽ��
	bool mTab = false;

public:
	// ---------------- �����o�֐��錾 ----------------	
	ObjectCreate(class Game* game);
	~ObjectCreate();

	void LoadFile();
	void Init();
	void ProcessInput();
	void UpdateGame();

	void DeleteUI();		// �V�[�����ς��Ƃ���UI�`��폜

	bool GetPutOnUI() { return mPutOnWindow; }
	bool RectOnMouse(CreativeUI ui);		// �}�E�X��ui�̋�`�Ƃ̓����蔻��

	bool GetTitleTrans() { return mTitleTrans; }	// �^�C�g���ɕψڂ���
	bool GetPlayTrans() { return mPlayTrans; }		// �v���C��ʂɕψڂ���

	bool Get2DMode() { return mHierarchyWindow.m2D; }	// true�Ȃ�2D���[�h,false�Ȃ�3D���[�h
private:
	void SetDrawOrder();											// �`�揇�Ԃ����߂�

	void ObjectDrag();
	void HierarchyWindowInput();
	void InspectorWindowInput();
	void AddObjWindowInput();
	void AddComponentInput();
	void SelectWindowInput();

	void HierarchyWindowUpdate();
	void InspectorWindowUpdate();
	void AddObjWindowUpdate();
	void AddComponentUpdate();
	void SelectWindowUpdate();

	void AddObj(std::wstring objName);								// �I�u�W�F�N�g�̒ǉ�
	void DeleteObj();							// �I�u�W�F�N�g�폜
	void AddComponent(class Actor* actor, std::wstring componentName, const char* path = "");				// �R���|�[�l���g�̒ǉ�
	void InputComponentData(StructComponent* component);		// ���ꂼ��̃R���|�[�l���g�̓��͏���
	void DeleteComponent(int num);								// �R���|�[�l���g�폜
	void RiseInspector();									// �C���X�y�N�^�[��\��

	std::wstring Input(const TCHAR* text, bool numOnly = false, int maxSize = MAX_PATH);		// ����(maxSize:�擾�ł���ő啶����AnumOnly:true�Ő��������擾)

	bool GetModelFileNames(std::wstring folderPath, std::vector<std::wstring>& file_names);			// �w��̃��f���t�H���_�p�X�̒��g�𒲂ׂ�
	bool GetImageFileNames(std::wstring folderPath, std::vector<std::wstring>& file_names);			// �w��̉摜�t�H���_�p�X�̒��g�𒲂ׂ�
	void LoadText(const TCHAR filename[]);														// text�t�@�C�������[�h
	void SaveText();																			// text�t�@�C���ɃZ�[�u

	VECTOR2 InspectorFramePos(CreativeUI nowFrame, CreativeUI PrevFrame);						// ��ɂ���t���[������ɂ��č��̃t���[���̈ʒu�擾
	void TextBoxSelect(TextBox& textBox);														// TextBox�̑I��
	void TextBoxInput(TextBox& textBox, int& value);											// TextBox�̓��͏���(int)
	void TextBoxInput(TextBox& textBox, float& value);											// TextBox�̓��͏���(float)
	void TabClickInput(TextBox& nowSelect, TextBox& nextSelect);										// ���I�𒆂�TextBox���玟��TextBox�Ɉڂ�

	bool HitCheckPolygonLine(std::vector<VECTOR> vertices, VECTOR rayStartPos, VECTOR rayEndPos, VECTOR& crossPoint);	// �|���S���Ɛ��̏Փ�
};

