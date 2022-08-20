// -------------------- IncludeFile -----------------
#include "DInput.h"
DInput* DInput::m_pInstance = nullptr;			// �C���X�^���X�̎���

// �R���X�g���N�^
DInput::DInput(HWND hWnd, HINSTANCE hInstance)
{
	m_pDI = nullptr;
	m_pDIKeyboard = nullptr;
	m_pDIMouse = nullptr;
	InitDirectInput(hWnd, hInstance);
}
// �f�X�g���N�^
DInput::~DInput()
{
	ReleaseDirectInput();
}
// ------------------------------------------------------------------------
//	�֐�	:InitDirectInput			DirectInput�̏�����
//	����	:hWnd						�E�B���h�E�n���h��
//          :hInstance					�C���X�^���X�n���h��
//	�߂�l	:							�Ȃ�
// ------------------------------------------------------------------------
void DInput::InitDirectInput(HWND hWnd, HINSTANCE hInstance)
{
	// DirectInput�V�X�e���쐬
	::DirectInput8Create(
		hInstance,				// �v���O�����̃C���X�^���g�n���h�����w��
		DIRECTINPUT_VERSION,	// �o�[�W���������w��(�ʏ�w��)
		IID_IDirectInput8A,		// �ړI�̃C���^�[�t�F�C�X�̈�ӂȎ��ʎq(�ʏ�w��)
		(void**)&m_pDI,		// DirectInput�V�X�e���̃A�h���X���w��
		NULL);					// COM �W���̂ɑ΂��鐧��I�u�W�F�N�g��IUnknown�C���^�[�t�F�C�X�̃A�h���X�ւ̃|�C���^(�ʏ�w��)

	// DirectInputDevice�R�l�N�^�쐬
	// �L�[�{�[�h�I�u�W�F�N�g�쐬
	m_pDI->CreateDevice(
		GUID_SysKeyboard,	// ���͋@���GUID���w��(�L�[�{�[�h)
		&m_pDIKeyboard,		// DirectInput�R�l�N�^�̃A�h���X���w��
		NULL);				// COM �W���̂ɑ΂��鐧��I�u�W�F�N�g��IUnknown�C���^�[�t�F�C�X�̃A�h���X
	// �}�E�X�I�u�W�F�N�g�쐬
	m_pDI->CreateDevice(
		GUID_SysMouse,		// ���͋@���GUID���w��(�}�E�X)
		&m_pDIMouse,		// DirectInput�R�l�N�^�̃A�h���X���w��
		NULL);				// COM �W���̂ɑ΂��鐧��I�u�W�F�N�g��IUnknown�C���^�[�t�F�C�X�̃A�h���X

	// ���̓f�[�^�`���w��
	m_pDIKeyboard->SetDataFormat(&c_dfDIKeyboard);	// �L�[�{�[�h���̓f�[�^�`���w��
	m_pDIMouse->SetDataFormat(&c_dfDIMouse2);		// �}�E�X���̓f�[�^�`���w��

	// �������x���ݒ�
	// �L�[�{�[�h�������x���ݒ�
	m_pDIKeyboard->SetCooperativeLevel(
		hWnd,										// �E�C���h�E�n���h��
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);		// �������x���ݒ�(�t�H�A�O���E���h���̂ݓ��͏��擾/��r���A�N�Z�X����v��)
	// �}�E�X�������x���ݒ�
	m_pDIMouse->SetCooperativeLevel(
		hWnd,										// �E�B���h�E�n���h��
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);		// �������x���ݒ�(�t�H�A�O���E���h���̂ݓ��͏��擾/��r���A�N�Z�X����v��)

	DIPROPDWORD diprdw = {};
	// DIPROPDWORD�^�\���̂̐ݒ�
	diprdw.diph.dwSize = sizeof(diprdw);			// ��܂���\���̂̃T�C�Y���w��
	diprdw.diph.dwHeaderSize = sizeof(diprdw);		// DIPROPHEADER�\���̂̃T�C�Y���w��
	diprdw.diph.dwObj = 0;							// �A�N�Z�X����v���p�e�B��������I�u�W�F�N�g��ݒ�(dwHow�Ɉˑ�����)
	diprdw.diph.dwHow = DIPH_DEVICE;				// �f�o�C�X�S�̂���v���p�e�B���擾������ݒ肷��
	diprdw.dwData = DIPROPAXISMODE_REL;

	// �o�b�t�@�T�C�Y��ݒ�
	m_pDIMouse->SetProperty(
		DIPROP_AXISMODE,							// ������ݒ�
		&diprdw.diph);								// DIPROPHEADER�\���̂̃A�h���X���w��

	// ���͋@��̎擾
	m_pDIKeyboard->Acquire();						// �L�[�{�[�h������͎擾���J�n
	m_pDIMouse->Acquire();							// �}�E�X������͎擾���J�n
}
// ------------------------------------------------------------------------
//	�֐�	:UpdateKeyboardState			�L�[�{�[�h���͏��̍X�V
//	����	:								�Ȃ�
//	�߂�l	:								�Ȃ�
// ------------------------------------------------------------------------
void DInput::UpdateKeyboardState()
{
	HRESULT hr;												// ���\�b�h���s���l���
	ZeroMemory(m_KeyState, sizeof(BYTE) * BUFFER_SIZE);		// �z����[���ŏ�����

	if (!m_pDIKeyboard)
		return;

	// ���ړ��̓f�[�^�擾����
	hr = m_pDIKeyboard->GetDeviceState(
		BUFFER_SIZE,										// �i�[�z��v�f�����w��
		m_KeyState);										// �i�[�z��̃A�h���X���w��

	if ((hr == DIERR_NOTACQUIRED) || (hr == DIERR_INPUTLOST))	// ���͎擾�̎��s�����ꍇ
		m_pDIKeyboard->Acquire();								// ���͎擾�ĊJ
}
// ------------------------------------------------------------------------
//	�֐�	:UpdateMouseState				�}�E�X���͏��̎擾
//	����	:								�Ȃ�
//	�߂�l	:								�Ȃ�
// ------------------------------------------------------------------------
void DInput::UpdateMouseState()
{
	HRESULT hr;												// ���\�b�h���s���l���
	::ZeroMemory(&m_MouseState, sizeof(m_MouseState));		// �z����[���ŏ�����

	if (!m_pDIMouse)
		return;

	// �o�b�t�@�����O���̓f�[�^�擾
	hr = m_pDIMouse->GetDeviceState(
		sizeof(DIMOUSESTATE2),		// ���̓f�[�^���i�[����\���̂̃T�C�Y���w��
		&m_MouseState);				// ���̓f�[�^���i�[����\���̂̃A�h���X���w��

	if (hr != DI_OK)
	{
		// �}�E�X������͎擾���J�n
		hr = m_pDIMouse->Acquire();
		while (hr == DIERR_INPUTLOST)
			hr = m_pDIMouse->Acquire();
		return;
	}
	if (FAILED(hr))
		return;
}
// ------------------------------------------------------------------------
//	�֐�	:ReleaseDirectInput				DirectInput�I�u�W�F�N�g���
//	����	:								�Ȃ�
//	�߂�l	:								�Ȃ�
// ------------------------------------------------------------------------
void DInput::ReleaseDirectInput()
{
	// DirectInput�֘A�I�u�W�F�N�g���
	if (m_pDI != nullptr)							// DirectInput�V�X�e�������݂���ꍇ
	{
		if (m_pDIKeyboard != nullptr)				// DirectInputDevice�L�[�{�[�h�R�l�N�V���������݂���ꍇ
		{
			m_pDIKeyboard->Unacquire();				// �L�[�{�[�h�擾��~
			m_pDIKeyboard->Release();				// DirectInputDevice�L�[�{�[�h�R�l�N�V�����̉��
		}
		if (m_pDIMouse != nullptr)					// DirectInputDevice�}�E�X�R�l�N�V���������݂���ꍇ
		{
			m_pDIMouse->Unacquire();				// �}�E�X�擾��~
			m_pDIMouse->Release();					// DirectInputDevice�}�E�X�R�l�N�V�����̉��
		}
		m_pDI->Release();							// DirectInput�V�X�e���̉��
	}
}
// �쐬
void DInput::Create(HWND hWnd, HINSTANCE hInstance)
{
	if (!m_pInstance)
		m_pInstance = new DInput(hWnd, hInstance);
}
// �j��
void DInput::Destroy()
{
	delete m_pInstance;
}

