#include "ObjectCreateStruct.h"
#include "../GameObject/Texture.h"
#include "../DX12System.h"
// ===================================== Window関連 ===============================================
void HierarchyWindow::SetMetaData()
{
	// ----- HierarchyWindow作成 -----
	// UI
	CreativeUI HierarchyWindow(300, 1080);
	HierarchyWindow.fileName = "Resouse\\Texture\\UI\\Window.png";
	HierarchyWindow.position = VECTOR2(0, 540);
	Window = HierarchyWindow;
	CreativeUI HB(6, 1080);
	HB.fileName = "Resouse\\Texture\\UI\\Border.png";
	// 3はボーダーの幅
	HB.position = VECTOR2(Window.right + 3, 540);
	Border = HB;
	CreativeUI BS(25, 50);
	BS.fileName = "Resouse\\Texture\\UI\\Slider.png";
	BS.position = VECTOR2(Border.position.x, 0 + BS.bottom);
	BorderSlider = BS;
	CreativeUI SelectButton(300, 100);
	SelectButton.fileName = "Resouse\\Texture\\UI\\SelectButtonBlue.png";
	SelectButtonBlue = SelectButton;
	SelectButton.fileName = "Resouse\\Texture\\UI\\SelectButtonRed.png";
	SelectButtonRed = SelectButton;
	SelectButton.fileName = "Resouse\\Texture\\UI\\SelectButtonGreen.png";
	SelectButtonGreen = SelectButton;
	CreativeUI tab3D2D(30, 150);
	tab3D2D.fileName= "Resouse\\Texture\\UI\\Button3D2D1.png";
	// 18はボーダーの幅+このタブの幅(3+15)
	tab3D2D.position = VECTOR2(Border.position.x + 18, 200);
	Tab3D2D.push_back(tab3D2D);
	tab3D2D.fileName = "Resouse\\Texture\\UI\\Button3D2D2.png";
	Tab3D2D.push_back(tab3D2D);
}

void InspectorWindow::SetMetaData()
{
	// ----- InspectorWindow作成 -----
	// UI
	CreativeUI InspectorWindow(300, 1080);
	InspectorWindow.fileName = "Resouse\\Texture\\UI\\Window.png";
	InspectorWindow.position = VECTOR2((float)g_window_width - 150, 540);
	Window = InspectorWindow;
	CreativeUI InspectorBorder(5, 1080);
	InspectorBorder.fileName = "Resouse\\Texture\\UI\\Border.png";
	// 3はボーダーの幅
	InspectorBorder.position = VECTOR2((float)g_window_width + Window.left - 153, 540);
	Border = InspectorBorder;
	CreativeUI SelectTB(100, 40);
	SelectTB.scale.x = 2;
	SelectTB.fileName = "Resouse\\Texture\\UI\\SelectTextBox.png";
	SelectTextBox = SelectTB;
	CreativeUI PB(25, 50);
	PB.fileName = "Resouse\\Texture\\UI\\Pull.png";
	PB.position.y = 50;
	PullButton = PB;
	CreativeUI Frame(300, 50);
	Frame.fileName = "Resouse\\Texture\\UI\\Frame.png";
	TopFrame.Frame = Frame;
	CreativeUI XButton(40, 40);
	XButton.fileName = "Resouse\\Texture\\UI\\XButton.png";
	TopFrame.XButton = XButton;

	// ColorButton
	CreativeUI ColorBlue(45, 45);
	ColorBlue.fileName = "Resouse\\Texture\\UI\\ColorBlue.png";
	TopFrame.ColorBlue = ColorBlue;
	CreativeUI ColorRed(45, 45);
	ColorRed.fileName = "Resouse\\Texture\\UI\\ColorRed.png";
	TopFrame.ColorRed = ColorRed;
	CreativeUI ColorGreen(45, 45);
	ColorGreen.fileName = "Resouse\\Texture\\UI\\ColorGreen.png";
	TopFrame.ColorGreen = ColorGreen;

	// TextBox
	CreativeUI Box(100, 40);
	Box.scale.x = 2;
	Box.fileName = "Resouse\\Texture\\UI\\TextBox.png";
	// Text
	CreativeText Name;
	Name.minisize = true;
	// TextBoxと同じ大きさにする
	Name.maxWidth = 200;
	Name.maxHeight = 40;
	TextBox NameBox;
	NameBox.Box = Box;
	NameBox.Text = Name;
	TopFrame.NameBox = NameBox;
	TopFrame.basePos = VECTOR2((float)g_window_width - 150, 25); // 25はTopFramの高さ/2

	// ColorFrame
	CreativeUI CF(300, 25);
	CF.fileName = "Resouse\\Texture\\UI\\ColorFrame.png";
	Box.scale.x = 0.65;
	// ここだけ枠線にかぶるのでBoxを少し小さめにした
	Box.scale.y = 0.5;
	// Text
	CreativeText Text;
	Text.minisize = true;
	Text.maxWidth = 65;
	Text.maxHeight = 24;

	ColorFrame.Frame = CF;
	// 3はr,g,b
	for (int i = 0; i < 3; i++)
	{
		ColorFrame.rgb[i].Box = Box;
		ColorFrame.rgb[i].Text = Text;
	}

	// TransformFrame
	CreativeUI TransformTop(300, 25);
	TransformTop.fileName = "Resouse\\Texture\\UI\\TransformTop.png";
	Transform.Top = TransformTop;
	CreativeUI TransformBottom(300, 150);
	TransformBottom.fileName = "Resouse\\Texture\\UI\\TransformBottom.png";
	Transform.Bottom = TransformBottom;
	Transform.basePos = VECTOR2((float)g_window_width - 150, 50); // 50はTopFramの高さ
	CreativeUI Arrow(20, 20);
	Arrow.fileName = "Resouse\\Texture\\UI\\RightArrow.png";
	Transform.RightArrow = Arrow;
	Arrow.fileName = "Resouse\\Texture\\UI\\DownArrow.png";
	Transform.DownArrow = Arrow;
	// TextBox(３種＊３)
	// 3はx,y,z
	for (int i = 0; i < 3; i++)
	{
		Transform.Position[i].Box = Box;
		Transform.Position[i].Text = Text;
		Transform.Rotation[i].Box = Box;
		Transform.Rotation[i].Text = Text;
		Transform.Scale[i].Box = Box;
		Transform.Scale[i].Text = Text;
	}
}

