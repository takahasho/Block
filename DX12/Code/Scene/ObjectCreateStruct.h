#pragma once
// --------------------- Include File --------------------
#include "../VECTOR2.h"
#include "../VECTOR.h"
#include "../Input/DInput.h"
#include "Actor.h"
#include <string>
#include <map>
// ---------------- �����o�\���̐錾 ----------------
// ui�p�摜���
struct CreativeUI
{
	VECTOR2 position;
	VECTOR2 scale;
	float angle;

	// Rect
	float left;
	float right;
	float top;
	float bottom;

	const char* fileName;
	int entryPos;
	int drawOrder;

	CreativeUI()
	{
		position = VECTOR2(0, 0);
		scale = VECTOR2(1, 1);
		angle = 0;

		left = 0;
		right = 0;
		top = 0;
		bottom = 0;

		fileName = {};
		entryPos = -1;
		drawOrder = 0;
	}
	CreativeUI(float width, float height)
	{
		position = VECTOR2(0, 0);
		scale = VECTOR2(1, 1);
		angle = 0;

		left = -width / 2;
		right = width / 2;
		top = -height / 2;
		bottom = height / 2;

		fileName = {};
		entryPos = -1;
		drawOrder = 0;
	}
};
// text���
struct CreativeText
{
	VECTOR2 position;
	VECTOR color;
	float maxWidth;
	float maxHeight;
	bool underLine;
	bool minisize;
	DWRITE_FONT_WEIGHT fontWeigh;
	TCHAR Text[MAX_PATH] = {};

	int entryPos;
	CreativeText()
	{
		position = {};
		color = {};
		maxWidth = 400;
		maxHeight = 100;
		underLine = false;
		minisize = false;
		fontWeigh = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_NORMAL;
		entryPos = -1;
	}
};
// HierarchyWindow�̃{�^��
struct HierarchyButton
{
	CreativeUI RectBlue;
	CreativeUI RectRed;
	CreativeUI RectGreen;
	CreativeText Text;
	VECTOR2 basePos;	// �X���C�h�ړ�����Ƃ��Ɋ�{�̈ʒu�ɖ߂��悤��
	int addNum = 0;			// �ǉ����ꂽ�Ƃ��̔ԍ�
	int ColorNum = 0;				// �F����(0:�A1:�ԁA2:��)
	std::wstring ObjType;			// Object�̂��Ƃ��Ƃ̃^�C�v(���f���Ȃ烂�f����)
};
// HierarchyWindow(��)
struct HierarchyWindow
{
	CreativeUI Window;
	CreativeUI Border;
	CreativeUI BorderSlider;
	std::vector<CreativeUI> Tab3D2D;
	std::vector<HierarchyButton> Button3D;
	std::vector<HierarchyButton> Button2D;
	CreativeUI SelectButtonBlue;
	CreativeUI SelectButtonRed;
	CreativeUI SelectButtonGreen;
	bool m2D;			// 2D�E�B���h�E�ɂ��邩(false:3D�Atrue:2D)
	bool mPutOnHie;		// UI�ƃJ�[�\�����d�Ȃ��Ă���Ƃ��J�����������ƍ���̂ŏd�Ȃ��Ă��邩�m�F����
	bool mGetLeftBorder;
	bool mGetLeftBorderSlider;
	HierarchyWindow()
	{
		m2D = false;
		mPutOnHie = false;
		mGetLeftBorder = false;
		mGetLeftBorderSlider = false;
	}

	void SetMetaData();
	void SetPos();
};

struct TextBox
{
	CreativeUI Box;
	CreativeText Text;
	bool Select;
	TextBox()
	{
		Select = false;
	}
};

// �C���X�y�N�^�[�̖��O����
struct NameFrame
{
	CreativeUI Frame;
	CreativeUI XButton;
	CreativeUI ColorBlue;
	CreativeUI ColorRed;
	CreativeUI ColorGreen;
	TextBox NameBox;
	VECTOR2 basePos;	// �X���C�h�ړ�����Ƃ��Ɋ�{�̈ʒu�ɖ߂��悤��
};

