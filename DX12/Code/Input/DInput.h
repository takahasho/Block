#ifndef __DINPUT_H__
#define __DINPUT_H__
#pragma once
// -------------------- Define�錾 -------------------
#define DIRECTINPUT_VERSION 0x0800			// DirectInputVersion�ݒ�(DX8.0�ȍ~)
#define BUFFER_SIZE		256					// �o�b�t�@�T�C�Y
#define KEY_ENTER(p)	DInput::GetInstance()->GetKeyEnter(p)	// �������Ƃ�
#define KEY(p)			DInput::GetInstance()->GetKey(p)		// �����Ă�Ƃ�
#define KEY_EXIT(p)		DInput::GetInstance()->GetKeyExit(p)	// �������Ƃ�
#define MOUSE_ENTER(p)	DInput::GetInstance()->GetMouseEnter(p)	// �������Ƃ�
#define MOUSE(p)		DInput::GetInstance()->GetMouse(p)		// �����Ă�Ƃ�
#define MOUSE_EXIT(p)	DInput::GetInstance()->GetMouseExit(p)	// �������Ƃ�
// -------------------- IncludeFile ------------------
#include <dinput.h>
#include "../Global.h"
// -------------------- LibraryFile ------------------
#pragma comment(lib,"dinput8.lib")
// ---------------------- �萔���錾 ---------------------
auto constexpr MouseButton = 4;	// �ǂݍ��މ摜��
// -------------------- �L�[���z�� -----------------
// �Q�l�FDirectInput �L�[���ʃR�[�h�\ http://www.flint.jp/misc/?q=dik
// // �L�[���
// �w��͂��������g��
enum KEY_INFO
{
	ESK_KEY,		// ESC�L�[
	NUM1_KEY,		// 1�L�[
	NUM2_KEY,		// 2�L�[
	NUM3_KEY,		// 3�L�[
	NUM4_KEY,		// 4�L�[
	NUM5_KEY,		// 5�L�[
	NUM6_KEY,		// 6�L�[
	NUM7_KEY,		// 7�L�[
	NUM8_KEY,		// 8�L�[
	NUM9_KEY,		// 9�L�[
	// 10
	NUM0_KEY,		// 0�L�[
	MINUS_KEY,		// -�L�[
	EQUALS_KEY,		// =�L�[
	BACK_KEY,		// BackSpace�L�[
	TAB_KEY,		// Tab�L�[
	Q_KEY,			// Q�L�[
	W_KEY,			// W�L�[
	E_KEY,			// E�L�[
	R_KEY,			// R�L�[
	T_KEY,			// T�L�[
	// 20
	Y_KEY,			// Y�L�[
	U_KEY,			// U�L�[
	I_KEY,			// I�L�[
	O_KEY,			// O�L�[
	P_KEY,			// P�L�[
	LBRACKET_KEY,	// [�L�[
	RBRACKET_KEY,	// ]�L�[
	RETURN_KEY,		// Enter�L�[
	LCONTROL_KEY,	// Ctrl(��)�L�[
	A_KEY,			// A�L�[
	// 30
	S_KEY,			// S�L�[
	D_KEY,			// D�L�[
	F_KEY,			// F�L�[
	G_KEY,			// G�L�[
	H_KEY,			// H�L�[
	J_KEY,			// J�L�[
	K_KEY,			// K�L�[
	L_KEY,			// L�L�[
	SEMICOLON_KEY,	// ;�L�[
	APOSTROPHE_KEY,	// '�L�[
	// 40
	GRAVE_KEY,		// `�L�[
	LSHIFT_KEY,		// Shiht(��)�L�[
	BACKSLASH_KEY,	// \�L�[
	Z_KEY,			// Z�L�[
	X_KEY,			// X�L�[
	C_KEY,			// C�L�[
	V_KEY,			// V�L�[
	B_KEY,			// B�L�[
	N_KEY,			// N�L�[
	M_KEY,			// M�L�[
	// 50
	COMMA_KEY,		// ,�L�[
	PERIOD_KEY,		// .�L�[
	SLASH_KEY,		// /�L�[
	RSHIFT_KEY,		// Shift(�E)�L�[
	MULTIPLY_KEY,	// * (Numpad)�L�[
	LMENU_KEY,		// Alt (��)�L�[
	SPACE_KEY,		// Space�L�[
	CAPITAL_KEY,	// Caps Lock�L�[
	F1_KEY,			// F1�L�[
	F2_KEY,			// F2�L�[
	// 60
	F3_KEY,			// F3�L�[
	F4_KEY,			// F4�L�[
	F5_KEY,			// F5�L�[
	F6_KEY,		    // F6�L�[
	F7_KEY,		    // F7�L�[
	F8_KEY,		    // F8�L�[
	F9_KEY,		    // F9�L�[
	F10_KEY,	    // F10�L�[
	NUMLOCK_KEY,	// NumLock�L�[
	SCROLL_KEY,		// ScrollLock�L�[
	// 70
	NUMPAD7_KEY,	// 7(Numpad)�L�[
	NUMPAD8_KEY,	// 8(Numpad)�L�[
	NUMPAD9_KEY,	// 9(Numpad)�L�[
	SUBTRACT_KEY,	// -(Numpad)�L�[
	NUMPAD4_KEY,	// 4(Numpad)�L�[
	NUMPAD5_KEY,	// 5(Numpad)�L�[
	NUMPAD6_KEY,	// 6(Numpad)�L�[
	ADD_KEY,		// +(Numpad)�L�[
	NUMPAD1_KEY,	// 1(Numpad)�L�[
	NUMPAD2_KEY,	// 2(Numpad)�L�[
	// 80
	NUMPAD3_KEY,	// 3(Numpad)�L�[
	NUMPAD0_KEY,	// 0(Numpad)�L�[
	DECIMAL_KEY,	// .(Numpad)�L�[
	F11_KEY,		// F11�L�[
	F12_KEY,		// F12�L�[
	F13_KEY,		// F13�L�[
	F14_KEY,		// F14�L�[
	F15_KEY,		// F15�L�[
	KANA_KEY,		// �J�i�L�[
	CONVERT_KEY,	// �ϊ��L�[
	// 90
	NOCONVERT_KEY,	// ���ϊ��L�[
	YEN_KEY,		// \�L�[
	NUMPADEQUALS_KEY,	// =�L�[
	CIRCUMFLEX_KEY,	// ^�L�[
	AT_KEY,			// @�L�[
	COLON_KEY,		// :�L�[
	UNDERLINE_KEY,	// _�L�[
	KANJI_KEY,		// �����L�[
	STOP_KEY,		// Stop�L�[
	AX_KEY,			// (Japan AX)�L�[
	// 100
	UNLABELED_KEY,	// (J3100)�L�[
	NUMPADENTER_KEY,// Enter (Numpad)�L�[
	RCONTROL_KEY,	// Ctrl (�E)�L�[
	NUMPADCOMMA_KEY,// ,(Numpad)�L�[
	DIVIDE_KEY,		// /(Numpad)�L�[
	SYSRQ_KEY,		// Sys Rq�L�[
	RMENU_KEY,		// Alt (�E)�L�[
	PAUSE_KEY,		// Pause�L�[
	HOME_KEY,		// Home�L�[
	UP_KEY,			// ���L�[
	// 110
	PRIOR_KEY,		// 	Page Up�L�[
	LEFT_KEY,		// ���L�[
	RIGHT_KEY,		// �E�L�[
	END_KEY,		// End�L�[
	DOWN_KEY,		// ���L�[
	NEXT_KEY,		// 	Page Down�L�[
	INSERT_KEY,		// 	Insert�L�[
	DELETE_KEY,		// 	Delete�L�[
	LWIN_KEY,		// 	Windows�L�[
	RWIN_KEY,		// 	Windows�L
	APPS_KEY,		// 	Menu�L�[
	POWER_KEY,		// 	Power�L�[
	SLEEP_KEY,		// 	Windows�L�[