void DInput::KeyStateUpdate()
{
	LPBYTE pKeyboard = GetKeyboardState();

	// �g���L�[�̎�ޕ���
	for (int i = 0; i < MAX_KEY_INFO; i++)
	{
		// �L�[��������Ă��邩�̔���
		if (pKeyboard[g_KeyInfo[i]] & 0x80)
		{
			// �O�̃t���[���ŉ�����ĂȂ�
			if (g_InputState[i] == INPUT_STATE::NOT_PUSH ||
				g_InputState[i] == INPUT_STATE::PUSH_EXIT)
			{
				// ��Ԃ�������Ă��u�ԂɕύX
				g_InputState[i] = INPUT_STATE::PUSH_ENTER;
			}
			else
			{
				// ��Ԃ�������Ă�ɕύX
				g_InputState[i] = INPUT_STATE::PUSH;
			}
		}
		else
		{
			// ��Ԃ�������Ă邩�𔻒�
			if (g_InputState[i] == INPUT_STATE::PUSH ||
				g_InputState[i] == INPUT_STATE::PUSH_ENTER)
			{
				// ��Ԃ𗣂����u�ԂɕύX
				g_InputState[i] = INPUT_STATE::PUSH_EXIT;
			}
			else
			{
				// ��Ԃ������ē��ɕύX
				g_InputState[i] = INPUT_STATE::NOT_PUSH;
			}
		}
	}
}

