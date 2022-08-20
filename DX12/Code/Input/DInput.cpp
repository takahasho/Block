// -------------------- IncludeFile -----------------
#include "DInput.h"
DInput* DInput::m_pInstance = nullptr;			// インスタンスの実態

// コンストラクタ
DInput::DInput(HWND hWnd, HINSTANCE hInstance)
{
	m_pDI = nullptr;
	m_pDIKeyboard = nullptr;
	m_pDIMouse = nullptr;
	InitDirectInput(hWnd, hInstance);
}
// デストラクタ
DInput::~DInput()
{
	ReleaseDirectInput();
}
// ------------------------------------------------------------------------
//	関数	:InitDirectInput			DirectInputの初期化
//	引数	:hWnd						ウィンドウハンドル
//          :hInstance					インスタンスハンドル
//	戻り値	:							なし
// ------------------------------------------------------------------------
void DInput::InitDirectInput(HWND hWnd, HINSTANCE hInstance)
{
	// DirectInputシステム作成
	::DirectInput8Create(
		hInstance,				// プログラムのインスタントハンドルを指定
		DIRECTINPUT_VERSION,	// バージョン情報を指定(通常指定)
		IID_IDirectInput8A,		// 目的のインターフェイスの一意な識別子(通常指定)
		(void**)&m_pDI,		// DirectInputシステムのアドレスを指定
		NULL);					// COM 集成体に対する制御オブジェクトのIUnknownインターフェイスのアドレスへのポインタ(通常指定)

	// DirectInputDeviceコネクタ作成
	// キーボードオブジェクト作成
	m_pDI->CreateDevice(
		GUID_SysKeyboard,	// 入力機器のGUIDを指定(キーボード)
		&m_pDIKeyboard,		// DirectInputコネクタのアドレスを指定
		NULL);				// COM 集成体に対する制御オブジェクトのIUnknownインターフェイスのアドレス
	// マウスオブジェクト作成
	m_pDI->CreateDevice(
		GUID_SysMouse,		// 入力機器のGUIDを指定(マウス)
		&m_pDIMouse,		// DirectInputコネクタのアドレスを指定
		NULL);				// COM 集成体に対する制御オブジェクトのIUnknownインターフェイスのアドレス

	// 入力データ形式指定
	m_pDIKeyboard->SetDataFormat(&c_dfDIKeyboard);	// キーボード入力データ形式指定
	m_pDIMouse->SetDataFormat(&c_dfDIMouse2);		// マウス入力データ形式指定

	// 協調レベル設定
	// キーボード協調レベル設定
	m_pDIKeyboard->SetCooperativeLevel(
		hWnd,										// ウインドウハンドル
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);		// 協調レベル設定(フォアグラウンド時のみ入力情報取得/非排他アクセス権を要求)
	// マウス協調レベル設定
	m_pDIMouse->SetCooperativeLevel(
		hWnd,										// ウィンドウハンドル
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);		// 協調レベル設定(フォアグラウンド時のみ入力情報取得/非排他アクセス権を要求)

	DIPROPDWORD diprdw = {};
	// DIPROPDWORD型構造体の設定
	diprdw.diph.dwSize = sizeof(diprdw);			// 包含する構造体のサイズを指定
	diprdw.diph.dwHeaderSize = sizeof(diprdw);		// DIPROPHEADER構造体のサイズを指定
	diprdw.diph.dwObj = 0;							// アクセスするプロパティが属するオブジェクトを設定(dwHowに依存する)
	diprdw.diph.dwHow = DIPH_DEVICE;				// デバイス全体からプロパティを取得したり設定する
	diprdw.dwData = DIPROPAXISMODE_REL;

	// バッファサイズを設定
	m_pDIMouse->SetProperty(
		DIPROP_AXISMODE,							// 属性を設定
		&diprdw.diph);								// DIPROPHEADER構造体のアドレスを指定

	// 入力機器の取得
	m_pDIKeyboard->Acquire();						// キーボードから入力取得を開始
	m_pDIMouse->Acquire();							// マウスから入力取得を開始
}
// ------------------------------------------------------------------------
//	関数	:UpdateKeyboardState			キーボード入力情報の更新
//	引数	:								なし
//	戻り値	:								なし
// ------------------------------------------------------------------------
void DInput::UpdateKeyboardState()
{
	HRESULT hr;												// メソッド失敗時値代入
	ZeroMemory(m_KeyState, sizeof(BYTE) * BUFFER_SIZE);		// 配列をゼロで初期化

	if (!m_pDIKeyboard)
		return;

	// 直接入力データ取得処理
	hr = m_pDIKeyboard->GetDeviceState(
		BUFFER_SIZE,										// 格納配列要素数を指定
		m_KeyState);										// 格納配列のアドレスを指定

	if ((hr == DIERR_NOTACQUIRED) || (hr == DIERR_INPUTLOST))	// 入力取得の失敗した場合
		m_pDIKeyboard->Acquire();								// 入力取得再開
}
// ------------------------------------------------------------------------
//	関数	:UpdateMouseState				マウス入力情報の取得
//	引数	:								なし
//	戻り値	:								なし
// ------------------------------------------------------------------------
void DInput::UpdateMouseState()
{
	HRESULT hr;												// メソッド失敗時値代入
	::ZeroMemory(&m_MouseState, sizeof(m_MouseState));		// 配列をゼロで初期化

	if (!m_pDIMouse)
		return;

	// バッファリング入力データ取得
	hr = m_pDIMouse->GetDeviceState(
		sizeof(DIMOUSESTATE2),		// 入力データを格納する構造体のサイズを指定
		&m_MouseState);				// 入力データを格納する構造体のアドレスを指定

	if (hr != DI_OK)
	{
		// マウスから入力取得を開始
		hr = m_pDIMouse->Acquire();
		while (hr == DIERR_INPUTLOST)
			hr = m_pDIMouse->Acquire();
		return;
	}
	if (FAILED(hr))
		return;
}
// ------------------------------------------------------------------------
//	関数	:ReleaseDirectInput				DirectInputオブジェクト解放
//	引数	:								なし
//	戻り値	:								なし
// ------------------------------------------------------------------------
void DInput::ReleaseDirectInput()
{
	// DirectInput関連オブジェクト解放
	if (m_pDI != nullptr)							// DirectInputシステムが存在する場合
	{
		if (m_pDIKeyboard != nullptr)				// DirectInputDeviceキーボードコネクションが存在する場合
		{
			m_pDIKeyboard->Unacquire();				// キーボード取得停止
			m_pDIKeyboard->Release();				// DirectInputDeviceキーボードコネクションの解放
		}
		if (m_pDIMouse != nullptr)					// DirectInputDeviceマウスコネクションが存在する場合
		{
			m_pDIMouse->Unacquire();				// マウス取得停止
			m_pDIMouse->Release();					// DirectInputDeviceマウスコネクションの解放
		}
		m_pDI->Release();							// DirectInputシステムの解放
	}
}
// 作成
void DInput::Create(HWND hWnd, HINSTANCE hInstance)
{
	if (!m_pInstance)
		m_pInstance = new DInput(hWnd, hInstance);
}
// 破棄
void DInput::Destroy()
{
	delete m_pInstance;
}

