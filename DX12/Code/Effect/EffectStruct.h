#pragma once
// --------------------- Include File --------------------
#include <Effekseer.h>
#include <EffekseerRendererDX12.h>
#include <XAudio2.h>
#include <EffekseerSoundXAudio2.h>
// -------------- 列挙型宣言 -------------
enum class PlayType
{
	STOP,		// 停止
	ONESHOT,	// 一回再生
	LOOP,		// ループ再生
};
// ---------- 構造体宣言 ----------
typedef struct _Effekseer_
{
	PlayType playType;			// 再生方法（PLAYTYPE_STOP：停止、PLAYTYPE_ONESHOT：一回再生、PLAYTYPE_LOOP：ループ再生）
	// エフェクトレンダラー
	EffekseerRenderer::RendererRef renderer;
	// エフェクトマネージャー
	Effekseer::ManagerRef manager;
	// メモリプール
	Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> sfMemoryPoolEfk;
	// コマンドリスト
	Effekseer::RefPtr < EffekseerRenderer::CommandList> commandListEfk;
	// サウンド
	EffekseerSound::SoundRef sound;
	IXAudio2* g_xa2;
	IXAudio2MasteringVoice* g_xa2_master;
	// ---- エフェクト再生に必要なもの ---------------
	// エフェクト本体（エフェクトファイルに対応）
	Effekseer::EffectRef effect;
	// エフェクトハンドル（再生中のエフェクトに対応）
	Effekseer::Handle handle;
	// エフェクトタイム（時間更新に必要）
	FLOAT time = 0;
	_Effekseer_()
	{
		playType = PlayType::LOOP;
		g_xa2 = nullptr;
		g_xa2_master = nullptr;
		handle = 0;
	}
}EFFEKSEER;