// Mesh�̐F�ς�
struct MeshColor
{
	CreativeUI Frame;
	TextBox rgb[3];
};
// ======================================== ComponentStruct ==============================================
struct StructComponent
{
public:
	CreativeUI Top;
	CreativeUI Bottom;
	CreativeUI RightArrow;
	CreativeUI DownArrow;
	CreativeUI XButton;
	CreativeUI CheckBoxOn;
	CreativeUI CheckBoxOff;
	CreativeText Title;
	std::wstring Type;
	int ComponentNum = 0;	// ����̃R���|�[�l���g�̐�
	VECTOR2 basePos;	// �X���C�h�ړ�����Ƃ��Ɋ�{�̈ʒu�ɖ߂��悤��
	bool Open = true;	// �R���|�[�l���g�E�B���h�E���J���Ă��邩�ǂ���(false:���Ă���Atrue:�J���Ă���)
	bool Check = true;	// ���̃R���|�[�l���g��K�p���邩(false:�K�p���Ȃ��Atrue:�K�p����)
	virtual void SetMetaData() { ; }
	virtual void SetPos() { ; }
	virtual void Input() { ; }
	virtual void Update(CreativeUI& SlectTextBox) { ; }
	virtual void RiseDraw() { ; }
};
struct TransformComponent :public StructComponent
{
	TextBox Position[3];
	TextBox Rotation[3];
	TextBox Scale[3];
};

struct InputUI :public StructComponent
{
	CreativeText MoveSpeedText;
	TextBox MoveSpeed;
	void SetMetaData() override;
	void SetPos() override;
	void Update(CreativeUI& SlectTextBox) override;
	void RiseDraw() override;
};

struct MeshUI :public StructComponent
{
	TextBox SourceImage;
	TextBox Color[4];
	CreativeUI SelectButton;
	void SetMetaData() override;
	void SetPos() override;
	void Update(CreativeUI& SlectTextBox) override;
	void RiseDraw() override;
};

struct ModelUI :public StructComponent
{
	TextBox SourceImage;
	TextBox Color[4];
	CreativeUI SelectButton;
	void SetMetaData() override;
	void SetPos() override;
	void Update(CreativeUI& SlectTextBox) override;
	void RiseDraw() override;
};

struct BoxColliderUI :public StructComponent
{
	CreativeText Sizexyz;
	CreativeText Centerxyz;
	TextBox Size[3];
	TextBox Center[3];
	void SetMetaData() override;
	void SetPos() override;
	void Update(CreativeUI& SlectTextBox) override;
	void RiseDraw() override;
};

struct CylinderColliderUI :public StructComponent
{
	void SetMetaData() override;
};

struct CapsuleColliderUI :public StructComponent
{
	CreativeText HeightText;
	CreativeText RadiusText;
	CreativeText Centerxyz;
	TextBox Height;
	TextBox Radius;
	TextBox Center[3];
	void SetMetaData() override;
	void SetPos() override;
	void Update(CreativeUI& SlectTextBox) override;
	void RiseDraw() override;
};

struct RigidBodyUI :public StructComponent
{
	void SetMetaData() override;
	void SetPos() override;
	void Update(CreativeUI& SlectTextBox) override;
	void RiseDraw() override;
};

struct ImageUI :public StructComponent
{
	TextBox SourceImage;
	TextBox Color[4];
	TextBox OrderinLayer;
	CreativeUI SelectButton;
	void SetMetaData() override;
	void SetPos() override;
	void Update(CreativeUI& SlectTextBox) override;
	void RiseDraw() override;
};

// �ȈՓI��UI
struct ScriptUI :public StructComponent
{
	void SetMetaData() override;
	void SetPos() override;
	void Update(CreativeUI& SlectTextBox) override;
	void RiseDraw() override;
};

// InspectorWindow(�E)
struct InspectorWindow
{
	CreativeUI Window;
	CreativeUI Border;
	CreativeUI SelectTextBox;
	CreativeUI PullButton;
	NameFrame TopFrame;
	MeshColor ColorFrame;

	// ========== Component ==========
	TransformComponent Transform;

	bool mPutOnIns;		// UI�ƃJ�[�\�����d�Ȃ��Ă���Ƃ��J�����������ƍ���̂ŏd�Ȃ��Ă��邩�m�F����
	bool mGetRightBorder;
	bool mPull;
	bool mColorFrameFlag;		// ColorFrame�̗L��
	InspectorWindow()
	{
		mPutOnIns = false;
		mGetRightBorder = false;
		mPull = false;
		mColorFrameFlag = false;
	}

	void SetMetaData();
};

// AddObj�N���b�N���̃E�B���h�E
struct AddObjWindow
{
	CreativeUI AddObjectButton;

	// MeshWindow
	CreativeUI MeshWindow;
	CreativeText MeshText;
	CreativeUI BOXButton;
	CreativeUI BOXButtonOff;
	CreativeUI CylinderButton;
	CreativeUI CylinderButtonOff;
	CreativeUI SphereButton;
	CreativeUI SphereButtonOff;
	bool ClickBoxButton;
	bool ClickCylinderButton;
	bool ClickSphereButton;

