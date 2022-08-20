#ifndef __EFFECT_H__
#define __EFFECT_H__
#pragma once
//--------------------- Define�錾 ------------------
#define PLAY_ONESHOT(p)		{p->playType = PlayType::ONESHOT; p->time =0;}//���Đ�
#define PLAY_STOP(p)		{p->playType = PlayType::STOP; p->time =0;}	//��~
#define PLAY_LOOP(p)		{p->playType = PlayType::LOOP; p->time =0;}	//���[�v
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
// -------------------- �N���X�錾 -------------------
class Effect
{
private:
	// ---------------- �����o�ϐ��錾 ----------------
	static Effect* _instance;				// �V���O���g��
	std::vector<EFFEKSEER*> effects;
	// ---------- �֐��錾 ----------
	void EffekseerDestroy(EFFEKSEER* effect);	// Effekseer�I��
public:
	Effect();										// �R���X�g���N�^�[
	~Effect();									// �f�X�g���N�^�[
	static void Create();					// ����
	static void Destroy();					// �j��
	EFFEKSEER* EffekseerCreate(const char* EffectName);	// Effekseer����
	void EffekseerDraw(EFFEKSEER* effect, VECTOR position, VECTOR angle, VECTOR scale, float speed);		// Effekseer�Đ�
	static Effect* GetInstance() {
		return _instance;
	}
};
#endif	/* __EFFECT_H__ */