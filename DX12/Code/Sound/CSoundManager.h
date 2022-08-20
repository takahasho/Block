#pragma once
// -------------------- IncludeFile -----------------
#include "../Global.h"
#include "DSound.h"
// -------------------- Define�錾 ------------------
#define NUMEFFECTSOUND	16		// �T�E���h��
#define NUMSECONDARY	32		// �Z�J���_���o�b�t�@��(MAX:63�AMAXIMUM_WAIT_OBJECTS = 64)
// -------------------- �N���X�錾 -------------------
class CSoundManager
{
public:
	CSoundManager(HWND);	// �R���X�g���N�^
	~CSoundManager();		// �f�X�g���N�^	
	BOOL LoadWave(DWORD, char*);			// WaveFile�̓ǂݍ���
	BOOL ChangeWave(DWORD, DWORD);
	void Play(DWORD SetPosition, BOOL loop);
	void Stop(DWORD SetPosition, bool Reset);
	void StopAllSound();
	void VolumeAllSound(int volume);	// 100�`0
	CDirectSound* GetDirectSound() { return &DirectSound; }
protected:
	CDirectSound DirectSound;			// DirectSound�I�u�W�F�N�g
	CDSoundBuffer* pDSBuffer;			// �T�E���h�o�b�t�@
	CDSoundBuffer* pDSSecondaryBuffer;	// �T�E���h�Z�J���_���o�b�t�@
	CDSBufferNotify Notify;				// ���t�I���ʒm�p
	HANDLE hEvent[NUMSECONDARY + 1];	// �C�x���g�n���h��
	HANDLE hThread;						// �C�x���g�Ǘ��X���b�h�n���h��
	static DWORD WINAPI EventNotifyProc(LPVOID param);// �C�x���g�Ǘ��X���b�h
};