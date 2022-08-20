#pragma once
// --------------------- Include File --------------------
#include <Effekseer.h>
#include <EffekseerRendererDX12.h>
#include <XAudio2.h>
#include <EffekseerSoundXAudio2.h>
// -------------- �񋓌^�錾 -------------
enum class PlayType
{
	STOP,		// ��~
	ONESHOT,	// ���Đ�
	LOOP,		// ���[�v�Đ�
};
// ---------- �\���̐錾 ----------
typedef struct _Effekseer_
{
	PlayType playType;			// �Đ����@�iPLAYTYPE_STOP�F��~�APLAYTYPE_ONESHOT�F���Đ��APLAYTYPE_LOOP�F���[�v�Đ��j
	// �G�t�F�N�g�����_���[
	EffekseerRenderer::RendererRef renderer;
	// �G�t�F�N�g�}�l�[�W���[
	Effekseer::ManagerRef manager;
	// �������v�[��
	Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> sfMemoryPoolEfk;
	// �R�}���h���X�g
	Effekseer::RefPtr < EffekseerRenderer::CommandList> commandListEfk;
	// �T�E���h
	EffekseerSound::SoundRef sound;
	IXAudio2* g_xa2;
	IXAudio2MasteringVoice* g_xa2_master;
	// ---- �G�t�F�N�g�Đ��ɕK�v�Ȃ��� ---------------
	// �G�t�F�N�g�{�́i�G�t�F�N�g�t�@�C���ɑΉ��j
	Effekseer::EffectRef effect;
	// �G�t�F�N�g�n���h���i�Đ����̃G�t�F�N�g�ɑΉ��j
	Effekseer::Handle handle;
	// �G�t�F�N�g�^�C���i���ԍX�V�ɕK�v�j
	FLOAT time = 0;
	_Effekseer_()
	{
		playType = PlayType::LOOP;
		g_xa2 = nullptr;
		g_xa2_master = nullptr;
		handle = 0;
	}
}EFFEKSEER;
