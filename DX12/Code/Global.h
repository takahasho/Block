#ifndef __GLOBAL_H__
#define __GLOBAL_H__
#pragma once
// ---------------------- define�錾 ---------------------
#define UM_CURSORSTATUS		WM_USER + 101	// �J�[�\���̏��

#define SAFE_DELETE(p)		{if(p){delete(p);    (p)=nullptr;}}		//�I�u�W�F�N�g�J���}�N��
#define SAFE_DELETE_ARRAY(p){if(p){delete[](p);    (p)=nullptr;}}	//�I�u�W�F�N�g�J���}�N��
#define SAFE_RELEASE(p)		{if(p){(p)->Release();    (p)=nullptr;}}//�I�u�W�F�N�g�J���}�N��
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
#include <locale.h>				// ���P�[��
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
// --------------- �e���v���[�g ---------------------
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

// ------------ �O���[�o���ϐ��錾 ------------
extern UINT g_window_width;	// �E�B���h�E��
extern UINT g_window_height;// �E�B���h�E����
extern HWND g_hwnd;			// �E�B���h�E�n���h��
extern FLOAT g_elapsedTime; // deltaTime
extern BOOL g_FullScreen;	// �E�B���h�E�̍ő剻�̗L��
// ------------ �O���[�o���֐��錾 ------------
extern void Update();						// �A�b�v�f�[�g

#endif	/* __GLOBAL_H__ */