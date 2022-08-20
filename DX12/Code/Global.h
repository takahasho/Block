#ifndef __GLOBAL_H__
#define __GLOBAL_H__
#pragma once
// ---------------------- define宣言 ---------------------
#define UM_CURSORSTATUS		WM_USER + 101	// カーソルの状態

#define SAFE_DELETE(p)		{if(p){delete(p);    (p)=nullptr;}}		//オブジェクト開放マクロ
#define SAFE_DELETE_ARRAY(p){if(p){delete[](p);    (p)=nullptr;}}	//オブジェクト開放マクロ
#define SAFE_RELEASE(p)		{if(p){(p)->Release();    (p)=nullptr;}}//オブジェクト開放マクロ
// --------------- Include File ---------------
#include <windows.h>
#include <vector>
#include <wrl.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <atlbase.h>            // ATL 
#include <wincodec.h>           // WIC 
#include <wincodecsdk.h>        // WIC
#include <dwrite.h>				// WIC
#include <locale.h>				// ロケール
#include <thread>
#include <string>
#include <map>
// ------ D3D11On12 -----
#include <d2d1_3.h>
#include <dwrite.h>
#include <d3d11on12.h>
#include "d3dx12.h"
#include <pix.h>
#include <shellapi.h>
// --------------------- Library File ---------------------
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "WindowsCodecs.lib")
#pragma comment(lib, "winmm.lib")
// --------------- テンプレート ---------------------
template <class T> void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}
// -------------- using namespace -------------
using namespace Microsoft::WRL;
using namespace DirectX;

// ------------ グローバル変数宣言 ------------
extern UINT g_window_width;	// ウィンドウ幅
extern UINT g_window_height;// ウィンドウ高さ
extern HWND g_hwnd;			// ウィンドウハンドル
extern FLOAT g_elapsedTime; // deltaTime
extern BOOL g_FullScreen;	// ウィンドウの最大化の有無
// ------------ グローバル関数宣言 ------------
extern void Update();						// アップデート

#endif	/* __GLOBAL_H__ */