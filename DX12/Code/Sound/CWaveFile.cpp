#include "CWaveFile.h"
// �R���X�g���N�^
CWaveFile::CWaveFile() : m_pwfx(NULL)
{
	::ZeroMemory(&m_hmmio, sizeof(m_hmmio));
	::ZeroMemory(&m_ckRiff, sizeof(m_ckRiff));
	::ZeroMemory(&m_ck, sizeof(m_ck));
}
// �f�X�g���N�^
CWaveFile::~CWaveFile()
{
	Close();
}
// ------------------------------------------------------------------------
//�@�֐���	:Open						�E�F�[�u�t�@�C���̓ǂݍ���
//	����	:pFileName					�t�@�C����
//	�߂�l	:							TRUE:�����AFALSE:���s
// ------------------------------------------------------------------------
BOOL CWaveFile::Open(char* pFileName)
{
	MMCKINFO		ckIn;
	PCMWAVEFORMAT	pcmWaveFormat;

#ifdef UNICODE
	WCHAR* pwName = new WCHAR[(strlen(pFileName) + 1) * 2];

	MultiByteToWideChar(CP_ACP, 0, pFileName, -1, pwName
		, static_cast<UINT>(strlen(pFileName) + 1) * 2);
	WideCharToMultiByte(CP_UTF8, 0, pwName, -1
		, pFileName, static_cast<UINT>(wcslen(pwName) + 1) * 2, nullptr, nullptr);

	m_hmmio = mmioOpenW(pwName, NULL, MMIO_ALLOCBUF | MMIO_READ);
	SAFE_DELETE_ARRAY(pwName);
#else
	m_hmmio = mmioOpen(pFileName, NULL, MMIO_ALLOCBUF | MMIO_READ);
#endif
	if (m_hmmio == NULL)
		return FALSE;

	// ���̓t�@�C����'RIFF'�`�����N�ֈړ�
	if (mmioDescend(m_hmmio, &m_ckRiff, NULL, 0) != 0)
		return FALSE;

	// ���̓t�@�C����WAVE�t�@�C���ł��邩�m�F
	if ((m_ckRiff.ckid != FOURCC_RIFF)
		|| (m_ckRiff.fccType != mmioFOURCC('W', 'A', 'V', 'E')))
		return FALSE;

	// ���̓t�@�C������'fmt'�`�����N������
	ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (mmioDescend(m_hmmio, &ckIn, &m_ckRiff, MMIO_FINDCHUNK) != 0)
		return FALSE;

	// 'fmt'�`�����N�̃T�C�Y�����Ȃ��Ƃ�<PCMWAVEFORMAT>
	// �����x�̃T�C�Y�ł��邱�Ƃ�z��
	if (ckIn.cksize < (LONG)sizeof(PCMWAVEFORMAT))
		return FALSE;

	// <wfex>��'fmt'�`�����N��ǂݎ��
	if (mmioRead(m_hmmio, (HPSTR)&pcmWaveFormat,
		sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat))
		return FALSE;

	// �t�H�[�}�b�g��'PCM'�̏ꍇ
	if (pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM) {
		m_pwfx = (WAVEFORMATEX*)new CHAR[sizeof(WAVEFORMATEX)];
		if (NULL == m_pwfx)
			return FALSE;
		// �E�F�[�u�t�H�[�}�b�g�̃R�s�[
		memcpy(m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat));
		m_pwfx->cbSize = 0;		// �v���C�}���o�b�t�@��0
	}
	else {
		// �t�H�[�}�b�g��'PCM'�ȊO�̏ꍇ
		WORD cbExtraBytes = 0L;
		if (mmioRead(m_hmmio, (CHAR*)&cbExtraBytes, sizeof(WORD)) != sizeof(WORD))
			return FALSE;
		m_pwfx = (WAVEFORMATEX*)new CHAR[sizeof(WAVEFORMATEX) + cbExtraBytes];
		if (NULL == m_pwfx)
			return FALSE;
		// �E�F�[�u�t�H�[�}�b�g�̃R�s�[
		memcpy(m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat));
		m_pwfx->cbSize = cbExtraBytes;
		// Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
		if (mmioRead(m_hmmio, (CHAR*)(((BYTE*)&(m_pwfx->cbSize)) + sizeof(WORD)),
			cbExtraBytes) != cbExtraBytes) {
			SAFE_DELETE(m_pwfx);
			return FALSE;
		}
	}
	// 'fmt'�`�����N�̊O�֓��̓t�@�C�����ړ�
	if (mmioAscend(m_hmmio, &ckIn, 0) != 0) {
		SAFE_DELETE(m_pwfx);
		return FALSE;
	}

	return TRUE;
}
// ------------------------------------------------------------------------
//�@�֐���	:StartRead					�ǂݍ��ݏꏊ��data�Ɉړ�
//	����	:							�Ȃ�
//	�߂�l	:							TRUE:�����AFALSE:���s
// ------------------------------------------------------------------------
BOOL CWaveFile::StartRead()
{
	if (mmioSeek(m_hmmio, m_ckRiff.dwDataOffset + sizeof(FOURCC), SEEK_SET) == -1)
		return FALSE;

	// ���̓t�@�C������'data'�`�����N������
	m_ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (0 != mmioDescend(m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK))
		return FALSE;	// �f�[�^�`�����N����

	return TRUE;
}
// ------------------------------------------------------------------------
//�@�֐���	:Read						���̓ǂݍ���
//	����	:pBuffer					�i�[�o�b�t�@
//			;dwSizeToRead				�ǂݍ��ރT�C�Y
//			:pdwSizeRead				�ǂݍ��񂾃T�C�Y�i�[�p�A�h���X
//	�߂�l	:							TRUE:�����AFALSE:���s
// ------------------------------------------------------------------------
BOOL CWaveFile::Read(LPVOID pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead)
{
	*pdwSizeRead = 0;
	MMIOINFO	mmioinfoIn; // current status of m_hmmio

	// ���̓o�b�t�@�̒��ڃA�N�Z�X���邽�߁A�t�@�C�����擾
	if (mmioGetInfo(m_hmmio, &mmioinfoIn, 0) != 0)
		return FALSE;

	UINT cbDataIn = dwSizeToRead;
	if (cbDataIn > m_ck.cksize)
		cbDataIn = m_ck.cksize;

	m_ck.cksize -= cbDataIn;

	for (DWORD cT = 0; cT < cbDataIn; cT++) {
		// �o�b�t�@�̏I�[�ɒB������A�o�b�t�@�Ƀf�[�^��ǂݍ���
		if (mmioinfoIn.pchNext == mmioinfoIn.pchEndRead) {
			if ((mmioAdvance(m_hmmio, &mmioinfoIn, MMIO_READ) != 0)
				|| (mmioinfoIn.pchNext == mmioinfoIn.pchEndRead))
				return FALSE;
		}

		// Actual copy.
		*((BYTE*)pBuffer + cT) = *((BYTE*)mmioinfoIn.pchNext);
		mmioinfoIn.pchNext++;
	}

	// �o�b�t�@�̏�Ԃ��X�V���A���̓��͂ɔ�����
	if (mmioSetInfo(m_hmmio, &mmioinfoIn, 0) != 0)
		return FALSE;

	if (pdwSizeRead != NULL)
		*pdwSizeRead = cbDataIn;

	return TRUE;
}
// ------------------------------------------------------------------------
//�@�֐���	:Close						�t�@�C�������
//	����	:							�Ȃ�
//	�߂�l	:							TRUE:�����AFALSE:���s
// ------------------------------------------------------------------------
BOOL CWaveFile::Close()
{
	delete[](char*)m_pwfx;

	if (m_hmmio != NULL) {
		if (FAILED(mmioClose(m_hmmio, 0)))
			return FALSE;
	}

	return TRUE;
}