void AddObjWindow::SetMetaData(std::vector<std::wstring> SimpleModelName, std::vector<std::wstring> SimpleImageName)
{
	CreativeUI AddObj(30, 150);
	AddObj.fileName = "Resouse\\Texture\\UI\\AddObj.png";
	AddObjectButton = AddObj;
	CreativeUI AddObjWindow(150, 250);
	AddObjWindow.position = VECTOR2(-100, 0);
	AddObjWindow.fileName = "Resouse\\Texture\\UI\\AddObjWindow.png";
	MeshWindow = AddObjWindow;
	AddObjWindow.scale = VECTOR2(-1, 1);
	ModelWindow = AddObjWindow;
	AddObjWindow.fileName = "Resouse\\Texture\\UI\\AddImageWindow.png";
	AddObjWindow.scale = VECTOR2(1, 1);
	ImageWindow = AddObjWindow;
	// MeshWindow
	CreativeUI MiniWindowButton(150, 40);
	MiniWindowButton.fileName = "Resouse\\Texture\\UI\\BOX.png";
	BOXButton = MiniWindowButton;
	MiniWindowButton.fileName = "Resouse\\Texture\\UI\\BOXoff.png";
	BOXButtonOff = MiniWindowButton;
	MiniWindowButton.fileName = "Resouse\\Texture\\UI\\Cylinder.png";
	CylinderButton = MiniWindowButton;
	MiniWindowButton.fileName = "Resouse\\Texture\\UI\\Cylinderoff.png";
	CylinderButtonOff = MiniWindowButton;
	MiniWindowButton.fileName = "Resouse\\Texture\\UI\\Sphere.png";
	SphereButton = MiniWindowButton;
	MiniWindowButton.fileName = "Resouse\\Texture\\UI\\Sphereoff.png";
	SphereButtonOff = MiniWindowButton;
	// ModelWindow
	ModelButton.resize(SimpleModelName.size());
	ClickModelButton.resize(SimpleModelName.size());
	MiniWindowButton.fileName = "Resouse\\Texture\\UI\\Buttonoff.png";
	for (int i = 0; i < SimpleModelName.size(); i++)
	{
		ModelButton[i] = MiniWindowButton;
	}
	MiniWindowButton.fileName = "Resouse\\Texture\\UI\\Button.png";
	ModelButtonOn = MiniWindowButton;
	CreativeText MiniWindowText;
	MiniWindowText.maxWidth = 150;
	MiniWindowText.maxHeight = 40;
	MiniWindowText.minisize = true;
	ModelNameText.resize(SimpleModelName.size());
	for (int i = 0; i < SimpleModelName.size(); i++)
	{
		swprintf(MiniWindowText.Text, MAX_PATH, SimpleModelName[i].c_str());
		ModelNameText[i] = MiniWindowText;
	}
	// ImageWindow
	ImageButton.resize(SimpleImageName.size());
	ClickImageButton.resize(SimpleImageName.size());
	MiniWindowButton.fileName = "Resouse\\Texture\\UI\\Buttonoff.png";
	for (int i = 0; i < SimpleImageName.size(); i++)
	{
		ImageButton[i] = MiniWindowButton;
	}
	ImageNameText.resize(SimpleImageName.size());
	for (int i = 0; i < SimpleImageName.size(); i++)
	{
		swprintf(MiniWindowText.Text, MAX_PATH, SimpleImageName[i].c_str());
		ImageNameText[i] = MiniWindowText;
	}

	swprintf(MiniWindowText.Text, MAX_PATH, _T("Mesh"));
	MiniWindowText.maxWidth = 75;
	MiniWindowText.maxHeight = 25;
	MeshText = MiniWindowText;
	swprintf(MiniWindowText.Text, MAX_PATH, _T("Model"));
	ModelText = MiniWindowText;

	CreativeUI Arrow(20, 20);
	Arrow.fileName = "Resouse\\Texture\\UI\\RightArrow.png";
	RightArrow = Arrow;
	Arrow.scale.x *= -1;
	LeftArrow = Arrow;
	swprintf(MiniWindowText.Text, MAX_PATH, _T("1"));
	PageNumText = MiniWindowText;

	MeshPageNumMax = 1;
	ModelPageNumMax = (int)(ModelButton.size() - 1) / 5 + 1;
	ImagePageNumMax = (int)(ImageButton.size() - 1) / 5 + 1;
}

