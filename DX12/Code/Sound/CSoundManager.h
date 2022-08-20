#pragma once
// -------------------- IncludeFile -----------------
#include "../Global.h"
#include "DSound.h"
// -------------------- Define宣言 ------------------
#define NUMEFFECTSOUND	16		// サウンド数
#define NUMSECONDARY	32		// セカンダリバッファ数(MAX:63、MAXIMUM_WAIT_OBJECTS = 64)
// -------------------- クラス宣言 -------------------
class CSoundManager
{
public:
	CSoundManager(HWND);	// コンストラクタ
	~CSoundManager();		// デストラクタ	
	BOOL LoadWave(DWORD, char*);			// WaveFileの読み込み
	BOOL ChangeWave(DWORD, DWORD);
	void Play(DWORD SetPosition, BOOL loop);
	void Stop(DWORD SetPosition, bool Reset);
	void StopAllSound();
	void VolumeAllSound(int volume);	// 100～0
	CDirectSound* GetDirectSound() { return &DirectSound; }
protected:
	CDirectSound DirectSound;			// DirectSoundオブジェクト
	CDSoundBuffer* pDSBuffer;			// サウンドバッファ
	CDSoundBuffer* pDSSecondaryBuffer;	// サウンドセカンダリバッファ
	CDSBufferNotify Notify;				// 演奏終了通知用
	HANDLE hEvent[NUMSECONDARY + 1];	// イベントハンドル
	HANDLE hThread;						// イベント管理スレッドハンドル
	static DWORD WINAPI EventNotifyProc(LPVOID param);// イベント管理スレッド
};