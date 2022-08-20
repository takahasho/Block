#ifndef __DX12SYSTEM_H__
#define __DX12SYSTEM_H__
#pragma once
//--------------------- Define宣言 ------------------
#define DX					DX12::GetInstance()	//DX12Instance
// --------------------- Include File --------------------
#include "Global.h"
#include <stdexcept>
#include "VECTOR2.h"
#include "VECTOR.h"
// ---------------------- クラス宣言 ---------------------

class DX12
{
private:
	struct TextData
	{
		ComPtr<IDWriteTextLayout> _textLayout;
		ComPtr<ID2D1Brush> _brush;
		D2D1_POINT_2F _pos;
		float _fontSize;
	};
private:
	bool _use11On12;		// false:使わない,true:使う
	static const UINT FrameCount = 2;
	const float _baseFontSize;
	const float _baseFontMiniSize;
	// ---------------- メンバ変数宣言 ----------------
	ComPtr<ID3D12Device> _dev;				// デバイス
	ComPtr<IDXGIFactory6> _dxgiFactory;		// ファクトリー
	ComPtr<IDXGISwapChain4> _swapchain;		// スワップチェーン

	ComPtr<ID3D12CommandAllocator> _cmdAllocator;	// コマンドアロケーター
	ComPtr<ID3D12GraphicsCommandList> _cmdList;		// コマンドリスト

	ComPtr<ID3D12CommandQueue> _cmdQueue;	// コマンドキュー

	ComPtr<ID3D12Resource> _backBuffers[FrameCount];	// レンダーターゲット

	ComPtr<ID3D12DescriptorHeap> _rtvHeaps;	// レンダーターゲットビューのディスクリプターヒープ

	ComPtr<ID3D12Fence> _fence;				// フェンス
	UINT64 _fenceVal;						// フェンスのインデックス
	HANDLE _fenceEvent;						// フェンスイベント

	ComPtr<ID3D12Resource> _depthBuffer;	// 深度バッファー
	ComPtr<ID3D12DescriptorHeap> _dsvHeaps;	// 深度ステンシルバッファービューヒープ

	// ----- D3D11On12 -----
	ComPtr<ID3D11DeviceContext> _d3d11DeviceContext;
	ComPtr<ID3D11On12Device> _d3d11On12Device;
	ComPtr<IDWriteFactory> _dWriteFactory;
	ComPtr<ID2D1Factory3> _d2dFactory;
	ComPtr<ID2D1Device2> _d2dDevice;
	ComPtr<ID2D1DeviceContext2> _d2dDeviceContext;
	ComPtr<ID3D11Resource> _wrappedBackBuffers[FrameCount];
	ComPtr<ID2D1Bitmap1> _d2dRenderTargets[FrameCount];
	ComPtr<ID3D12CommandAllocator> _commandAllocators;

	// リソース
	ComPtr<IWICImagingFactory>	_imagingFactory;	// イメージファクトリー
	ComPtr<IDWriteTextFormat> _textFormat;
	ComPtr<IDWriteTextFormat> _miniTextFormat;
	std::vector<TextData>	_textData;
	std::vector<bool>		_textDataCheck;
private:
	HRESULT CreateDX12();					// DX12の生成
	HRESULT CreateDevice();					// デバイスの生成
	HRESULT CreateFactory();				// ファクトリーの生成
	HRESULT CreateCommandAllocatorList();	// コマンドアロケーターとリストの生成
	HRESULT CreateCommandQueue();			// コマンドキューの生成
	HRESULT CreateSwapChain();				// スワップチェーンの生成
	HRESULT CreateD3D11On12();				// Direct3D11On12Deviceなど作成
	HRESULT CreateRenderTargetView();		// レンダーターゲットビューの生成
	void LoadAssets();						// fontなどのアセットをロードする
	HRESULT CreateFence();					// フェンスの生成

	HRESULT CreateDepthStencilView();		// デプスステンシルビューの生成
public:
	// ---------------- メンバ変数宣言 ----------------
	static DX12* _instance;				// シングルトン
	DX12();		// コンストラクター
	~DX12();								// デストラクター
	static void Create();					// 生成
	static void Destroy();					// 破棄
	void BeginDraw();						// 全体の描画準備
	void EndDraw();							// 全体の描画終了
	void WaitForPreviousFrame();			// GPUが完了するまで待機
	void TextLayoutDeleate(int& entryPos);				// textLayout削除
	void SetTextLayout(int& entryPos, VECTOR2 Pos, VECTOR Color, const TCHAR* pFormat,
		float maxWidth, float maxHeight, bool underLine = false, DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_NORMAL, bool miniText = false);		// textLayout作成
	void DrawTextFormat();	// 文字の描画

	// ゲッターセッター
	ComPtr<ID3D12Device> GetDevice() const {
		return _dev;
	}
	ComPtr<IDXGISwapChain4> Swapchain() const {
		return _swapchain;
	}
	ComPtr<ID3D12CommandAllocator> CommandAllocator() const {
		return _cmdAllocator;
	}
	ComPtr<ID3D12GraphicsCommandList> CommandList() const {
		return _cmdList;
	}
	ComPtr<ID3D12CommandQueue> CommandQueue() const {
		return _cmdQueue;
	}
	static DX12* GetInstance() {
		return _instance;
	}
};
#endif	/* __DX12SYSTEM_H__ */