void DInput::KeyStateUpdate()
{
	LPBYTE pKeyboard = GetKeyboardState();

	// 使うキーの種類分回す
	for (int i = 0; i < MAX_KEY_INFO; i++)
	{
		// キーが押されているかの判定
		if (pKeyboard[g_KeyInfo[i]] & 0x80)
		{
			// 前のフレームで押されてない
			if (g_InputState[i] == INPUT_STATE::NOT_PUSH ||
				g_InputState[i] == INPUT_STATE::PUSH_EXIT)
			{
				// 状態を押されてた瞬間に変更
				g_InputState[i] = INPUT_STATE::PUSH_ENTER;
			}
			else
			{
				// 状態を押されてるに変更
				g_InputState[i] = INPUT_STATE::PUSH;
			}
		}
		else
		{
			// 状態が押されてるかを判定
			if (g_InputState[i] == INPUT_STATE::PUSH ||
				g_InputState[i] == INPUT_STATE::PUSH_ENTER)
			{
				// 状態を離した瞬間に変更
				g_InputState[i] = INPUT_STATE::PUSH_EXIT;
			}
			else
			{
				// 状態を押して内に変更
				g_InputState[i] = INPUT_STATE::NOT_PUSH;
			}
		}
	}
}

bool DInput::GetKeyEnter(KEY_INFO key_info)
{
	// 指定されたキーの状態が押された瞬間ならtrue
	return (g_InputState[key_info] == INPUT_STATE::PUSH_ENTER);
}