	// ModelWindow
	CreativeUI ModelWindow;
	CreativeText ModelText;
	std::vector<CreativeUI> ModelButton;
	std::vector<CreativeText> ModelNameText;
	std::vector<bool>ClickModelButton;
	CreativeUI ModelButtonOn;

	// ImageWindow
	CreativeUI ImageWindow;
	std::vector<CreativeUI> ImageButton;
	std::vector<CreativeText> ImageNameText;
	std::vector<bool>ClickImageButton;

	// �y�[�W����
	CreativeUI LeftArrow;
	CreativeUI RightArrow;
	CreativeText PageNumText;
	int PageNumber;				// ���݂̃y�[�W�ԍ�
	int PrevPageNumber;			// �O�̃y�[�W�ԍ�
	int MeshPageNumMax;			// �ő�y�[�W��(Mesh)
	int ModelPageNumMax;		// �ő�y�[�W��(Model)
	int ImagePageNumMax;		// �ő�y�[�W��(Image)

	bool OpenWindow;
	bool mPutOnAddOW;			// UI�ƃJ�[�\�����d�Ȃ��Ă���Ƃ��J�����������ƍ���̂ŏd�Ȃ��Ă��邩�m�F����
	bool MeshOrModelWindow;	// MeshWindow,ModelWindow�ǂ��炪�J���Ă��邩(false:Mesh,true:Model)
	AddObjWindow()
	{
		ClickBoxButton = false;
		ClickCylinderButton = false;
		ClickSphereButton = false;
		OpenWindow = false;
		mPutOnAddOW = false;
		MeshOrModelWindow = false;
		PageNumber = 1;
		PrevPageNumber = 1;
		MeshPageNumMax = 1;
		ModelPageNumMax = 1;
		ImagePageNumMax = 1;
	}

	void SetMetaData(std::vector<std::wstring> SimpleModelName, std::vector<std::wstring> SimpleImageName);
};

// AddComp�N���b�N���̃E�B���h�E
struct AddComponentWindow
{
	CreativeUI AddButton;

	CreativeUI Window;
	std::vector<CreativeUI> Button;
	std::vector<CreativeText> NameText;
	std::vector<bool>ClickModelButton;
	CreativeUI ButtonOn;

	// �y�[�W����
	CreativeUI LeftArrow;
	CreativeUI RightArrow;
	CreativeText PageNumText;
	int PageNumber;				// ���݂̃y�[�W�ԍ�
	int PrevPageNumber;			// �O�̃y�[�W�ԍ�
	int PageNumMax;				// �ő�y�[�W��

	bool OpenWindow;
	bool mPutOnAddCW;		// UI�ƃJ�[�\�����d�Ȃ��Ă���Ƃ��J�����������ƍ���̂ŏd�Ȃ��Ă��邩�m�F����
	AddComponentWindow()
	{
		OpenWindow = false;
		mPutOnAddCW = false;
		PageNumber = 1;
		PrevPageNumber = 1;
		PageNumMax = 1;
	}

	void SetMetaData();
	void SetButton(int size);
	void SetText(std::vector<std::wstring> text);
};

// Inspector�Ńt�@�C���Ȃǂ�I�����鎞�̃E�B���h�E
struct SelectWindow
{	
	CreativeUI Window;
	std::vector<CreativeUI> Button;
	std::vector<CreativeText> NameText;
	std::vector<bool>ClickSelectButton;
	CreativeUI ButtonOn;
	CreativeUI RightArrow;
	CreativeUI LeftArrow;
	int DecisionNum = 0;
	bool Decision = false;
	bool mPutOnSW = false;	// UI�ƃJ�[�\�����d�Ȃ��Ă���Ƃ��J�����������ƍ���̂ŏd�Ȃ��Ă��邩�m�F����
	int ComponentNum = 0;	// �����Component�������������Ԗڂ�Component��������悤��

	CreativeText PageNumText;
	int PageNumber = 1;				// ���݂̃y�[�W�ԍ�
	int PrevPageNumber = 1;			// �O�̃y�[�W�ԍ�
	int PageNumMax = 1;				// �ő�y�[�W��

	void SetMetaData();
	void SetButton(int size);
	void SetText(std::vector<std::wstring> text);
};


void SetUI(CreativeUI& ui);
void RiseUI(CreativeUI& ui);									// ui������
void SetText(CreativeText& text);		// fontsize:true(������)false(�傫��)
void RiseText(CreativeText& text);
// Text��\������UI����ɂ���Text�̈ʒu�擾
VECTOR2 TextPos(CreativeText text, CreativeUI ui);						
