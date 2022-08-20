#include "DSound.h"
#include "CWaveFile.h"
// コンストラクタ
CDirectSound::CDirectSound() : m_pDS(NULL)
{

}
// デストラクタ
CDirectSound::~CDirectSound()
{
	ReleaseObjects();
}
// ------------------------------------------------------------------------
//　関数名	:Create						ダイレクトサウンド作成
//	引数	:hWnd						ウィンドウハンドル
//			:freq						周波数
//	戻り値	:							TRUE:成功、FALSE:失敗
// ------------------------------------------------------------------------
BOOL CDirectSound::Create(HWND hWnd, int freq)
{
	HRESULT hr;
	// DirectSoundインタフェースの取得
	if (FAILED(hr = DirectSoundCreate(NULL, &m_pDS, NULL)))
		return FALSE;
	// 強調レベルを設定
	if (FAILED(hr = m_pDS->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)))
		return FALSE;
	// プライマリーバッファの作成
	return Primary.CreatePrimary(*this, freq);

	return TRUE;
}
// ------------------------------------------------------------------------
//　関数名	:ReleaseObjects				開放処理
//	引数	:							なし
//	戻り値	:							なし
// ------------------------------------------------------------------------
void CDirectSound::ReleaseObjects()
{
	if (m_pDS) {
		Primary.Release();
		m_pDS->Release();
		m_pDS = NULL;
	}
}
// コンストラクタ
CDSoundBuffer::CDSoundBuffer() :
	Volume(0), Pan(0), Frequency(0), BufferBytes(0), bPlay(TRUE), NumPlay(-1)
{
	::ZeroMemory(&Wfx, sizeof(Wfx));
	::ZeroMemory(&buffer, sizeof(buffer));
}
// デストラクタ
CDSoundBuffer::~CDSoundBuffer()
{
	if (buffer)
		buffer->Release();
}
// ------------------------------------------------------------------------
//　関数名	:Create						サウンドバッファ作成
//	引数	:ds							ダイレクトサウンドのアドレス
//			:pFormat					ウェーブフォーマット
//			:dwBufferBytes				バッファサイズ
//	戻り値	:							TRUE:成功、FALSE:失敗
// ------------------------------------------------------------------------
BOOL CDSoundBuffer::Create(CDirectSound& ds, WAVEFORMATEX* pFormat, DWORD dwBufferBytes)
{
	HRESULT	hr;
	DSBUFFERDESC dsbd;

	// DSBUFFERDESC構造体の設定
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS
		| DSBCAPS_LOCDEFER | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY;
	dsbd.dwBufferBytes = dwBufferBytes;
	dsbd.lpwfxFormat = pFormat;
	// 情報のバックアップ
	BufferBytes = dwBufferBytes;
	memcpy(&Wfx, pFormat, sizeof(WAVEFORMATEX));

	// サウンドバッファの作成
	if (FAILED(hr = ds->CreateSoundBuffer(&dsbd, &buffer, NULL)))
		return FALSE;

	// 情報の取得
	buffer->GetVolume(&Volume);
	buffer->GetPan(&Pan);
	buffer->GetFrequency(&Frequency);

	return TRUE;
}
// ------------------------------------------------------------------------
//　関数名	:CreatePrimary				プライマリバッファ作成
//	引数	:ds							ダイレクトサウンドのアドレス
//			:dwPrimaryFreq				周波数
//	戻り値	:							TRUE:成功、FALSE:失敗
// ------------------------------------------------------------------------
BOOL CDSoundBuffer::CreatePrimary(CDirectSound& ds, DWORD dwPrimaryFreq)
{
	HRESULT hr;

	// DSBUFFERDESC構造体の設定
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes = 0;
	dsbd.lpwfxFormat = NULL;

	// サウンドバッファの作成
	if (FAILED(hr = ds->CreateSoundBuffer(&dsbd, &buffer, NULL)))
		return FALSE;

	// WAVEフォーマットの設定
	WAVEFORMATEX wfx;
	ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 2;
	wfx.nSamplesPerSec = dwPrimaryFreq;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = 4;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	// サウンドバッファフォーマットの設定
	if (FAILED(hr = buffer->SetFormat(&wfx)))
		return FALSE;

	buffer->GetVolume(&Volume);
	buffer->GetPan(&Pan);
	buffer->GetFrequency(&Frequency);

	return TRUE;
}
// ------------------------------------------------------------------------
//　関数名	:Release					開放処理
//	引数	:							なし
//	戻り値	:							S_OK:成功
// ------------------------------------------------------------------------
HRESULT CDSoundBuffer::Release()
{
	if (buffer)
		HRESULT hr = buffer->Release();
	buffer = NULL;

	return S_OK;
}
// ------------------------------------------------------------------------
//　関数名	:Restore					リストア処理
//	引数	:							なし
//	戻り値	:							S_OK:成功
// ------------------------------------------------------------------------
HRESULT CDSoundBuffer::Restore()
{
	if (buffer)
		HRESULT hr = buffer->Restore();

	return S_OK;
}
// ------------------------------------------------------------------------
//　関数名	:LoadWave					ファイルの読み込み
//	引数	:ds							ダイレクトサウンドのアドレス
//			:pFileName					ファイル名
//	戻り値	:							TRUE:成功、FALSE:失敗
// ------------------------------------------------------------------------
BOOL CDSoundBuffer::LoadWave(CDirectSound& ds, char* pFileName)
{
	CWaveFile file;
	if (!file.Open(pFileName) || !file.StartRead())
		return FALSE;

	Release();	// バッファがあればReleaseする

	// 新しくWAVEのサイズでバッファを作成する
	if (!Create(ds, file.GetFormat(), file.CkSize()))
		return FALSE;

	// バッファにWaveデータに書き込む
	LPVOID pFirstDest, pSecondDest;
	DWORD dwFirstDestBufferBytes, dwSecondDestBufferBytes;

	HRESULT hr = buffer->Lock(
		0,							// オフセット値をバイト単位
		file.CkSize(),				// ロックされるバッファ部分のサイズをバイト単位
		&pFirstDest,				// 最初にロックされた部分へのポインタ
		&dwFirstDestBufferBytes,	// ブロックのバイト数
		&pSecondDest,				// 溢れたブロックのロックされた部分へのポインタ
		&dwSecondDestBufferBytes,	// 溢れたブロックのバイト数
		0L);
	if (hr == DSERR_BUFFERLOST) {
		Restore();
		hr = buffer->Lock(0,			// オフセット値をバイト単位
			file.CkSize(),				// ロックされるバッファ部分のサイズをバイト単位
			&pFirstDest,				// 最初にロックされた部分へのポインタ
			&dwFirstDestBufferBytes,	// ブロックのバイト数
			&pSecondDest,				// 溢れたブロックのロックされた部分へのポインタ
			&dwSecondDestBufferBytes,	// 溢れたブロックのバイト数
			0L);
	}
	if (SUCCEEDED(hr)) {
		BOOL ReadError = FALSE;
		DWORD dwSizeRead;
		if (!file.Read((LPBYTE)pFirstDest, dwFirstDestBufferBytes, &dwSizeRead)
			|| dwFirstDestBufferBytes != dwSizeRead)
			ReadError = TRUE;
		if (dwSecondDestBufferBytes) {
			if (!file.Read((LPBYTE)pSecondDest, dwSecondDestBufferBytes, &dwSizeRead)
				|| dwSecondDestBufferBytes != dwSizeRead)
				ReadError = TRUE;
		}
		hr = buffer->Unlock(pFirstDest, dwFirstDestBufferBytes, pSecondDest, dwSecondDestBufferBytes);
		if (ReadError)
			return FALSE;
		return TRUE;
	}

	return FALSE;
}
// コンストラクタ
CDSBufferNotify::CDSBufferNotify()
{
	pDSNotify = NULL;
}
// デストラクタ
CDSBufferNotify::~CDSBufferNotify()
{
}
// ------------------------------------------------------------------------
//　関数名	:SetNotify					演奏終了通知イベントセット
//	引数	:buffer						サウンドバッファ
//			:hEvent						演奏終了時に発生するイベントハンドル
//	戻り値	:							TRUE:成功、FALSE:失敗
// ------------------------------------------------------------------------
BOOL CDSBufferNotify::SetNotify(CDSoundBuffer& buffer, HANDLE hEvent)
{
	// インターフェイスを指すポインタの取得
	HRESULT hr = buffer->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&pDSNotify);
	if (FAILED(hr))
		return FALSE;

	DSBPOSITIONNOTIFY PositionNotify;
	PositionNotify.dwOffset = DSBPN_OFFSETSTOP;	// 再生終了を指定
	PositionNotify.hEventNotify = hEvent;
	hr = pDSNotify->SetNotificationPositions(1					// DSBPOSITIONNOTIFY構造体の数
		, &PositionNotify);	// DSBPOSITIONNOTIFY構造体のアドレス
	SAFE_RELEASE(pDSNotify);
	if (hr == DS_OK)
		return TRUE;
	return FALSE;
}