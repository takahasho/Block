#pragma once

#include "Scene/Game.h"
UINT g_window_width = 1920;	// �E�B���h�E��
UINT g_window_height =1080;	// �E�B���h�E����
FLOAT g_elapsedTime = 0;
BOOL g_FullScreen = TRUE;
HWND g_hwnd = nullptr;		// �E�B���h�E�n���h��
bool _loopFlag = true;		// �X���b�h���[�v�𔲂��邽�߂̃t���O
std::chrono::system_clock::time_point  start, end; // �^�� auto �ŉ�

Game game;