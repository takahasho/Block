#include "CSoundManager.h"
// �R���X�g���N�^
CSoundManager::CSoundManager(HWND hWnd)
{
	pDSBuffer = new CDSoundBuffer[NUMEFFECTSOUND];			// �T�E���h�o�b�t�@
	pDSSecondaryBuffer = new CDSoundBuffer[NUMSECONDARY];	// �Z�J���_���T�E���h�o�b�t�@

	// ��~���ɔ�������C�x���g���쐬
	for (DWORD i = 0; i < NUMSECONDARY + 1; i++)
		hEvent[i] = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	DWORD ThreadID;
	hThread = CreateThread(NULL, 0, EventNotifyProc, this, 0, &ThreadID);
}
// �f�X�g���N�^
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
	// �I���ʒm�C�x���g���Z�b�g
	SetEvent(hEvent[NUMSECONDARY]);
	// �X���b�h�̏I����҂�
	WaitForSingleObject(hThread, INFINITE);
	// �X���b�h�n���h�������
	CloseHandle(hThread);
	// �C�x���g�����
	for (i = 0; i < NUMSECONDARY + 1; i++) {
		if (hEvent[i])
			CloseHandle(hEvent[i]);
	}
}
// ------------------------------------------------------------------------
//�@�֐���	:LoadWave					�E�F�[�u�t�@�C���̓ǂݍ���
//	����	:SetPosition				�i�[�ԍ�
//			:pFileName					�t�@�C����
//	�߂�l	:							TRUE:�����AFALSE:���s
// ------------------------------------------------------------------------
BOOL CSoundManager::LoadWave(DWORD SetPosition, char* pFileName)
{
	if (!pDSBuffer[SetPosition].LoadWave(DirectSound, pFileName))
		return FALSE;

	return TRUE;
}
// ------------------------------------------------------------------------
//�@�֐���	:ChangeWave					�E�F�[�u�����Z�J���_���o�b�t�@�փR�s�[
//	����	:dwDest						�i�[�ԍ�
//			:dwSrc						���t�ԍ�
//	�߂�l	:							TRUE:�����AFALSE:���s
// ------------------------------------------------------------------------
BOOL CSoundManager::ChangeWave(DWORD dwDest, DWORD dwSrc)
{
	if (dwDest >= NUMSECONDARY || dwSrc >= NUMEFFECTSOUND)
		return FALSE;

	pDSSecondaryBuffer[dwDest].Release();

	WAVEFORMATEX WaveFormatEX = pDSBuffer[dwSrc].GetSetWaveFormat();
	if (!pDSSecondaryBuffer[dwDest].Create(DirectSound, &WaveFormatEX, pDSBuffer[dwSrc].GetBufferBytes()))
		return FALSE;

	// ��o�b�t�@�Ƀo�b�t�@�f�[�^����������
	LPVOID pFirstDest, pSecondDest, pFirstSrc, pSecondSrc;
	DWORD dwFirstDestBufferBytes, dwSecondDestBufferBytes;
	DWORD dwFirstSrcBufferBytes, dwSecondSrcBufferBytes;

	pDSSecondaryBuffer[dwDest]->Lock(
		0,							// �I�t�Z�b�g�l���o�C�g�P��
		pDSSecondaryBuffer[dwDest].GetBufferBytes(),	// ���b�N�����o�b�t�@�����̃T�C�Y���o�C�g�P��
		&pFirstDest,				// �ŏ��Ƀ��b�N���ꂽ�����ւ̃|�C���^
		&dwFirstDestBufferBytes,	// �u���b�N�̃o�C�g��
		&pSecondDest,				// ��ꂽ�u���b�N�̃��b�N���ꂽ�����ւ̃|�C���^
		&dwSecondDestBufferBytes,	// ��ꂽ�u���b�N�̃o�C�g��
		0L);
	pDSBuffer[dwSrc]->Lock(
		0,							// �I�t�Z�b�g�l���o�C�g�P��
		pDSBuffer[dwSrc].GetBufferBytes(),// ���b�N�����o�b�t�@�����̃T�C�Y���o�C�g�P��
		&pFirstSrc,					// �ŏ��Ƀ��b�N���ꂽ�����ւ̃|�C���^
		&dwFirstSrcBufferBytes,		// �u���b�N�̃o�C�g��
		&pSecondSrc,				// ��ꂽ�u���b�N�̃��b�N���ꂽ�����ւ̃|�C���^
		&dwSecondSrcBufferBytes,	// ��ꂽ�u���b�N�̃o�C�g��
		0L);
	memcpy_s(pFirstDest, dwFirstDestBufferBytes, pFirstSrc, dwFirstSrcBufferBytes);
	if (dwSecondDestBufferBytes)
		memcpy_s(pSecondDest, dwSecondDestBufferBytes, pSecondSrc, dwSecondSrcBufferBytes);
	pDSBuffer[dwSrc]->Unlock(pFirstSrc, dwFirstSrcBufferBytes, pSecondSrc, dwSecondSrcBufferBytes);
	pDSSecondaryBuffer[dwDest]->Unlock(pFirstDest, dwFirstDestBufferBytes, pSecondDest, dwSecondDestBufferBytes);

	return TRUE;
}
// ------------------------------------------------------------------------
//�@�֐���	:Play						���t�Đ�
//	����	:SetPosition				���t�ԍ�
//			:loop						���[�v�̗L��
//	�߂�l	:							�Ȃ�
// ------------------------------------------------------------------------
void CSoundManager::Play(DWORD SetPosition, BOOL loop)
{
	DWORD i;

	if (SetPosition >= NUMEFFECTSOUND)
		return;

	if (pDSBuffer[SetPosition].IsAlive()) {
		// �Z�J���_���o�b�t�@�ɁA�����T�E���h�����邩�m�F
		for (i = 0; i < NUMSECONDARY; i++) {
			if (pDSSecondaryBuffer[i].GetPlay() && pDSSecondaryBuffer[i].GetNumPlay() == (int)SetPosition)
				break;
		}
		// �����T�E���h���������̂ŁA�Đ�
		if (i < NUMSECONDARY) {
			pDSSecondaryBuffer[i].Play(loop);
			pDSSecondaryBuffer[i].SetPlay(FALSE);
		}
		// �Ȃ������ꍇ�̏���
		else {
			// ���s�\�ȃZ�J���_���o�b�t�@������
			for (i = 0; i < NUMSECONDARY; i++) {
				if (pDSSecondaryBuffer[i].GetPlay())
					break;
			}
			// ���݂����ꍇ
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
//�@�֐���	:Stop						���t���~
//	����	:SetPosition				���t�ԍ�
//	�߂�l	:							�Ȃ�
// ------------------------------------------------------------------------
void CSoundManager::Stop(DWORD SetPosition,bool Reset)
{
	DWORD i;

	if (SetPosition >= NUMEFFECTSOUND)
		return;

	// �Z�J���_���o�b�t�@�̉����ōĐ�����Ă��邩����
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
//�@�֐���	:StopAllSound				�S�Ẳ��t���~
//	����	:							�Ȃ�
//	�߂�l	:							�Ȃ�
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
//�@�֐���	:VolumeAllSound				���ʒ���
//	����	:SetPosition				���t�ԍ�
//			:volume						���ʑ傫��
//	�߂�l	:							�Ȃ�
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
	// ���ۂ�0�`-8000�Ȃ̂�-8000+volume*80
	volume = -8000 + volume * 80;

	GetDirectSound()->GetPrimary().SetVol(volume);
}
// ------------------------------------------------------------------------
//�@�֐���	:EventNotifyProc			�C�x���g�Ǘ��p�v���V�[�W��
//	����	:param						�A�h���X
//	�߂�l	:							�Ȃ�
// ------------------------------------------------------------------------
DWORD WINAPI CSoundManager::EventNotifyProc(LPVOID param)
{
	CSoundManager* p = (CSoundManager*)param;

	while (TRUE) {
		DWORD dwResult = WaitForMultipleObjects(NUMSECONDARY + 1,	// �I�u�W�F�N�g�n���h���̐�
			p->hEvent,	// �I�u�W�F�N�g�n���h���̔z��ւ̃|�C���^
			FALSE,		// �ҋ@�̎��(�̂ǂꂩ����V�O�i����ԂɂȂ����Ƃ�)
			INFINITE);	// �^�C���A�E�g���Ԃ��A�~���b�P��(���������������܂ő҂�)
		if (dwResult == NUMSECONDARY)
			ExitThread(0);
		else
			p->pDSSecondaryBuffer[dwResult].SetPlay(TRUE);
	}

	return 0;
}