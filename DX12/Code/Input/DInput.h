#ifndef __DINPUT_H__
#define __DINPUT_H__
#pragma once
// -------------------- Define宣言 -------------------
#define DIRECTINPUT_VERSION 0x0800			// DirectInputVersion設定(DX8.0以降)
#define BUFFER_SIZE		256					// バッファサイズ
#define KEY_ENTER(p)	DInput::GetInstance()->GetKeyEnter(p)	// 押したとき
#define KEY(p)			DInput::GetInstance()->GetKey(p)		// 押してるとき
#define KEY_EXIT(p)		DInput::GetInstance()->GetKeyExit(p)	// 離したとき
#define MOUSE_ENTER(p)	DInput::GetInstance()->GetMouseEnter(p)	// 押したとき
#define MOUSE(p)		DInput::GetInstance()->GetMouse(p)		// 押してるとき
#define MOUSE_EXIT(p)	DInput::GetInstance()->GetMouseExit(p)	// 離したとき
// -------------------- IncludeFile ------------------
#include <dinput.h>
#include "../Global.h"
// -------------------- LibraryFile ------------------
#pragma comment(lib,"dinput8.lib")
// ---------------------- 定数式宣言 ---------------------
auto constexpr MouseButton = 4;	// 読み込む画像数
// -------------------- キー情報配列 -----------------
// 参考：DirectInput キー識別コード表 http://www.flint.jp/misc/?q=dik
// // キー情報
// 指定はこっちを使う
enum KEY_INFO
{
	ESK_KEY,		// ESCキー
	NUM1_KEY,		// 1キー
	NUM2_KEY,		// 2キー
	NUM3_KEY,		// 3キー
	NUM4_KEY,		// 4キー
	NUM5_KEY,		// 5キー
	NUM6_KEY,		// 6キー
	NUM7_KEY,		// 7キー
	NUM8_KEY,		// 8キー
	NUM9_KEY,		// 9キー
	// 10
	NUM0_KEY,		// 0キー
	MINUS_KEY,		// -キー
	EQUALS_KEY,		// =キー
	BACK_KEY,		// BackSpaceキー
	TAB_KEY,		// Tabキー
	Q_KEY,			// Qキー
	W_KEY,			// Wキー
	E_KEY,			// Eキー
	R_KEY,			// Rキー
	T_KEY,			// Tキー
	// 20
	Y_KEY,			// Yキー
	U_KEY,			// Uキー
	I_KEY,			// Iキー
	O_KEY,			// Oキー
	P_KEY,			// Pキー
	LBRACKET_KEY,	// [キー
	RBRACKET_KEY,	// ]キー
	RETURN_KEY,		// Enterキー
	LCONTROL_KEY,	// Ctrl(左)キー
	A_KEY,			// Aキー
	// 30
	S_KEY,			// Sキー
	D_KEY,			// Dキー
	F_KEY,			// Fキー
	G_KEY,			// Gキー
	H_KEY,			// Hキー
	J_KEY,			// Jキー
	K_KEY,			// Kキー
	L_KEY,			// Lキー
	SEMICOLON_KEY,	// ;キー
	APOSTROPHE_KEY,	// 'キー
	// 40
	GRAVE_KEY,		// `キー
	LSHIFT_KEY,		// Shiht(左)キー
	BACKSLASH_KEY,	// \キー
	Z_KEY,			// Zキー
	X_KEY,			// Xキー
	C_KEY,			// Cキー
	V_KEY,			// Vキー
	B_KEY,			// Bキー
	N_KEY,			// Nキー
	M_KEY,			// Mキー
	// 50
	COMMA_KEY,		// ,キー
	PERIOD_KEY,		// .キー
	SLASH_KEY,		// /キー
	RSHIFT_KEY,		// Shift(右)キー
	MULTIPLY_KEY,	// * (Numpad)キー
	LMENU_KEY,		// Alt (左)キー
	SPACE_KEY,		// Spaceキー
	CAPITAL_KEY,	// Caps Lockキー
	F1_KEY,			// F1キー
	F2_KEY,			// F2キー
	// 60
	F3_KEY,			// F3キー
	F4_KEY,			// F4キー
	F5_KEY,			// F5キー
	F6_KEY,		    // F6キー
	F7_KEY,		    // F7キー
	F8_KEY,		    // F8キー
	F9_KEY,		    // F9キー
	F10_KEY,	    // F10キー
	NUMLOCK_KEY,	// NumLockキー
	SCROLL_KEY,		// ScrollLockキー
	// 70
	NUMPAD7_KEY,	// 7(Numpad)キー
	NUMPAD8_KEY,	// 8(Numpad)キー
	NUMPAD9_KEY,	// 9(Numpad)キー
	SUBTRACT_KEY,	// -(Numpad)キー
	NUMPAD4_KEY,	// 4(Numpad)キー
	NUMPAD5_KEY,	// 5(Numpad)キー
	NUMPAD6_KEY,	// 6(Numpad)キー
	ADD_KEY,		// +(Numpad)キー
	NUMPAD1_KEY,	// 1(Numpad)キー
	NUMPAD2_KEY,	// 2(Numpad)キー
	// 80
	NUMPAD3_KEY,	// 3(Numpad)キー
	NUMPAD0_KEY,	// 0(Numpad)キー
	DECIMAL_KEY,	// .(Numpad)キー
	F11_KEY,		// F11キー
	F12_KEY,		// F12キー
	F13_KEY,		// F13キー
	F14_KEY,		// F14キー
	F15_KEY,		// F15キー
	KANA_KEY,		// カナキー
	CONVERT_KEY,	// 変換キー
	// 90
	NOCONVERT_KEY,	// 無変換キー
	YEN_KEY,		// \キー
	NUMPADEQUALS_KEY,	// =キー
	CIRCUMFLEX_KEY,	// ^キー
	AT_KEY,			// @キー
	COLON_KEY,		// :キー
	UNDERLINE_KEY,	// _キー
	KANJI_KEY,		// 漢字キー
	STOP_KEY,		// Stopキー
	AX_KEY,			// (Japan AX)キー
	// 100
	UNLABELED_KEY,	// (J3100)キー
	NUMPADENTER_KEY,// Enter (Numpad)キー
	RCONTROL_KEY,	// Ctrl (右)キー
	NUMPADCOMMA_KEY,// ,(Numpad)キー
	DIVIDE_KEY,		// /(Numpad)キー
	SYSRQ_KEY,		// Sys Rqキー
	RMENU_KEY,		// Alt (右)キー
	PAUSE_KEY,		// Pauseキー
	HOME_KEY,		// Homeキー
	UP_KEY,			// ↑キー
	// 110
	PRIOR_KEY,		// 	Page Upキー
	LEFT_KEY,		// 左キー
	RIGHT_KEY,		// 右キー
	END_KEY,		// Endキー
	DOWN_KEY,		// 下キー
	NEXT_KEY,		// 	Page Downキー
	INSERT_KEY,		// 	Insertキー
	DELETE_KEY,		// 	Deleteキー
	LWIN_KEY,		// 	Windowsキー
	RWIN_KEY,		// 	Windowsキ
	APPS_KEY,		// 	Menuキー
	POWER_KEY,		// 	Powerキー
	SLEEP_KEY,		// 	Windowsキー