void AddComponentWindow::SetMetaData()
{
	CreativeUI AddComponent(30, 150);
	AddComponent.fileName = "Resouse\\Texture\\UI\\AddComponent.png";
	AddButton = AddComponent;
	CreativeUI window(150, 250);
	window.fileName = "Resouse\\Texture\\UI\\ComponentWindow.png";
	Window = window;
	CreativeUI MiniWindowButton(150, 40);
	MiniWindowButton.fileName = "Resouse\\Texture\\UI\\Button.png";
	ButtonOn = MiniWindowButton;

	CreativeUI Arrow(20, 20);
	Arrow.fileName = "Resouse\\Texture\\UI\\RightArrow.png";
	RightArrow = Arrow;
	Arrow.scale.x *= -1;
	LeftArrow = Arrow;
	CreativeText Text;
	Text.maxWidth = 150;
	Text.maxHeight = 40;
	Text.minisize = true;
	swprintf(Text.Text, MAX_PATH, _T("1"));
	PageNumText = Text;	
}
void AddComponentWindow::SetButton(int size)
{
	Button.clear();
	CreativeUI MiniWindowButton(150, 40);
	MiniWindowButton.fileName = "Resouse\\Texture\\UI\\Buttonoff.png";
	for (int i = 0; i < size; i++)
	{
		Button.push_back(MiniWindowButton);
	}
	PageNumMax = (int)(Button.size() - 1) / 5 + 1;
}

void AddComponentWindow::SetText(std::vector<std::wstring> text)
{
	NameText.clear();
	NameText.resize(text.size());
	for (int i = 0; i < text.size(); i++)
	{
		NameText[i].maxWidth = 130;
		NameText[i].maxHeight = 45;
		NameText[i].minisize = true;
		swprintf(NameText[i].Text, MAX_PATH, text[i].c_str());
	}
}

void SelectWindow::SetMetaData()
{
	CreativeUI window(150, 250);
	window.fileName = "Resouse\\Texture\\UI\\ComponentWindow.png";
	Window = window;
	CreativeUI MiniWindowButton(150, 40);
	MiniWindowButton.fileName = "Resouse\\Texture\\UI\\Button.png";
	ButtonOn = MiniWindowButton;

	CreativeUI Arrow(20, 20);
	Arrow.fileName = "Resouse\\Texture\\UI\\RightArrow.png";
	RightArrow = Arrow;
	Arrow.scale.x *= -1;
	LeftArrow = Arrow;
	CreativeText Text;
	Text.maxWidth = 150;
	Text.maxHeight = 40;
	Text.minisize = true;
	swprintf(Text.Text, MAX_PATH, _T("1"));
	PageNumText = Text;
}
void SelectWindow::SetButton(int size)
{
	Button.clear();
	CreativeUI MiniWindowButton(150, 40);
	MiniWindowButton.fileName = "Resouse\\Texture\\UI\\Buttonoff.png";
	for (int i = 0; i < size; i++)
	{
		Button.push_back(MiniWindowButton);
	}
	PageNumMax = (int)(Button.size() - 1) / 5 + 1;
}

void SelectWindow::SetText(std::vector<std::wstring> text)
{
	NameText.clear();
	NameText.resize(text.size());
	for (int i = 0; i < text.size(); i++)
	{
		NameText[i].maxWidth = 130;
		NameText[i].maxHeight = 45;
		NameText[i].minisize = true;
		swprintf(NameText[i].Text, MAX_PATH, text[i].c_str());
	}
}

// ===================================== Component関連 ===============================================
// ---------------------- PlayerInputComponent -------------------
void InputUI::SetMetaData()
{
	CreativeUI TopFrame(300, 25);
	TopFrame.fileName = "Resouse\\Texture\\UI\\ComponentFrame.png";
	Top = TopFrame;
	CreativeUI BottomFrame(300, 50);
	BottomFrame.fileName = "Resouse\\Texture\\UI\\Frame300x50.png";
	Bottom = BottomFrame;
	CreativeUI Arrow(20, 20);
	Arrow.fileName = "Resouse\\Texture\\UI\\RightArrow.png";
	RightArrow = Arrow;
	Arrow.fileName = "Resouse\\Texture\\UI\\DownArrow.png";
	DownArrow = Arrow;
	CreativeUI xButton(40, 40);
	xButton.fileName = "Resouse\\Texture\\UI\\XButton.png";
	xButton.scale = VECTOR2(0.5f, 0.5f);
	XButton = xButton;
	CreativeUI CheckBox(20, 20);
	CheckBox.fileName = "Resouse\\Texture\\UI\\CheckBoxOn.png";
	CheckBoxOn = CheckBox;
	CheckBox.fileName = "Resouse\\Texture\\UI\\CheckBoxOff.png";
	CheckBoxOff = CheckBox;
	CreativeText title;
	swprintf(title.Text, MAX_PATH, _T("InputCollider"));
	title.maxWidth = 250;
	title.maxHeight = 25;
	title.minisize = true;
	Title = title;
	swprintf(title.Text, MAX_PATH, _T(" MoveSpeed"));
	MoveSpeedText = title;
	// TextBox
	CreativeUI Box(100, 40);
	Box.fileName = "Resouse\\Texture\\UI\\TextBox.png";

	Box.scale.x = 0.65;
	// ここだけ枠線にかぶるのでBoxを少し小さめにした
	Box.scale.y = 0.5;
	// Text
	CreativeText Text;
	Text.minisize = true;
	Text.maxWidth = 65;
	Text.maxHeight = 24;

	// 3はx,y,z
	MoveSpeed.Text = Text;
	MoveSpeed.Box = Box;
}

void InputUI::SetPos()
{
	MoveSpeedText.position = Bottom.position + VECTOR2(-200, -10);
	MoveSpeed.Box.position = Bottom.position + VECTOR2(35, 0);
	MoveSpeed.Text.position = TextPos(MoveSpeed.Text, MoveSpeed.Box);
}