bool DInput::GetKey(KEY_INFO key_info)
{
	// 指定されたキーの状態が押されてるならtrue
	return (g_InputState[key_info] == INPUT_STATE::PUSH);
}

bool DInput::GetKeyExit(KEY_INFO key_info)
{
	// 指定されたキーの状態が離した瞬間ならtrue
	return (g_InputState[key_info] == INPUT_STATE::PUSH_EXIT);
}

bool DInput::GetAllKeyEnter()
{
	// 全キーの内どれか押されているかの判定
	for (int i = 0; i < MAX_KEY_INFO; i++)
	{
		if (g_InputState[i] == INPUT_STATE::PUSH_ENTER)
			return true;
	}
	return false;
}

bool DInput::GetAllKey()
{
	// 全キーの内どれか押されているかの判定
	for (int i = 0; i < MAX_KEY_INFO; i++)
	{
		if (g_InputState[i] == INPUT_STATE::PUSH)
			return true;
	}
	return false;
}

void DInput::MouseStateUpdate()
{
	LPDIMOUSESTATE2 pMouseState = GetMouseState();

	// 使うキーの種類分回す
	for (int i = 0; i < MouseButton; i++)
	{
		// キーが押されているかの判定
		if (pMouseState->rgbButtons[i] & 0x80)
		{
			// 前のフレームで押されてない
			if (g_MouseState[i] == INPUT_STATE::NOT_PUSH ||
				g_MouseState[i] == INPUT_STATE::PUSH_EXIT)
			{
				// 状態を押されてた瞬間に変更
				g_MouseState[i] = INPUT_STATE::PUSH_ENTER;
			}
			else
			{
				// 状態を押されてるに変更
				g_MouseState[i] = INPUT_STATE::PUSH;
			}
		}
		else
		{
			// 状態が押されてるかを判定
			if (g_MouseState[i] == INPUT_STATE::PUSH ||
				g_MouseState[i] == INPUT_STATE::PUSH_ENTER)
			{
				// 状態を離した瞬間に変更
				g_MouseState[i] = INPUT_STATE::PUSH_EXIT;
			}
			else
			{
				// 状態を押して内に変更
				g_MouseState[i] = INPUT_STATE::NOT_PUSH;
			}
		}
	}
}

bool DInput::GetMouseEnter(int mouse_info)
{
	// 指定されたキーの状態が押された瞬間ならtrue
	return (g_MouseState[mouse_info] == INPUT_STATE::PUSH_ENTER);
}

bool DInput::GetMouse(int mouse_info)
{
	// 指定されたキーの状態が押されてるならtrue
	return (g_MouseState[mouse_info] == INPUT_STATE::PUSH);
}

bool DInput::GetMouseExit(int mouse_info)
{
	// 指定されたキーの状態が離した瞬間ならtrue
	return (g_MouseState[mouse_info] == INPUT_STATE::PUSH_EXIT);
}