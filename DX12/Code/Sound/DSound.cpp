#include "DSound.h"
#include "CWaveFile.h"
// �R���X�g���N�^
CDirectSound::CDirectSound() : m_pDS(NULL)
{

}
// �f�X�g���N�^
CDirectSound::~CDirectSound()
{
	ReleaseObjects();
}
// ------------------------------------------------------------------------
//�@�֐���	:Create						�_�C���N�g�T�E���h�쐬
//	����	:hWnd						�E�B���h�E�n���h��
//			:freq						���g��
//	�߂�l	:							TRUE:�����AFALSE:���s
// ------------------------------------------------------------------------
BOOL CDirectSound::Create(HWND hWnd, int freq)
{
	HRESULT hr;
	// DirectSound�C���^�t�F�[�X�̎擾
	if (FAILED(hr = DirectSoundCreate(NULL, &m_pDS, NULL)))
		return FALSE;
	// �������x����ݒ�
	if (FAILED(hr = m_pDS->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)))
		return FALSE;
	// �v���C�}���[�o�b�t�@�̍쐬
	return Primary.CreatePrimary(*this, freq);

	return TRUE;
}
// ------------------------------------------------------------------------
//�@�֐���	:ReleaseObjects				�J������
//	����	:							�Ȃ�
//	�߂�l	:							�Ȃ�
// ------------------------------------------------------------------------
void CDirectSound::ReleaseObjects()
{
	if (m_pDS) {
		Primary.Release();
		m_pDS->Release();
		m_pDS = NULL;
	}
}
// �R���X�g���N�^
CDSoundBuffer::CDSoundBuffer() :
	Volume(0), Pan(0), Frequency(0), BufferBytes(0), bPlay(TRUE), NumPlay(-1)
{
	::ZeroMemory(&Wfx, sizeof(Wfx));
	::ZeroMemory(&buffer, sizeof(buffer));
}
// �f�X�g���N�^
CDSoundBuffer::~CDSoundBuffer()
{
	if (buffer)
		buffer->Release();
}
// ------------------------------------------------------------------------
//�@�֐���	:Create						�T�E���h�o�b�t�@�쐬
//	����	:ds							�_�C���N�g�T�E���h�̃A�h���X
//			:pFormat					�E�F�[�u�t�H�[�}�b�g
//			:dwBufferBytes				�o�b�t�@�T�C�Y
//	�߂�l	:							TRUE:�����AFALSE:���s
// ------------------------------------------------------------------------
BOOL CDSoundBuffer::Create(CDirectSound& ds, WAVEFORMATEX* pFormat, DWORD dwBufferBytes)
{
	HRESULT	hr;
	DSBUFFERDESC dsbd;

	// DSBUFFERDESC�\���̂̐ݒ�
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS
		| DSBCAPS_LOCDEFER | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY;
	dsbd.dwBufferBytes = dwBufferBytes;
	dsbd.lpwfxFormat = pFormat;
	// ���̃o�b�N�A�b�v
	BufferBytes = dwBufferBytes;
	memcpy(&Wfx, pFormat, sizeof(WAVEFORMATEX));

	// �T�E���h�o�b�t�@�̍쐬
	if (FAILED(hr = ds->CreateSoundBuffer(&dsbd, &buffer, NULL)))
		return FALSE;

	// ���̎擾
	buffer->GetVolume(&Volume);
	buffer->GetPan(&Pan);
	buffer->GetFrequency(&Frequency);

	return TRUE;
}
// ------------------------------------------------------------------------
//�@�֐���	:CreatePrimary				�v���C�}���o�b�t�@�쐬
//	����	:ds							�_�C���N�g�T�E���h�̃A�h���X
//			:dwPrimaryFreq				���g��
//	�߂�l	:							TRUE:�����AFALSE:���s
// ------------------------------------------------------------------------
BOOL CDSoundBuffer::CreatePrimary(CDirectSound& ds, DWORD dwPrimaryFreq)
{
	HRESULT hr;

	// DSBUFFERDESC�\���̂̐ݒ�
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes = 0;
	dsbd.lpwfxFormat = NULL;

	// �T�E���h�o�b�t�@�̍쐬
	if (FAILED(hr = ds->CreateSoundBuffer(&dsbd, &buffer, NULL)))
		return FALSE;

	// WAVE�t�H�[�}�b�g�̐ݒ�
	WAVEFORMATEX wfx;
	ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 2;
	wfx.nSamplesPerSec = dwPrimaryFreq;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = 4;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	// �T�E���h�o�b�t�@�t�H�[�}�b�g�̐ݒ�
	if (FAILED(hr = buffer->SetFormat(&wfx)))
		return FALSE;

	buffer->GetVolume(&Volume);
	buffer->GetPan(&Pan);
	buffer->GetFrequency(&Frequency);

	return TRUE;
}
// ------------------------------------------------------------------------
//�@�֐���	:Release					�J������
//	����	:							�Ȃ�
//	�߂�l	:							S_OK:����
// ------------------------------------------------------------------------
HRESULT CDSoundBuffer::Release()
{
	if (buffer)
		HRESULT hr = buffer->Release();
	buffer = NULL;

	return S_OK;
}
// ------------------------------------------------------------------------
//�@�֐���	:Restore					���X�g�A����
//	����	:							�Ȃ�
//	�߂�l	:							S_OK:����
// ------------------------------------------------------------------------
HRESULT CDSoundBuffer::Restore()
{
	if (buffer)
		HRESULT hr = buffer->Restore();

	return S_OK;
}
// ------------------------------------------------------------------------
//�@�֐���	:LoadWave					�t�@�C���̓ǂݍ���
//	����	:ds							�_�C���N�g�T�E���h�̃A�h���X
//			:pFileName					�t�@�C����
//	�߂�l	:							TRUE:�����AFALSE:���s
// ------------------------------------------------------------------------
BOOL CDSoundBuffer::LoadWave(CDirectSound& ds, char* pFileName)
{
	CWaveFile file;
	if (!file.Open(pFileName) || !file.StartRead())
		return FALSE;

	Release();	// �o�b�t�@�������Release����

	// �V����WAVE�̃T�C�Y�Ńo�b�t�@���쐬����
	if (!Create(ds, file.GetFormat(), file.CkSize()))
		return FALSE;

	// �o�b�t�@��Wave�f�[�^�ɏ�������
	LPVOID pFirstDest, pSecondDest;
	DWORD dwFirstDestBufferBytes, dwSecondDestBufferBytes;

	HRESULT hr = buffer->Lock(
		0,							// �I�t�Z�b�g�l���o�C�g�P��
		file.CkSize(),				// ���b�N�����o�b�t�@�����̃T�C�Y���o�C�g�P��
		&pFirstDest,				// �ŏ��Ƀ��b�N���ꂽ�����ւ̃|�C���^
		&dwFirstDestBufferBytes,	// �u���b�N�̃o�C�g��
		&pSecondDest,				// ��ꂽ�u���b�N�̃��b�N���ꂽ�����ւ̃|�C���^
		&dwSecondDestBufferBytes,	// ��ꂽ�u���b�N�̃o�C�g��
		0L);
	if (hr == DSERR_BUFFERLOST) {
		Restore();
		hr = buffer->Lock(0,			// �I�t�Z�b�g�l���o�C�g�P��
			file.CkSize(),				// ���b�N�����o�b�t�@�����̃T�C�Y���o�C�g�P��
			&pFirstDest,				// �ŏ��Ƀ��b�N���ꂽ�����ւ̃|�C���^
			&dwFirstDestBufferBytes,	// �u���b�N�̃o�C�g��
			&pSecondDest,				// ��ꂽ�u���b�N�̃��b�N���ꂽ�����ւ̃|�C���^
			&dwSecondDestBufferBytes,	// ��ꂽ�u���b�N�̃o�C�g��
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
// �R���X�g���N�^
CDSBufferNotify::CDSBufferNotify()
{
	pDSNotify = NULL;
}
// �f�X�g���N�^
CDSBufferNotify::~CDSBufferNotify()
{
}
// ------------------------------------------------------------------------
//�@�֐���	:SetNotify					���t�I���ʒm�C�x���g�Z�b�g
//	����	:buffer						�T�E���h�o�b�t�@
//			:hEvent						���t�I�����ɔ�������C�x���g�n���h��
//	�߂�l	:							TRUE:�����AFALSE:���s
// ------------------------------------------------------------------------
BOOL CDSBufferNotify::SetNotify(CDSoundBuffer& buffer, HANDLE hEvent)
{
	// �C���^�[�t�F�C�X���w���|�C���^�̎擾
	HRESULT hr = buffer->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&pDSNotify);
	if (FAILED(hr))
		return FALSE;

	DSBPOSITIONNOTIFY PositionNotify;
	PositionNotify.dwOffset = DSBPN_OFFSETSTOP;	// �Đ��I�����w��
	PositionNotify.hEventNotify = hEvent;
	hr = pDSNotify->SetNotificationPositions(1					// DSBPOSITIONNOTIFY�\���̂̐�
		, &PositionNotify);	// DSBPOSITIONNOTIFY�\���̂̃A�h���X
	SAFE_RELEASE(pDSNotify);
	if (hr == DS_OK)
		return TRUE;
	return FALSE;
}