void InputUI::Update(CreativeUI& SlectTextBox)
{
	if (Open)
	{
		SetText(MoveSpeedText);
		if (MoveSpeed.Select)
		{
			SetUI(SlectTextBox);
		}
		else
		{
			SetUI(MoveSpeed.Box);
		}
		SetText(MoveSpeed.Text);
	}
	else
	{
		RiseDraw();
	}
}

void InputUI::RiseDraw()
{
	RiseText(MoveSpeedText);
	RiseUI(MoveSpeed.Box);
	RiseText(MoveSpeed.Text);
}

// ---------------------- MeshComponent -------------------
void MeshUI::SetMetaData()
{
	CreativeUI TopFrame(300, 25);
	TopFrame.fileName = "Resouse\\Texture\\UI\\ComponentFrame.png";
	Top = TopFrame;
	CreativeUI BottomFrame(300, 100);
	BottomFrame.fileName = "Resouse\\Texture\\UI\\MeshBottom.png";
	Bottom = BottomFrame;
	CreativeUI Arrow(20, 20);
	Arrow.fileName = "Resouse\\Texture\\UI\\RightArrow.png";
	RightArrow = Arrow;
	Arrow.fileName = "Resouse\\Texture\\UI\\DownArrow.png";
	DownArrow = Arrow;
	CreativeUI xButton(40, 40);
	xButton.fileName = "Resouse\\Texture\\UI\\XButton.png";
	xButton.scale = VECTOR2(0.5f, 0.5f);
	XButton = xButton;
	CreativeUI CheckBox(20, 20);
	CheckBox.fileName = "Resouse\\Texture\\UI\\CheckBoxOn.png";
	CheckBoxOn = CheckBox;
	CheckBox.fileName = "Resouse\\Texture\\UI\\CheckBoxOff.png";
	CheckBoxOff = CheckBox;
	CreativeUI button(15, 15);
	button.fileName = "Resouse\\Texture\\UI\\SelectButton.png";
	SelectButton = button;
	CreativeText title;
	swprintf(title.Text, MAX_PATH, _T("Mesh"));
	title.maxWidth = 250;
	title.maxHeight = 25;
	title.minisize = true;
	Title = title;

	// TextBox
	CreativeUI Box(100, 40);
	Box.fileName = "Resouse\\Texture\\UI\\TextBox.png";

	Box.scale.x = 1.3;
	// ここだけ枠線にかぶるのでBoxを少し小さめにした
	Box.scale.y = 0.5;
	// Text
	CreativeText Text;
	Text.minisize = true;
	Text.maxWidth = 130;
	Text.maxHeight = 24;

	SourceImage.Text = Text;
	SourceImage.Box = Box;

	Box.scale.x = 0.45;
	Text.maxWidth = 45;

	// 4はrgba
	for (int i = 0; i < 4; i++)
	{
		if (i == 3)
		{
			Box.scale.x = 0.3;
			Text.maxWidth = 30;
		}
		Color[i].Text = Text;
		Color[i].Box = Box;
	}
}

void MeshUI::SetPos()
{
	SelectButton.position = Bottom.position + VECTOR2(140, -25);
	SourceImage.Box.position = Bottom.position + VECTOR2(65, -25);
	SourceImage.Text.position = TextPos(SourceImage.Text, SourceImage.Box);
	for (int i = 0; i < 4; i++)
	{
		if (i == 0)
		{
			Color[i].Box.position = Bottom.position + VECTOR2(-55, 25);
		}
		if (i == 1)
		{
			Color[i].Box.position = Bottom.position + VECTOR2(10, 25);
		}
		if (i == 2)
		{
			Color[i].Box.position = Bottom.position + VECTOR2(75, 25);
		}
		if (i == 3)
		{
			Color[i].Box.position = Bottom.position + VECTOR2(130, 25);
		}

		Color[i].Text.position = TextPos(Color[i].Text, Color[i].Box);
	}
}

void MeshUI::Update(CreativeUI& SlectTextBox)
{
	if (Open)
	{
		SetUI(SelectButton);
		if (SourceImage.Select)
		{
			SetUI(SlectTextBox);
		}
		else
		{
			SetUI(SourceImage.Box);
		}
		SetText(SourceImage.Text);
		for (int i = 0; i < 4; i++)
		{
			if (Color[i].Select)
			{

				SetUI(SlectTextBox);
			}
			else
			{
				SetUI(Color[i].Box);
			}
			SetText(Color[i].Text);
		}
	}
	else
	{
		RiseDraw();
	}
}

void MeshUI::RiseDraw()
{
	RiseUI(SelectButton);
	RiseUI(SourceImage.Box);
	RiseText(SourceImage.Text);
	for (int i = 0; i < 4; i++)
	{
		RiseUI(Color[i].Box);
		RiseText(Color[i].Text);
	}
}

