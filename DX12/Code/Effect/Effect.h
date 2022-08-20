#ifndef __EFFECT_H__
#define __EFFECT_H__
#pragma once
//--------------------- Define宣言 ------------------
#define PLAY_ONESHOT(p)		{p->playType = PlayType::ONESHOT; p->time =0;}//一回再生
#define PLAY_STOP(p)		{p->playType = PlayType::STOP; p->time =0;}	//停止
#define PLAY_LOOP(p)		{p->playType = PlayType::LOOP; p->time =0;}	//ループ
// --------------------- Include File --------------------
#include "../Global.h"
#include "../Camera/Camera.h"
#include "../DX12System.h"
#include "../VECTOR.h"
#include "EffectStruct.h"
#include <stdio.h>
// ------ Effekseer ------
#include <Window.h>
// --------------------- Library File ---------------------
#pragma comment(lib, "xaudio2.lib")
// -------------------- クラス宣言 -------------------
class Effect
{
private:
	// ---------------- メンバ変数宣言 ----------------
	static Effect* _instance;				// シングルトン
	std::vector<EFFEKSEER*> effects;
	// ---------- 関数宣言 ----------
	void EffekseerDestroy(EFFEKSEER* effect);	// Effekseer終了
public:
	Effect();										// コンストラクター
	~Effect();									// デストラクター
	static void Create();					// 生成
	static void Destroy();					// 破棄
	EFFEKSEER* EffekseerCreate(const char* EffectName);	// Effekseer生成
	void EffekseerDraw(EFFEKSEER* effect, VECTOR position, VECTOR angle, VECTOR scale, float speed);		// Effekseer再生
	static Effect* GetInstance() {
		return _instance;
	}
};
#endif	/* __EFFECT_H__ */