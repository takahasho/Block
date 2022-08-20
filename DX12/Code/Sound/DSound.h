#pragma once

#include "../Global.h"
#include <dsound.h>

#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dsound.lib")

class CDirectSound;

class CDSoundBuffer
{
public:
	CDSoundBuffer();
	~CDSoundBuffer();
	BOOL Create(CDirectSound&, WAVEFORMATEX*, DWORD);
	BOOL CreatePrimary(CDirectSound&, DWORD dwFreq = 22050);
	BOOL LoadWave(CDirectSound&, char*);
	HRESULT Release();
	HRESULT Restore();
	BOOL Play(BOOL loop = FALSE);
	BOOL Stop();
	void SetVol(LONG volume);
	BOOL IsAlive() { return buffer != 0; }
	BOOL IsSoundPlaying() { return bPlay; }
	LPDIRECTSOUNDBUFFER operator -> () { return buffer; }
	WAVEFORMATEX GetSetWaveFormat() { return Wfx; }
	void	SetBufferBytes(DWORD NewBufferBytes) { BufferBytes = NewBufferBytes; }
	DWORD	GetBufferBytes() { return BufferBytes; }
	void	SetPlay(BOOL bNewPlay) { bPlay = bNewPlay; }
	BOOL	GetPlay() { return bPlay; }
	void	SetNumPlay(int NewNumPlay) { NumPlay = NewNumPlay; }
	int		GetNumPlay() { return NumPlay; }
protected:
	LONG	Volume;				// 音量
	LONG	Pan;				// 左右のオーディオチャンネルの相対ボリューム
	DWORD	Frequency;			// 周波数
	DWORD	BufferBytes;		// サウンドバッファサイズ
	WAVEFORMATEX Wfx;			// フォーマット
	LPDIRECTSOUNDBUFFER	buffer;	// サウンドバッファ
	BOOL	bPlay;				// 演奏中の有無
	int		NumPlay;			// 演奏番号
};

inline BOOL CDSoundBuffer::Play(BOOL loop)
{
	if (FAILED(buffer->Play(0, 0, loop ? DSBPLAY_LOOPING : 0)))
		return FALSE;
	return TRUE;
}

inline BOOL CDSoundBuffer::Stop()
{
	return buffer->Stop() == DS_OK;
}

inline void CDSoundBuffer::SetVol(LONG volume)
{	
	Volume = volume;
	buffer->SetVolume(volume);
}

class CDirectSound {
public:
	CDirectSound();
	~CDirectSound();
	BOOL Create(HWND, int freq = 22050);
	void ReleaseObjects();
	LPDIRECTSOUND operator -> () { return m_pDS; }
	CDSoundBuffer GetPrimary() { return Primary; }
protected:
	LPDIRECTSOUND	m_pDS;		// ダイレクトサウンド
	CDSoundBuffer	Primary;	// プライマリバッファ
};

class CDSBufferNotify {
public:
	CDSBufferNotify();
	~CDSBufferNotify();
	BOOL SetNotify(CDSoundBuffer&, HANDLE);
protected:
	LPDIRECTSOUNDNOTIFY pDSNotify;
};