// ---------------------- ModelComponent -------------------
void ModelUI::SetMetaData()
{
	CreativeUI TopFrame(300, 25);
	TopFrame.fileName = "Resouse\\Texture\\UI\\ComponentFrame.png";
	Top = TopFrame;
	CreativeUI BottomFrame(300, 100);
	BottomFrame.fileName = "Resouse\\Texture\\UI\\MeshBottom.png";
	Bottom = BottomFrame;
	CreativeUI Arrow(20, 20);
	Arrow.fileName = "Resouse\\Texture\\UI\\RightArrow.png";
	RightArrow = Arrow;
	Arrow.fileName = "Resouse\\Texture\\UI\\DownArrow.png";
	DownArrow = Arrow;
	CreativeUI xButton(40, 40);
	xButton.fileName = "Resouse\\Texture\\UI\\XButton.png";
	xButton.scale = VECTOR2(0.5f, 0.5f);
	XButton = xButton;
	CreativeUI CheckBox(20, 20);
	CheckBox.fileName = "Resouse\\Texture\\UI\\CheckBoxOn.png";
	CheckBoxOn = CheckBox;
	CheckBox.fileName = "Resouse\\Texture\\UI\\CheckBoxOff.png";
	CheckBoxOff = CheckBox;
	CreativeUI button(15, 15);
	button.fileName = "Resouse\\Texture\\UI\\SelectButton.png";
	SelectButton = button;
	CreativeText title;
	swprintf(title.Text, MAX_PATH, _T("Model"));
	title.maxWidth = 250;
	title.maxHeight = 25;
	title.minisize = true;
	Title = title;

	// TextBox
	CreativeUI Box(100, 40);
	Box.fileName = "Resouse\\Texture\\UI\\TextBox.png";

	Box.scale.x = 1.3;
	// ここだけ枠線にかぶるのでBoxを少し小さめにした
	Box.scale.y = 0.5;
	// Text
	CreativeText Text;
	Text.minisize = true;
	Text.maxWidth = 130;
	Text.maxHeight = 24;

	SourceImage.Text = Text;
	SourceImage.Box = Box;

	Box.scale.x = 0.45;
	Text.maxWidth = 45;

	// 4はrgba
	for (int i = 0; i < 4; i++)
	{
		if (i == 3)
		{
			Box.scale.x = 0.3;
			Text.maxWidth = 30;
		}
		Color[i].Text = Text;
		Color[i].Box = Box;
	}
}

void ModelUI::SetPos()
{
	SelectButton.position = Bottom.position + VECTOR2(140, -25);
	SourceImage.Box.position = Bottom.position + VECTOR2(65, -25);
	SourceImage.Text.position = TextPos(SourceImage.Text, SourceImage.Box);
	for (int i = 0; i < 4; i++)
	{
		if (i == 0)
		{
			Color[i].Box.position = Bottom.position + VECTOR2(-55, 25);
		}
		if (i == 1)
		{
			Color[i].Box.position = Bottom.position + VECTOR2(10, 25);
		}
		if (i == 2)
		{
			Color[i].Box.position = Bottom.position + VECTOR2(75, 25);
		}
		if (i == 3)
		{
			Color[i].Box.position = Bottom.position + VECTOR2(130, 25);
		}

		Color[i].Text.position = TextPos(Color[i].Text, Color[i].Box);
	}
}

void ModelUI::Update(CreativeUI& SlectTextBox)
{
	if (Open)
	{
		SetUI(SelectButton);
		if (SourceImage.Select)
		{
			SetUI(SlectTextBox);
		}
		else
		{
			SetUI(SourceImage.Box);
		}
		SetText(SourceImage.Text);
		for (int i = 0; i < 4; i++)
		{
			if (Color[i].Select)
			{

				SetUI(SlectTextBox);
			}
			else
			{
				SetUI(Color[i].Box);
			}
			SetText(Color[i].Text);
		}
	}
	else
	{
		RiseDraw();
	}
}

void ModelUI::RiseDraw()
{
	RiseUI(SelectButton);
	RiseUI(SourceImage.Box);
	RiseText(SourceImage.Text);
	for (int i = 0; i < 4; i++)
	{
		RiseUI(Color[i].Box);
		RiseText(Color[i].Text);
	}
}

// ---------------------- BoxColliderComponent -------------------
void BoxColliderUI::SetMetaData()
{
	CreativeUI TopFrame(300, 25);
	TopFrame.fileName = "Resouse\\Texture\\UI\\ComponentFrame.png";
	Top = TopFrame;
	CreativeUI BottomFrame(300, 100);
	BottomFrame.fileName = "Resouse\\Texture\\UI\\Frame300x100.png";
	Bottom = BottomFrame;
	CreativeUI Arrow(20, 20);
	Arrow.fileName = "Resouse\\Texture\\UI\\RightArrow.png";
	RightArrow = Arrow;
	Arrow.fileName = "Resouse\\Texture\\UI\\DownArrow.png";
	DownArrow = Arrow;
	CreativeUI xButton(40, 40);
	xButton.fileName = "Resouse\\Texture\\UI\\XButton.png";
	xButton.scale = VECTOR2(0.5f, 0.5f);
	XButton = xButton;
	CreativeUI CheckBox(20, 20);
	CheckBox.fileName = "Resouse\\Texture\\UI\\CheckBoxOn.png";
	CheckBoxOn = CheckBox;
	CheckBox.fileName = "Resouse\\Texture\\UI\\CheckBoxOff.png";
	CheckBoxOff = CheckBox;
	CreativeText title;
	swprintf(title.Text, MAX_PATH, _T("BoxCollider"));
	title.maxWidth = 250;
	title.maxHeight = 25;
	title.minisize = true;
	Title = title;

	swprintf(title.Text, MAX_PATH, _T("Size   x        y        z"));
	Sizexyz = title;
	swprintf(title.Text, MAX_PATH, _T("Center x       y        z"));
	Centerxyz = title;

	// TextBox
	CreativeUI Box(100, 40);
	Box.fileName = "Resouse\\Texture\\UI\\TextBox.png";

	Box.scale.x = 0.65;
	// ここだけ枠線にかぶるのでBoxを少し小さめにした
	Box.scale.y = 0.5;
	// Text
	CreativeText Text;
	Text.minisize = true;
	Text.maxWidth = 65;
	Text.maxHeight = 24;

	// 3はx,y,z
	for (int i = 0; i < 3; i++)
	{
		Size[i].Text = Text;
		Size[i].Box = Box;
		Center[i].Text = Text;
		Center[i].Box = Box;
	}
}