bool DInput::GetKeyEnter(KEY_INFO key_info)
{
	// �w�肳�ꂽ�L�[�̏�Ԃ������ꂽ�u�ԂȂ�true
	return (g_InputState[key_info] == INPUT_STATE::PUSH_ENTER);
}

bool DInput::GetKey(KEY_INFO key_info)
{
	// �w�肳�ꂽ�L�[�̏�Ԃ�������Ă�Ȃ�true
	return (g_InputState[key_info] == INPUT_STATE::PUSH);
}

bool DInput::GetKeyExit(KEY_INFO key_info)
{
	// �w�肳�ꂽ�L�[�̏�Ԃ��������u�ԂȂ�true
	return (g_InputState[key_info] == INPUT_STATE::PUSH_EXIT);
}

bool DInput::GetAllKeyEnter()
{
	// �S�L�[�̓��ǂꂩ������Ă��邩�̔���
	for (int i = 0; i < MAX_KEY_INFO; i++)
	{
		if (g_InputState[i] == INPUT_STATE::PUSH_ENTER)
			return true;
	}
	return false;
}

bool DInput::GetAllKey()
{
	// �S�L�[�̓��ǂꂩ������Ă��邩�̔���
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

	// �g���L�[�̎�ޕ���
	for (int i = 0; i < MouseButton; i++)
	{
		// �L�[��������Ă��邩�̔���
		if (pMouseState->rgbButtons[i] & 0x80)
		{
			// �O�̃t���[���ŉ�����ĂȂ�
			if (g_MouseState[i] == INPUT_STATE::NOT_PUSH ||
				g_MouseState[i] == INPUT_STATE::PUSH_EXIT)
			{
				// ��Ԃ�������Ă��u�ԂɕύX
				g_MouseState[i] = INPUT_STATE::PUSH_ENTER;
			}
			else
			{
				// ��Ԃ�������Ă�ɕύX
				g_MouseState[i] = INPUT_STATE::PUSH;
			}
		}
		else
		{
			// ��Ԃ�������Ă邩�𔻒�
			if (g_MouseState[i] == INPUT_STATE::PUSH ||
				g_MouseState[i] == INPUT_STATE::PUSH_ENTER)
			{
				// ��Ԃ𗣂����u�ԂɕύX
				g_MouseState[i] = INPUT_STATE::PUSH_EXIT;
			}
			else
			{
				// ��Ԃ������ē��ɕύX
				g_MouseState[i] = INPUT_STATE::NOT_PUSH;
			}
		}
	}
}

bool DInput::GetMouseEnter(int mouse_info)
{
	// �w�肳�ꂽ�L�[�̏�Ԃ������ꂽ�u�ԂȂ�true
	return (g_MouseState[mouse_info] == INPUT_STATE::PUSH_ENTER);
}

bool DInput::GetMouse(int mouse_info)
{
	// �w�肳�ꂽ�L�[�̏�Ԃ�������Ă�Ȃ�true
	return (g_MouseState[mouse_info] == INPUT_STATE::PUSH);
}

bool DInput::GetMouseExit(int mouse_info)
{
	// �w�肳�ꂽ�L�[�̏�Ԃ��������u�ԂȂ�true
	return (g_MouseState[mouse_info] == INPUT_STATE::PUSH_EXIT);
}