#pragma once
// --------------------- Include File --------------------
#include "ObjectCreateStruct.h"

class ObjectCreate
{
private:
	// mMeshNameの配列番号
	enum MeshName
	{
		Box,
		Cylinder,
		Sphere
	};
private:
	// ---------------- メンバ変数宣言 ----------------
	// --------- ウィンドウ -------------
	//Gameクラスのpublicメンバにアクセスするポインタ
	class Game* mGame;
	HierarchyWindow mHierarchyWindow;
	InspectorWindow mInspectorWindow;
	AddObjWindow mAddObjWindow;
	AddComponentWindow mAddCW;// CWはComponentWindowの略
	SelectWindow mSelectWindow;

	CreativeUI mPlayButton;
	CreativeUI mTitleButton;
	CreativeUI mSaveButton;

	// --------- コンポーネント -------------

	std::vector<class Actor*> mActor;	// Actorクラスで表示(HierarchyButtonの順番に対応)
	std::map<std::wstring, int> mActorName;	// Actorの名前と番号対応
	std::vector<std::wstring> mUseComponent;	// 選択中のオブジェクトが使えるコンポーネント
	std::vector<StructComponent*> mSelectComponent;	// 表示するコンポーネント格納
	std::map<std::wstring, int>mObjNum; // それぞれのオブジェクトの数
	std::wstring mModelFolderPath;			// どのフォルダを読み込むか指定
	std::wstring mImageFolderPath;			// どのフォルダを読み込むか指定
	std::vector<std::wstring> mMeshName;	// Meshの名前
	std::vector<std::wstring> mModelName;	// 読み込んだモデルの名前
	std::vector<std::wstring> mImageName;	// 読み込んだ画像の名前
	std::vector<std::wstring> mSimpleModelName;	// 表示する拡張子を消したモデル名
	std::vector<std::wstring> mSimpleImageName;	// 表示する拡張子を消した画像名
	std::vector<std::string> mFolderModelName; // フォルダ名とモデル名が繋がったもの
	std::vector<std::string> mFolderImageName; // フォルダ名と画像名が繋がったもの
	std::vector<VECTOR> mSavePosition;			// Play前の位置保存
	std::vector<VECTOR> mSaveRotation;			// Play前の回転保存

	HierarchyButton* mSelectButton;	// 選択されているボタン
	HierarchyButton* mPreviousSelectButton;			// 前のフレームに選択されていたオブジェクトボタン
	int mSelectActorNum;			// 選択されているActorの番号
	int mSelectButtonNum;			// 選択されたボタンの番号
	int mButtonNum;					// 全ボタンの数
	int mSelectComponentNum;			// 選択中ActorのComponent数
	bool mPutOnWindow;					// カーソルが全ウィンドウと重なっているか
	bool mOpenSelectWindow;					// SelectWindowが表示されているか

	POINT mMousePoint;
	RECT mClientRect;

	int mNumOnlySize = 0; // -と.以外なら増やす
	const int mColorNumMax = 3;			// 色数
	// 横幅変数
	float mClientWidth;
	// 縦幅変数
	float mClientHeight;
	// マウス座標変数
	float mMousePosX;
	float mMousePosY;

	bool mTitleTrans = false;
	bool mPlayTrans = false;

	// ObjectDrag関数用変数
	// あたったかどうか
	bool mHit = false;
	// 調べているActorの番号
	int mAddNum = 0;
	// imageSize
	float mWidthHalf = 0;
	float mHeightHalf = 0;
	// 入力時のBackSpace長押し時間
	float mInputBackSpace = 0;
	bool mBackSpace = false;
	// Tabが入力されたら
	bool mTab = false;

public:
	// ---------------- メンバ関数宣言 ----------------	
	ObjectCreate(class Game* game);
	~ObjectCreate();

	void LoadFile();
	void Init();
	void ProcessInput();
	void UpdateGame();

	void DeleteUI();		// シーンが変わるときにUI描画削除

	bool GetPutOnUI() { return mPutOnWindow; }
	bool RectOnMouse(CreativeUI ui);		// マウスとuiの矩形との当たり判定

	bool GetTitleTrans() { return mTitleTrans; }	// タイトルに変移する
	bool GetPlayTrans() { return mPlayTrans; }		// プレイ画面に変移する

	bool Get2DMode() { return mHierarchyWindow.m2D; }	// trueなら2Dモード,falseなら3Dモード
private:
	void SetDrawOrder();											// 描画順番を決める

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

	void AddObj(std::wstring objName);								// オブジェクトの追加
	void DeleteObj();							// オブジェクト削除
	void AddComponent(class Actor* actor, std::wstring componentName, const char* path = "");				// コンポーネントの追加
	void InputComponentData(StructComponent* component);		// それぞれのコンポーネントの入力処理
	void DeleteComponent(int num);								// コンポーネント削除
	void RiseInspector();									// インスペクター非表示

	std::wstring Input(const TCHAR* text, bool numOnly = false, int maxSize = MAX_PATH);		// 入力(maxSize:取得できる最大文字列、numOnly:trueで数字だけ取得)

	bool GetModelFileNames(std::wstring folderPath, std::vector<std::wstring>& file_names);			// 指定のモデルフォルダパスの中身を調べる
	bool GetImageFileNames(std::wstring folderPath, std::vector<std::wstring>& file_names);			// 指定の画像フォルダパスの中身を調べる
	void LoadText(const TCHAR filename[]);														// textファイルをロード
	void SaveText();																			// textファイルにセーブ

	VECTOR2 InspectorFramePos(CreativeUI nowFrame, CreativeUI PrevFrame);						// 上にあるフレームを基準にして今のフレームの位置取得
	void TextBoxSelect(TextBox& textBox);														// TextBoxの選択
	void TextBoxInput(TextBox& textBox, int& value);											// TextBoxの入力処理(int)
	void TextBoxInput(TextBox& textBox, float& value);											// TextBoxの入力処理(float)
	void TabClickInput(TextBox& nowSelect, TextBox& nextSelect);										// 今選択中のTextBoxから次のTextBoxに移る

	bool HitCheckPolygonLine(std::vector<VECTOR> vertices, VECTOR rayStartPos, VECTOR rayEndPos, VECTOR& crossPoint);	// ポリゴンと線の衝突
};