void BoxColliderUI::SetPos()
{
	Sizexyz.position = Bottom.position + VECTOR2(-160, -30);
	Centerxyz.position = Sizexyz.position + VECTOR2(0, 40);

	for (int i = 0; i < 3; i++)
	{
		if (i == 0)
		{
			Size[i].Box.position = Bottom.position + VECTOR2(-42, -20);
			Center[i].Box.position =Bottom.position + VECTOR2(-42, 20);
		}
		if (i == 1)
		{
			Size[i].Box.position = Bottom.position + VECTOR2(35, -20);
			Center[i].Box.position = Bottom.position + VECTOR2(35, 20);
		}
		if (i == 2)
		{
			Size[i].Box.position = Bottom.position + VECTOR2(115, -20);
			Center[i].Box.position = Bottom.position + VECTOR2(115, 20);
		}

		Center[i].Text.position = TextPos(Center[i].Text, Center[i].Box);
		Size[i].Text.position = TextPos(Size[i].Text, Size[i].Box);
	}
}

void BoxColliderUI::Update(CreativeUI& SlectTextBox)
{
	if (Open)
	{
		SetText(Sizexyz);
		SetText(Centerxyz);
		for (int i = 0; i < 3; i++)
		{
			if (Size[i].Select)
			{
				SetUI(SlectTextBox);
			}
			else
			{
				SetUI(Size[i].Box);
			}
			if (Center[i].Select)
			{

				SetUI(SlectTextBox);
			}
			else
			{
				SetUI(Center[i].Box);
			}

			SetText(Size[i].Text);
			SetText(Center[i].Text);
		}
	}
	else
	{
		RiseDraw();
	}
}

void BoxColliderUI::RiseDraw()
{
	RiseText(Sizexyz);
	RiseText(Centerxyz);
	for (int i = 0; i < 3; i++)
	{
		RiseUI(Size[i].Box);
		RiseUI(Center[i].Box);
		RiseText(Size[i].Text);
		RiseText(Center[i].Text);
	}
}

// ---------------------- CylinderColliderComponent -------------------
void CylinderColliderUI::SetMetaData()
{

}

// ---------------------- CapsuleColliderComponent -------------------
void CapsuleColliderUI::SetMetaData()
{
	CreativeUI TopFrame(300, 25);
	TopFrame.fileName = "Resouse\\Texture\\UI\\ComponentFrame.png";
	Top = TopFrame;
	CreativeUI BottomFrame(300, 100);
	BottomFrame.fileName = "Resouse\\Texture\\UI\\Frame300x100.png";
	Bottom = BottomFrame;
	CreativeUI Arrow(20, 20);
	Arrow.fileName = "Resouse\\Texture\\UI\\RightArrow.png";
	RightArrow = Arrow;
	Arrow.fileName = "Resouse\\Texture\\UI\\DownArrow.png";
	DownArrow = Arrow;
	CreativeUI xButton(40, 40);
	xButton.fileName = "Resouse\\Texture\\UI\\XButton.png";
	xButton.scale = VECTOR2(0.5f, 0.5f);
	XButton = xButton;
	CreativeUI CheckBox(20, 20);
	CheckBox.fileName = "Resouse\\Texture\\UI\\CheckBoxOn.png";
	CheckBoxOn = CheckBox;
	CheckBox.fileName = "Resouse\\Texture\\UI\\CheckBoxOff.png";
	CheckBoxOff = CheckBox;
	CreativeText title;
	swprintf(title.Text, MAX_PATH, _T("CapsuleCollider"));
	title.maxWidth = 250;
	title.maxHeight = 25;
	title.minisize = true;
	Title = title;

	swprintf(title.Text, MAX_PATH, _T("Height"));
	HeightText = title;
	swprintf(title.Text, MAX_PATH, _T("Radius"));
	RadiusText = title;
	swprintf(title.Text, MAX_PATH, _T("Center x       y        z"));
	Centerxyz = title;

	// TextBox
	CreativeUI Box(100, 40);
	Box.fileName = "Resouse\\Texture\\UI\\TextBox.png";

	Box.scale.x = 0.65;
	// ここだけ枠線にかぶるのでBoxを少し小さめにした
	Box.scale.y = 0.5;
	// Text
	CreativeText Text;
	Text.minisize = true;
	Text.maxWidth = 65;
	Text.maxHeight = 24;

	Height.Text = Text;
	Height.Box = Box;
	Radius.Text = Text;
	Radius.Box = Box;
	// 3はx,y,z
	for (int i = 0; i < 3; i++)
	{
		Center[i].Text = Text;
		Center[i].Box = Box;
	}
}

void CapsuleColliderUI::SetPos()
{
	HeightText.position = Bottom.position + VECTOR2(-240, -30);
	RadiusText.position = Bottom.position + VECTOR2(-90, -30);
	Centerxyz.position = Bottom.position + VECTOR2(-160, 10);

	Height.Box.position = Bottom.position + VECTOR2(-42, -20);
	Height.Text.position = TextPos(Height.Text, Height.Box);
	Radius.Box.position = Bottom.position + VECTOR2(115, -20);
	Radius.Text.position = TextPos(Radius.Text, Radius.Box);
	for (int i = 0; i < 3; i++)
	{
		if (i == 0)
		{
			Center[i].Box.position = Bottom.position + VECTOR2(-42, 20);
		}
		if (i == 1)
		{
			Center[i].Box.position = Bottom.position + VECTOR2(35, 20);
		}
		if (i == 2)
		{
			Center[i].Box.position = Bottom.position + VECTOR2(115, 20);
		}

		Center[i].Text.position = TextPos(Center[i].Text, Center[i].Box);
	}
}

