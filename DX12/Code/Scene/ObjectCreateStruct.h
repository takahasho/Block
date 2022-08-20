#pragma once
// --------------------- Include File --------------------
#include "../VECTOR2.h"
#include "../VECTOR.h"
#include "../Input/DInput.h"
#include "Actor.h"
#include <string>
#include <map>
// ---------------- メンバ構造体宣言 ----------------
// ui用画像情報
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
// text情報
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
// HierarchyWindowのボタン
struct HierarchyButton
{
	CreativeUI RectBlue;
	CreativeUI RectRed;
	CreativeUI RectGreen;
	CreativeText Text;
	VECTOR2 basePos;	// スライド移動するときに基本の位置に戻れるように
	int addNum = 0;			// 追加されたときの番号
	int ColorNum = 0;				// 色識別(0:青、1:赤、2:緑)
	std::wstring ObjType;			// Objectのもともとのタイプ(モデルならモデル名)
};
// HierarchyWindow(左)
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
	bool m2D;			// 2Dウィンドウにするか(false:3D、true:2D)
	bool mPutOnHie;		// UIとカーソルが重なっているときカメラが動くと困るので重なっているか確認する
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

// インスペクターの名前部分
struct NameFrame
{
	CreativeUI Frame;
	CreativeUI XButton;
	CreativeUI ColorBlue;
	CreativeUI ColorRed;
	CreativeUI ColorGreen;
	TextBox NameBox;
	VECTOR2 basePos;	// スライド移動するときに基本の位置に戻れるように
};

// Meshの色変え
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
	int ComponentNum = 0;	// 同一のコンポーネントの数
	VECTOR2 basePos;	// スライド移動するときに基本の位置に戻れるように
	bool Open = true;	// コンポーネントウィンドウが開いているかどうか(false:閉じている、true:開いている)
	bool Check = true;	// このコンポーネントを適用するか(false:適用しない、true:適用する)
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

// 簡易的なUI
struct ScriptUI :public StructComponent
{
	void SetMetaData() override;
	void SetPos() override;
	void Update(CreativeUI& SlectTextBox) override;
	void RiseDraw() override;
};

// InspectorWindow(右)
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

	bool mPutOnIns;		// UIとカーソルが重なっているときカメラが動くと困るので重なっているか確認する
	bool mGetRightBorder;
	bool mPull;
	bool mColorFrameFlag;		// ColorFrameの有無
	InspectorWindow()
	{
		mPutOnIns = false;
		mGetRightBorder = false;
		mPull = false;
		mColorFrameFlag = false;
	}

	void SetMetaData();
};

// AddObjクリック時のウィンドウ
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

	// ページ送り
	CreativeUI LeftArrow;
	CreativeUI RightArrow;
	CreativeText PageNumText;
	int PageNumber;				// 現在のページ番号
	int PrevPageNumber;			// 前のページ番号
	int MeshPageNumMax;			// 最大ページ数(Mesh)
	int ModelPageNumMax;		// 最大ページ数(Model)
	int ImagePageNumMax;		// 最大ページ数(Image)

	bool OpenWindow;
	bool mPutOnAddOW;			// UIとカーソルが重なっているときカメラが動くと困るので重なっているか確認する
	bool MeshOrModelWindow;	// MeshWindow,ModelWindowどちらが開いているか(false:Mesh,true:Model)
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

// AddCompクリック時のウィンドウ
struct AddComponentWindow
{
	CreativeUI AddButton;

	CreativeUI Window;
	std::vector<CreativeUI> Button;
	std::vector<CreativeText> NameText;
	std::vector<bool>ClickModelButton;
	CreativeUI ButtonOn;

	// ページ送り
	CreativeUI LeftArrow;
	CreativeUI RightArrow;
	CreativeText PageNumText;
	int PageNumber;				// 現在のページ番号
	int PrevPageNumber;			// 前のページ番号
	int PageNumMax;				// 最大ページ数

	bool OpenWindow;
	bool mPutOnAddCW;		// UIとカーソルが重なっているときカメラが動くと困るので重なっているか確認する
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

// Inspectorでファイルなどを選択する時のウィンドウ
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
	bool mPutOnSW = false;	// UIとカーソルが重なっているときカメラが動くと困るので重なっているか確認する
	int ComponentNum = 0;	// 同一のComponentがあった時何番目のComponentが分かるように

	CreativeText PageNumText;
	int PageNumber = 1;				// 現在のページ番号
	int PrevPageNumber = 1;			// 前のページ番号
	int PageNumMax = 1;				// 最大ページ数

	void SetMetaData();
	void SetButton(int size);
	void SetText(std::vector<std::wstring> text);
};


void SetUI(CreativeUI& ui);
void RiseUI(CreativeUI& ui);									// uiを消す
void SetText(CreativeText& text);		// fontsize:true(小さめ)false(大きめ)
void RiseText(CreativeText& text);
// Textを表示するUIを基準にしてTextの位置取得
VECTOR2 TextPos(CreativeText text, CreativeUI ui);						
