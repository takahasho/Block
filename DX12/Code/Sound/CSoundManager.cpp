#include "CSoundManager.h"
// コンストラクタ
CSoundManager::CSoundManager(HWND hWnd)
{
	pDSBuffer = new CDSoundBuffer[NUMEFFECTSOUND];			// サウンドバッファ
	pDSSecondaryBuffer = new CDSoundBuffer[NUMSECONDARY];	// セカンダリサウンドバッファ

	// 停止時に発生するイベントを作成
	for (DWORD i = 0; i < NUMSECONDARY + 1; i++)
		hEvent[i] = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	DWORD ThreadID;
	hThread = CreateThread(NULL, 0, EventNotifyProc, this, 0, &ThreadID);
}
// デストラクタ
CSoundManager::~CSoundManager()
{
	DWORD i;

	for (i = 0; i < NUMEFFECTSOUND; i++)
		pDSBuffer[i].Release();
	if (pDSBuffer)
		delete[] pDSBuffer;
	for (i = 0; i < NUMSECONDARY; i++)
		pDSSecondaryBuffer[i].Release();
	if (pDSSecondaryBuffer)
		delete[] pDSSecondaryBuffer;
	// 終了通知イベントをセット
	SetEvent(hEvent[NUMSECONDARY]);
	// スレッドの終了を待つ
	WaitForSingleObject(hThread, INFINITE);
	// スレッドハンドルを閉じる
	CloseHandle(hThread);
	// イベントを閉じる
	for (i = 0; i < NUMSECONDARY + 1; i++) {
		if (hEvent[i])
			CloseHandle(hEvent[i]);
	}
}
// ------------------------------------------------------------------------
//　関数名	:LoadWave					ウェーブファイルの読み込み
//	引数	:SetPosition				格納番号
//			:pFileName					ファイル名
//	戻り値	:							TRUE:成功、FALSE:失敗
// ------------------------------------------------------------------------
BOOL CSoundManager::LoadWave(DWORD SetPosition, char* pFileName)
{
	if (!pDSBuffer[SetPosition].LoadWave(DirectSound, pFileName))
		return FALSE;

	return TRUE;
}
// ------------------------------------------------------------------------
//　関数名	:ChangeWave					ウェーブ情報をセカンダリバッファへコピー
//	引数	:dwDest						格納番号
//			:dwSrc						演奏番号
//	戻り値	:							TRUE:成功、FALSE:失敗
// ------------------------------------------------------------------------
BOOL CSoundManager::ChangeWave(DWORD dwDest, DWORD dwSrc)
{
	if (dwDest >= NUMSECONDARY || dwSrc >= NUMEFFECTSOUND)
		return FALSE;

	pDSSecondaryBuffer[dwDest].Release();

	WAVEFORMATEX WaveFormatEX = pDSBuffer[dwSrc].GetSetWaveFormat();
	if (!pDSSecondaryBuffer[dwDest].Create(DirectSound, &WaveFormatEX, pDSBuffer[dwSrc].GetBufferBytes()))
		return FALSE;

	// 空バッファにバッファデータを書き込む
	LPVOID pFirstDest, pSecondDest, pFirstSrc, pSecondSrc;
	DWORD dwFirstDestBufferBytes, dwSecondDestBufferBytes;
	DWORD dwFirstSrcBufferBytes, dwSecondSrcBufferBytes;

	pDSSecondaryBuffer[dwDest]->Lock(
		0,							// オフセット値をバイト単位
		pDSSecondaryBuffer[dwDest].GetBufferBytes(),	// ロックされるバッファ部分のサイズをバイト単位
		&pFirstDest,				// 最初にロックされた部分へのポインタ
		&dwFirstDestBufferBytes,	// ブロックのバイト数
		&pSecondDest,				// 溢れたブロックのロックされた部分へのポインタ
		&dwSecondDestBufferBytes,	// 溢れたブロックのバイト数
		0L);
	pDSBuffer[dwSrc]->Lock(
		0,							// オフセット値をバイト単位
		pDSBuffer[dwSrc].GetBufferBytes(),// ロックされるバッファ部分のサイズをバイト単位
		&pFirstSrc,					// 最初にロックされた部分へのポインタ
		&dwFirstSrcBufferBytes,		// ブロックのバイト数
		&pSecondSrc,				// 溢れたブロックのロックされた部分へのポインタ
		&dwSecondSrcBufferBytes,	// 溢れたブロックのバイト数
		0L);
	memcpy_s(pFirstDest, dwFirstDestBufferBytes, pFirstSrc, dwFirstSrcBufferBytes);
	if (dwSecondDestBufferBytes)
		memcpy_s(pSecondDest, dwSecondDestBufferBytes, pSecondSrc, dwSecondSrcBufferBytes);
	pDSBuffer[dwSrc]->Unlock(pFirstSrc, dwFirstSrcBufferBytes, pSecondSrc, dwSecondSrcBufferBytes);
	pDSSecondaryBuffer[dwDest]->Unlock(pFirstDest, dwFirstDestBufferBytes, pSecondDest, dwSecondDestBufferBytes);

	return TRUE;
}
// ------------------------------------------------------------------------
//　関数名	:Play						演奏再生
//	引数	:SetPosition				演奏番号
//			:loop						ループの有無
//	戻り値	:							なし
// ------------------------------------------------------------------------
void CSoundManager::Play(DWORD SetPosition, BOOL loop)
{
	DWORD i;

	if (SetPosition >= NUMEFFECTSOUND)
		return;

	if (pDSBuffer[SetPosition].IsAlive()) {
		// セカンダリバッファに、同じサウンドがあるか確認
		for (i = 0; i < NUMSECONDARY; i++) {
			if (pDSSecondaryBuffer[i].GetPlay() && pDSSecondaryBuffer[i].GetNumPlay() == (int)SetPosition)
				break;
		}
		// 同じサウンドがあったので、再生
		if (i < NUMSECONDARY) {
			pDSSecondaryBuffer[i].Play(loop);
			pDSSecondaryBuffer[i].SetPlay(FALSE);
		}
		// なかった場合の処理
		else {
			// 実行可能なセカンダリバッファを検索
			for (i = 0; i < NUMSECONDARY; i++) {
				if (pDSSecondaryBuffer[i].GetPlay())
					break;
			}
			// 実在した場合
			if (i < NUMSECONDARY) {
				pDSSecondaryBuffer[i].SetNumPlay((int)SetPosition);
				ChangeWave(i, SetPosition);
				Notify.SetNotify(pDSSecondaryBuffer[i], hEvent[i]);
				pDSSecondaryBuffer[i].Play(loop);
				pDSSecondaryBuffer[i].SetPlay(FALSE);
			}
		}
	}
}
// ------------------------------------------------------------------------
//　関数名	:Stop						演奏中止
//	引数	:SetPosition				演奏番号
//	戻り値	:							なし
// ------------------------------------------------------------------------
void CSoundManager::Stop(DWORD SetPosition,bool Reset)
{
	DWORD i;

	if (SetPosition >= NUMEFFECTSOUND)
		return;

	// セカンダリバッファの何処で再生されているか検索
	for (i = 0; i < NUMSECONDARY; i++) {
		if (pDSSecondaryBuffer[i].GetNumPlay() == (int)SetPosition)
			break;
	}
	if (i < NUMSECONDARY) {
		pDSSecondaryBuffer[i].Stop();
		pDSSecondaryBuffer[i].SetPlay(TRUE);
		if (Reset)
		{
			pDSSecondaryBuffer[i].SetNumPlay(-1);
		}
	}
}
// ------------------------------------------------------------------------
//　関数名	:StopAllSound				全ての演奏中止
//	引数	:							なし
//	戻り値	:							なし
// ------------------------------------------------------------------------
void CSoundManager::StopAllSound()
{
	for (DWORD i = 0; i < NUMSECONDARY; i++) {
		if (pDSSecondaryBuffer[i].GetNumPlay() != -1) {
			pDSSecondaryBuffer[i].Stop();
			pDSSecondaryBuffer[i].SetPlay(TRUE);
			pDSSecondaryBuffer[i].SetNumPlay(-1);
		}
	}
}
// ------------------------------------------------------------------------
//　関数名	:VolumeAllSound				音量調整
//	引数	:SetPosition				演奏番号
//			:volume						音量大きさ
//	戻り値	:							なし
// ------------------------------------------------------------------------
void CSoundManager::VolumeAllSound(int volume)
{
	if (volume >= 100)
	{
		volume = 100;
	}
	else if (volume <= 0)
	{
		volume = 0;
	}
	// 実際は0～-8000なので-8000+volume*80
	volume = -8000 + volume * 80;

	GetDirectSound()->GetPrimary().SetVol(volume);
}
// ------------------------------------------------------------------------
//　関数名	:EventNotifyProc			イベント管理用プロシージャ
//	引数	:param						アドレス
//	戻り値	:							なし
// ------------------------------------------------------------------------
DWORD WINAPI CSoundManager::EventNotifyProc(LPVOID param)
{
	CSoundManager* p = (CSoundManager*)param;

	while (TRUE) {
		DWORD dwResult = WaitForMultipleObjects(NUMSECONDARY + 1,	// オブジェクトハンドルの数
			p->hEvent,	// オブジェクトハンドルの配列へのポインタ
			FALSE,		// 待機の種類(のどれか一つがシグナル状態になったとき)
			INFINITE);	// タイムアウト時間を、ミリ秒単位(条件が満たされるまで待ち)
		if (dwResult == NUMSECONDARY)
			ExitThread(0);
		else
			p->pDSSecondaryBuffer[dwResult].SetPlay(TRUE);
	}

	return 0;
}