void CapsuleColliderUI::Update(CreativeUI& SlectTextBox)
{
	if (Open)
	{
		SetText(HeightText);
		SetText(RadiusText);
		SetText(Centerxyz);
		if (Height.Select)
		{
			SetUI(SlectTextBox);
		}
		else
		{
			SetUI(Height.Box);
		}
		SetText(Height.Text);
		if (Radius.Select)
		{
			SetUI(SlectTextBox);
		}
		else
		{
			SetUI(Radius.Box);
		}
		SetText(Radius.Text);
		for (int i = 0; i < 3; i++)
		{
			if (Center[i].Select)
			{

				SetUI(SlectTextBox);
			}
			else
			{
				SetUI(Center[i].Box);
			}
			SetText(Center[i].Text);
		}
	}
	else
	{
		RiseDraw();
	}
}

void CapsuleColliderUI::RiseDraw()
{
	RiseText(HeightText);
	RiseText(RadiusText);
	RiseText(Centerxyz);
	RiseUI(Height.Box);
	RiseText(Height.Text);
	RiseUI(Radius.Box);
	RiseText(Radius.Text);
	for (int i = 0; i < 3; i++)
	{
		RiseUI(Center[i].Box);
		RiseText(Center[i].Text);
	}
}

// ---------------------- RigidBodyColliderComponent -------------------
void RigidBodyUI::SetMetaData()
{
	CreativeUI TopFrame(300, 25);
	TopFrame.fileName = "Resouse\\Texture\\UI\\ComponentFrame.png";
	Top = TopFrame;
	CreativeUI BottomFrame(300, 100);
	BottomFrame.fileName = "Resouse\\Texture\\UI\\Frame300x100.png";
	Bottom = BottomFrame;
	CreativeUI Arrow(20, 20);
	Arrow.fileName = "Resouse\\Texture\\UI\\RightArrow.png";
	RightArrow = Arrow;
	Arrow.fileName = "Resouse\\Texture\\UI\\DownArrow.png";
	DownArrow = Arrow;
	CreativeUI xButton(40, 40);
	xButton.fileName = "Resouse\\Texture\\UI\\XButton.png";
	xButton.scale = VECTOR2(0.5f, 0.5f);
	XButton = xButton;
	CreativeUI CheckBox(20, 20);
	CheckBox.fileName = "Resouse\\Texture\\UI\\CheckBoxOn.png";
	CheckBoxOn = CheckBox;
	CheckBox.fileName = "Resouse\\Texture\\UI\\CheckBoxOff.png";
	CheckBoxOff = CheckBox;
	CreativeText title;
	swprintf(title.Text, MAX_PATH, _T("RigidBody"));
	title.maxWidth = 250;
	title.maxHeight = 25;
	title.minisize = true;
	Title = title;

	// ここからこのコンポーネント固有の記述

}

void RigidBodyUI::SetPos()
{

}

void RigidBodyUI::Update(CreativeUI& SlectTextBox)
{

}

void RigidBodyUI::RiseDraw()
{

}

// ---------------------- ImageComponent -------------------
void ImageUI::SetMetaData()
{
	CreativeUI TopFrame(300, 25);
	TopFrame.fileName = "Resouse\\Texture\\UI\\ComponentFrame.png";
	Top = TopFrame;
	CreativeUI BottomFrame(300, 150);
	BottomFrame.fileName = "Resouse\\Texture\\UI\\ImageBottom.png";
	Bottom = BottomFrame;
	CreativeUI Arrow(20, 20);
	Arrow.fileName = "Resouse\\Texture\\UI\\RightArrow.png";
	RightArrow = Arrow;
	Arrow.fileName = "Resouse\\Texture\\UI\\DownArrow.png";
	DownArrow = Arrow;
	CreativeUI xButton(40, 40);
	xButton.fileName = "Resouse\\Texture\\UI\\XButton.png";
	xButton.scale = VECTOR2(0.5f, 0.5f);
	XButton = xButton;
	CreativeUI CheckBox(20, 20);
	CheckBox.fileName = "Resouse\\Texture\\UI\\CheckBoxOn.png";
	CheckBoxOn = CheckBox;
	CheckBox.fileName = "Resouse\\Texture\\UI\\CheckBoxOff.png";
	CheckBoxOff = CheckBox;
	CreativeUI button(15, 15);
	button.fileName = "Resouse\\Texture\\UI\\SelectButton.png";
	SelectButton = button;
	CreativeText title;
	swprintf(title.Text, MAX_PATH, _T("Image"));
	title.maxWidth = 250;
	title.maxHeight = 25;
	title.minisize = true;
	Title = title;

	// TextBox
	CreativeUI Box(100, 40);
	Box.fileName = "Resouse\\Texture\\UI\\TextBox.png";
	
	Box.scale.x = 1.3;
	// ここだけ枠線にかぶるのでBoxを少し小さめにした
	Box.scale.y = 0.5;
	// Text
	CreativeText Text;
	Text.minisize = true;
	Text.maxWidth = 130;
	Text.maxHeight = 24;

	SourceImage.Text = Text;
	SourceImage.Box = Box;
	OrderinLayer.Text = Text;
	OrderinLayer.Box = Box;

	Box.scale.x = 0.45;
	Text.maxWidth = 45;

	// 4はrgba
	for (int i = 0; i < 4; i++)
	{
		if (i == 3)
		{
			Box.scale.x = 0.3;
			Text.maxWidth = 30;
		}
		Color[i].Text = Text;
		Color[i].Box = Box;
	}
}