	MAX_KEY_INFO,	// キー状態最大
};
// 定数はDirectInputで用意されているDIK_～を使用する
static int g_KeyInfo[] =
{
	DIK_ESCAPE,
	DIK_1,
	DIK_2,
	DIK_3,
	DIK_4,
	DIK_5,
	DIK_6,
	DIK_7,
	DIK_8,
	DIK_9,
	// 10
	DIK_0,
	DIK_MINUS,
	DIK_EQUALS,
	DIK_BACK,
	DIK_TAB,
	DIK_Q,
	DIK_W,
	DIK_E,
	DIK_R,
	DIK_T,
	// 20
	DIK_Y,
	DIK_U,
	DIK_I,
	DIK_O,
	DIK_P,
	DIK_LBRACKET,
	DIK_RBRACKET,
	DIK_RETURN,
	DIK_LCONTROL,
	DIK_A,
	// 30
	DIK_S,
	DIK_D,
	DIK_F,
	DIK_G,
	DIK_H,
	DIK_J,
	DIK_K,
	DIK_L,
	DIK_SEMICOLON,
	DIK_APOSTROPHE,
	// 40
	DIK_GRAVE,
	DIK_LSHIFT,
	DIK_BACKSLASH,
	DIK_Z,
	DIK_X,
	DIK_C,
	DIK_V,
	DIK_B,
	DIK_N,
	DIK_M,
	// 50
	DIK_COMMA,
	DIK_PERIOD,
	DIK_SLASH,
	DIK_RSHIFT,
	DIK_MULTIPLY,
	DIK_LMENU,
	DIK_SPACE,
	DIK_CAPITAL,
	DIK_F1,
	DIK_F2,
	// 60
	DIK_F3,
	DIK_F4,
	DIK_F5,
	DIK_F6,
	DIK_F7,
	DIK_F8,
	DIK_F9,
	DIK_F10,
	DIK_NUMLOCK,
	DIK_SCROLL,
	// 70
	DIK_NUMPAD7,
	DIK_NUMPAD8,
	DIK_NUMPAD9,
	DIK_SUBTRACT,
	DIK_NUMPAD4,
	DIK_NUMPAD5,
	DIK_NUMPAD6,
	DIK_ADD,
	DIK_NUMPAD1,
	DIK_NUMPAD2,
	// 80
	DIK_NUMPAD3,
	DIK_NUMPAD0,
	DIK_DECIMAL,
	DIK_F11,
	DIK_F12,
	DIK_F13,
	DIK_F14,
	DIK_F15,
	DIK_KANA,
	DIK_CONVERT,
	// 90
	DIK_NOCONVERT,
	DIK_YEN,
	DIK_NUMPADEQUALS,
	DIK_CIRCUMFLEX,
	DIK_AT,
	DIK_COLON,
	DIK_UNDERLINE,
	DIK_KANJI,
	DIK_STOP,
	DIK_AX,
	// 100
	DIK_UNLABELED,
	DIK_NUMPADENTER,
	DIK_RCONTROL,
	DIK_NUMPADCOMMA,
	DIK_DIVIDE,
	DIK_SYSRQ,
	DIK_RMENU,
	DIK_PAUSE,
	DIK_HOME,
	DIK_UP,
	//110
	DIK_PRIOR,
	DIK_LEFT,
	DIK_RIGHT,
	DIK_END,
	DIK_DOWN,
	DIK_NEXT,
	DIK_INSERT,
	DIK_DELETE,
	DIK_LWIN,
	DIK_RWIN,
	DIK_APPS,
	DIK_POWER,
	DIK_SLEEP,
};
// -------------------- クラス宣言 -------------------
class DInput
{
private:
	// キーの状態
	enum INPUT_STATE
	{
		NOT_PUSH,	// 押されてない
		PUSH_ENTER,	// 押された瞬間
		PUSH,		// 押されてる
		PUSH_EXIT,	// 離された瞬間
	};

private:
	static DInput* m_pInstance;			// シングルトン
	LPDIRECTINPUT8				m_pDI;					// DirectInputシステム
	LPDIRECTINPUTDEVICE8		m_pDIKeyboard;			// DirectInputDeviceキーボードコネクタ
	LPDIRECTINPUTDEVICE8		m_pDIMouse;				// DirectInputDeviceマウスコネクタ
	BYTE						m_KeyState[BUFFER_SIZE];// キー入力格納用配列
	DIMOUSESTATE2				m_MouseState;			// マウス情報
	INPUT_STATE g_InputState[KEY_INFO::MAX_KEY_INFO];	// キー状態配列
	INPUT_STATE g_MouseState[MouseButton];				// マウス状態配列
public:
	DInput(HWND hWnd, HINSTANCE hInstance);
	virtual ~DInput();
	static void Create(HWND hWnd, HINSTANCE hInstance);	// 生成
	static void Destroy();								// 破棄
	void UpdateKeyboardState();							// キーボード入力情報の更新
	void UpdateMouseState();							// マウス入力情報の更新
	BYTE* GetKeyboardState() { return m_KeyState; }// キー入力情報の取得
	LPDIMOUSESTATE2 GetMouseState() { return &m_MouseState; }// マウス情報の取得
	static DInput* GetInstance() { return m_pInstance; }
	
	// キー状態の更新
	void KeyStateUpdate();
	// キーが押された瞬間の判定
	bool GetKeyEnter(KEY_INFO key_info);
	// キーが押されてるかの判定
	bool GetKey(KEY_INFO key_info);
	// キーが離された瞬間かの判定
	bool GetKeyExit(KEY_INFO key_info);
	// 全キーの内どれか押された瞬間の判定
	bool GetAllKeyEnter();
	// 全キーの内どれか押されているかの判定
	bool GetAllKey();

	// マウス状態の更新
	void MouseStateUpdate();
	// マウスが押された瞬間の判定
	bool GetMouseEnter(int mouse_info);
	// マウスが押されてるかの判定
	bool GetMouse(int mouse_info);
	// マウスが離された瞬間かの判定
	bool GetMouseExit(int mouse_info);
private:
	void InitDirectInput(HWND hWnd, HINSTANCE hInstance);// DirectInputの初期化
	void ReleaseDirectInput();							// DirectInputオブジェクト解放
};
#endif	/* __DINPUT_H__ */
