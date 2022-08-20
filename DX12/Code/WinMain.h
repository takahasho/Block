#pragma once

#include "Scene/Game.h"
UINT g_window_width = 1920;	// ウィンドウ幅
UINT g_window_height =1080;	// ウィンドウ高さ
FLOAT g_elapsedTime = 0;
BOOL g_FullScreen = TRUE;
HWND g_hwnd = nullptr;		// ウィンドウハンドル
bool _loopFlag = true;		// スレッドループを抜けるためのフラグ
std::chrono::system_clock::time_point  start, end; // 型は auto で可

Game game;