void ImageUI::SetPos()
{
	SelectButton.position = Bottom.position + VECTOR2(140, -45);
	SourceImage.Box.position = Bottom.position + VECTOR2(65, -45);
	SourceImage.Text.position = TextPos(SourceImage.Text, SourceImage.Box);
	OrderinLayer.Box.position = Bottom.position + VECTOR2(65, 50);
	OrderinLayer.Text.position = TextPos(OrderinLayer.Text, OrderinLayer.Box);
	for (int i = 0; i < 4; i++)
	{
		if (i == 0)
		{
			Color[i].Box.position = Bottom.position + VECTOR2(-55, 2);
		}
		if (i == 1)
		{
			Color[i].Box.position = Bottom.position + VECTOR2(10, 2);
		}
		if (i == 2)
		{
			Color[i].Box.position = Bottom.position + VECTOR2(75, 2);
		}
		if (i == 3)
		{
			Color[i].Box.position = Bottom.position + VECTOR2(130, 2);
		}

		Color[i].Text.position = TextPos(Color[i].Text, Color[i].Box);
	}
}

void ImageUI::Update(CreativeUI& SlectTextBox)
{
	if (Open)
	{
		SetUI(SelectButton);
		if (SourceImage.Select)
		{
			SetUI(SlectTextBox);
		}
		else
		{
			SetUI(SourceImage.Box);
		}
		SetText(SourceImage.Text);
		if (OrderinLayer.Select)
		{
			SetUI(SlectTextBox);
		}
		else
		{
			SetUI(OrderinLayer.Box);
		}
		SetText(OrderinLayer.Text);
		for (int i = 0; i < 4; i++)
		{
			if (Color[i].Select)
			{

				SetUI(SlectTextBox);
			}
			else
			{
				SetUI(Color[i].Box);
			}
			SetText(Color[i].Text);
		}
	}
	else
	{
		RiseDraw();
	}
}

void ImageUI::RiseDraw()
{
	RiseUI(SelectButton);
	RiseUI(SourceImage.Box);
	RiseText(SourceImage.Text);
	RiseUI(OrderinLayer.Box);
	RiseText(OrderinLayer.Text);
	for (int i = 0; i < 4; i++)
	{
		RiseUI(Color[i].Box);
		RiseText(Color[i].Text);
	}
}

// ---------------------- ScriptComponent -------------------
void ScriptUI::SetMetaData()
{
	CreativeUI TopFrame(300, 25);
	TopFrame.fileName = "Resouse\\Texture\\UI\\ComponentFrame.png";
	Top = TopFrame;
	CreativeUI xButton(40, 40);
	xButton.fileName = "Resouse\\Texture\\UI\\XButton.png";
	xButton.scale = VECTOR2(0.5f, 0.5f);
	XButton = xButton;
	CreativeUI CheckBox(20, 20);
	CheckBox.fileName = "Resouse\\Texture\\UI\\CheckBoxOn.png";
	CheckBoxOn = CheckBox;
	CheckBox.fileName = "Resouse\\Texture\\UI\\CheckBoxOff.png";
	CheckBoxOff = CheckBox;
	Title.maxWidth = 250;
	Title.maxHeight = 25;
	Title.minisize = true;

	// 押さないように位置を画面外にする
	Bottom.position = VECTOR2(-100, 0);
	RightArrow.position = VECTOR2(-100, 0);
	DownArrow.position = VECTOR2(-100, 0);
	Open = false;
}

void ScriptUI::SetPos()
{

}

void ScriptUI::Update(CreativeUI& SlectTextBox)
{

}

void ScriptUI::RiseDraw()
{

}


// --------------------- 関数 -------------------------
void SetUI(CreativeUI& ui)
{
	Texture::GetInstance()->SetTextureMetaData(
		ui.entryPos,								// シェーダーに送るインスタンス番号（-1ならまだ決まっていない）
		VECTOR(ui.position.x, ui.position.y, 0),	// 位置（左上が原点）
		VECTOR(0, 0, ui.angle),						// 回転（０～３６０度）
		VECTOR(ui.scale.x, ui.scale.y, 0),			// 大きさ(1なら画像の元サイズと同じ)
		VECTOR(1,1,1),								// 色
		1,											// α
		ui.drawOrder,								// 描画順
		true,										// UI化する
		ui.fileName);
}

void RiseUI(CreativeUI& ui)
{
	Texture::GetInstance()->TextureDeleate(ui.entryPos, ui.fileName);
}

void SetText(CreativeText& text)
{
	DX->SetTextLayout(
		text.entryPos,								// -1なら初期作成、それ以外なら更新
		text.position,								// クライアント座標
		text.color,									// 色（r,g,b）
		text.Text,									// 表示したいテキスト
		text.maxWidth,								// 最大幅
		text.maxHeight,								// 最大高さ
		text.underLine,								// 下線の有無
		text.fontWeigh,								// フォントの太さ
		text.minisize								// 小さめのフォントを使う
	);
}

void RiseText(CreativeText& text)
{
	DX->TextLayoutDeleate(text.entryPos);
}
// Textを表示するUIを基準にしてTextの位置取得
VECTOR2 TextPos(CreativeText text, CreativeUI ui)
{
	// left*scale.xとtop*scale.yを足している。
	return text.position = ui.position + VECTOR2(ui.left * ui.scale.x, ui.top * ui.scale.y);
}
