#pragma once
// -------------------- IncludeFile -----------------
#include "../Global.h"
// -------------------- �N���X�錾 -------------------
class CWaveFile
{
public:
	CWaveFile();
	~CWaveFile();
	BOOL Open(char*);
	BOOL StartRead();
	BOOL Read(LPVOID, DWORD, DWORD*);
	BOOL Close();
	WAVEFORMATEX* GetFormat() { return m_pwfx; }
	DWORD CkSize() { return m_ck.cksize; }
private:
	WAVEFORMATEX* m_pwfx;
	HMMIO			m_hmmio;
	MMCKINFO		m_ckRiff;
	MMCKINFO		m_ck;
};