	MAX_KEY_INFO,	// �L�[��ԍő�
};
// �萔��DirectInput�ŗp�ӂ���Ă���DIK_�`���g�p����
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
// -------------------- �N���X�錾 -------------------
class DInput
{
private:
	// �L�[�̏��
	enum INPUT_STATE
	{
		NOT_PUSH,	// ������ĂȂ�
		PUSH_ENTER,	// �����ꂽ�u��
		PUSH,		// ������Ă�
		PUSH_EXIT,	// �����ꂽ�u��
	};

private:
	static DInput* m_pInstance;			// �V���O���g��
	LPDIRECTINPUT8				m_pDI;					// DirectInput�V�X�e��
	LPDIRECTINPUTDEVICE8		m_pDIKeyboard;			// DirectInputDevice�L�[�{�[�h�R�l�N�^
	LPDIRECTINPUTDEVICE8		m_pDIMouse;				// DirectInputDevice�}�E�X�R�l�N�^
	BYTE						m_KeyState[BUFFER_SIZE];// �L�[���͊i�[�p�z��
	DIMOUSESTATE2				m_MouseState;			// �}�E�X���
	INPUT_STATE g_InputState[KEY_INFO::MAX_KEY_INFO];	// �L�[��Ԕz��
	INPUT_STATE g_MouseState[MouseButton];				// �}�E�X��Ԕz��
public:
	DInput(HWND hWnd, HINSTANCE hInstance);
	virtual ~DInput();
	static void Create(HWND hWnd, HINSTANCE hInstance);	// ����
	static void Destroy();								// �j��
	void UpdateKeyboardState();							// �L�[�{�[�h���͏��̍X�V
	void UpdateMouseState();							// �}�E�X���͏��̍X�V
	BYTE* GetKeyboardState() { return m_KeyState; }// �L�[���͏��̎擾
	LPDIMOUSESTATE2 GetMouseState() { return &m_MouseState; }// �}�E�X���̎擾
	static DInput* GetInstance() { return m_pInstance; }
	
	// �L�[��Ԃ̍X�V
	void KeyStateUpdate();
	// �L�[�������ꂽ�u�Ԃ̔���
	bool GetKeyEnter(KEY_INFO key_info);
	// �L�[��������Ă邩�̔���
	bool GetKey(KEY_INFO key_info);
	// �L�[�������ꂽ�u�Ԃ��̔���
	bool GetKeyExit(KEY_INFO key_info);
	// �S�L�[�̓��ǂꂩ�����ꂽ�u�Ԃ̔���
	bool GetAllKeyEnter();
	// �S�L�[�̓��ǂꂩ������Ă��邩�̔���
	bool GetAllKey();

	// �}�E�X��Ԃ̍X�V
	void MouseStateUpdate();
	// �}�E�X�������ꂽ�u�Ԃ̔���
	bool GetMouseEnter(int mouse_info);
	// �}�E�X��������Ă邩�̔���
	bool GetMouse(int mouse_info);
	// �}�E�X�������ꂽ�u�Ԃ��̔���
	bool GetMouseExit(int mouse_info);
private:
	void InitDirectInput(HWND hWnd, HINSTANCE hInstance);// DirectInput�̏�����
	void ReleaseDirectInput();							// DirectInput�I�u�W�F�N�g���
};
#endif	/* __DINPUT_H__ */
