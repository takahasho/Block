// --------------- Include File ---------------
#include "DX12System.h"

DX12* DX12::_instance = nullptr;			// インスタンスの実態
// コンストラクタ
DX12::DX12()
	:_fenceVal(0), _fenceEvent(nullptr), _use11On12(true), _baseFontSize(40), _baseFontMiniSize(18)
{

}

// デストラクター
DX12::~DX12()
{
	if (_use11On12)
	{
		_imagingFactory.Reset();
	}
	// COM終了
	CoUninitialize();
	// GPUが完了するまで待機
	WaitForPreviousFrame();
	// イベントハンドルを閉じる
	CloseHandle(_fenceEvent);
}
// 生成
void DX12::Create()
{
	if (!_instance)
	{
		_instance = new DX12();

		// DX12を作成
		_instance->CreateDX12();
	}

}

// 生成
HRESULT DX12::CreateDX12()
{
	HRESULT hr = S_OK;

	// ファクトリーを作成
	hr = CreateFactory();
	if (FAILED(hr))
		return E_FAIL;

	// デバイスの生成
	hr = CreateDevice();
	if (FAILED(hr))
		return E_FAIL;

	// コマンドアロケーターとリストの生成
	if (FAILED(CreateCommandAllocatorList()))
		return E_FAIL;

	// コマンドキューの生成
	if (FAILED(CreateCommandQueue()))
		return E_FAIL;

	// スワップチェーンの生成
	if (FAILED(CreateSwapChain()))
		return E_FAIL;

	if (_use11On12)
	{
		// Direct3D11On12Deviceなど作成
		if (FAILED(CreateD3D11On12()))
			return E_FAIL;
	}

	// レンダーターゲットビューの生成
	if (FAILED(CreateRenderTargetView()))
		return E_FAIL;

	if (_use11On12)
	{
		LoadAssets();	// fontなどの読み込み
	}

	// フェンスの生成
	if (FAILED(CreateFence()))
		return E_FAIL;

	// デプスステンシルビューの生成
	if (FAILED(CreateDepthStencilView()))
		return E_FAIL;

	return hr;
}

// 破棄
void DX12::Destroy()
{
	delete _instance;
}
// デバイスの生成
HRESULT DX12::CreateDevice()
{
	HRESULT hr = S_OK;

	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	// Direct3Dデバイスの初期化
	D3D_FEATURE_LEVEL featureLevel = {};

	// グラフィックボード複数判定はなし
	for (auto lv : levels)
	{
		if (D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(_dev.ReleaseAndGetAddressOf())) == S_OK)
		{
			featureLevel = lv;
			break;		// 生成可能なバージョンが見つかったらループを打ち切り
		}
	}

	hr = D3D12CreateDevice(
		nullptr,
		featureLevel,
		IID_PPV_ARGS(_dev.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	return hr;
}
// ファクトリーの生成
HRESULT DX12::CreateFactory()
{
	HRESULT hr = S_OK;

	// デバッグレイヤーを有効
#if defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugLayer;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(debugLayer.ReleaseAndGetAddressOf()));
		if (FAILED(hr))
			return hr;
		// デバックレイヤーを有効化
		debugLayer->EnableDebugLayer();
		// 有効化したらインターフェイスを解放
		debugLayer->Release();
	}
#endif

	// ファクトリーを作成
#if defined(_DEBUG)
	CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(_dxgiFactory.ReleaseAndGetAddressOf()));
#else
	CreateDXGIFactory1(IID_PPV_ARGS(_dxgiFactory.ReleaseAndGetAddressOf()));
#endif

	return hr;
}

// コマンドアロケーターとリストの生成
HRESULT DX12::CreateCommandAllocatorList()
{
	HRESULT hr = S_OK;

	// コマンドアロケーターを作成
	hr = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(_cmdAllocator.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// コマンドアロケーターと合わせてコマンドリストを作成
	hr = _dev->CreateCommandList(0
		, D3D12_COMMAND_LIST_TYPE_DIRECT,
		_cmdAllocator.Get(), nullptr
		, IID_PPV_ARGS(_cmdList.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// 命令のクローズ
	_cmdList->Close();

	return hr;
}

// コマンドキューの生成
HRESULT DX12::CreateCommandQueue()
{
	HRESULT hr = S_OK;

	// コマンドキューの記述を用意
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	// タイムアウトなし
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	// アダプターを１つしか使わないときは０でよい
	cmdQueueDesc.NodeMask = 0;
	// プライオリティは特に指定なし
	cmdQueueDesc.Priority =
		D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	// コマンドリストとして合わせる
	cmdQueueDesc.Type =
		D3D12_COMMAND_LIST_TYPE_DIRECT;

	// DIRECT のキューとコマンド リストはどのようなコマンドも受け取り
	// COMPUTE のキューとコマンド リストは計算とコピーに関連するコマンドを受け取り
	// COPY のキューとコマンド リストはコピーのコマンドのみを受け取ります

	// コマンドキューを作成
	hr = _dev->CreateCommandQueue(
		&cmdQueueDesc, IID_PPV_ARGS(_cmdQueue.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	return hr;
}

// スワップチェーンの生成
HRESULT DX12::CreateSwapChain()
{
	HRESULT hr = S_OK;

	// スワップチェーンの記述を用意
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	// 画像解像度[幅]
	swapChainDesc.Width = g_window_width;
	// 画像解像度[高さ]
	swapChainDesc.Height = g_window_height;
	// ピクセルフォーマット[赤,緑,青,アルファーの8Bit]
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// ステレオ表示フラグ[3Dディスプレイのステレオモードなし]
	swapChainDesc.Stereo = FALSE;
	// マルチサンプルの指定[Count = 1, Quality = 0でよい]
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	// バッファーの利用方法[DXGI_USAGE_BACK_BUFFERでよい]
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	// バッファー数[ダブルバッファーなら２でよい]
	swapChainDesc.BufferCount = 2;

	// バックバッファーは伸び縮み可能
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	// フリップ後は速やかに破棄
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	// 特に指定なし
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	// ウィンドウ⇔フルスクリーン切り替え可能
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// スワップチェーンを作成
	hr = _dxgiFactory->CreateSwapChainForHwnd(
		_cmdQueue.Get(),
		g_hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)_swapchain.ReleaseAndGetAddressOf());
	if (FAILED(hr))
		return hr;

	return hr;
}

// Direct3D11On12Deviceなど作成
HRESULT DX12::CreateD3D11On12()
{
	HRESULT hr = S_OK;
	// DX12デバイスとDX11デバイスが共有にラップされたをデバイス作成
	UINT d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	ComPtr<ID3D11Device> d3d11Device;
	hr = D3D11On12CreateDevice(
		_dev.Get(),
		d3d11DeviceFlags,
		nullptr,
		0,
		reinterpret_cast<IUnknown**>(_cmdQueue.GetAddressOf()),
		1,
		0,
		&d3d11Device,
		&_d3d11DeviceContext,
		nullptr
	);
	// 11デバイスから11On12デバイスを整合
	hr = d3d11Device.As(&_d3d11On12Device);
	// COM開始
	if (CoInitialize(NULL) != S_OK)
		return E_FAIL;
	// D2D/DWrite コンポーネントを作成
	D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};
	{
		D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2dFactoryOptions, &_d2dFactory);
		ComPtr<IDXGIDevice> dxgiDevice;
		hr = _d3d11On12Device.As(&dxgiDevice);
		hr = _d2dFactory->CreateDevice(dxgiDevice.Get(), &_d2dDevice);
		hr = _d2dDevice->CreateDeviceContext(deviceOptions, &_d2dDeviceContext);
		// イメージファクトリーの作成
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, &_imagingFactory);
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &_dWriteFactory);
	}

	return S_OK;
}

// レンダーターゲットビューの生成
HRESULT DX12::CreateRenderTargetView()
{
	HRESULT hr = S_OK;

	D2D1_BITMAP_PROPERTIES1 bitmapProperties = {};
	if (_use11On12)
	{
		// D2Dのレンダーターゲット。
		float dpiX;
		float dpiY;
#pragma warning(push)
#pragma warning(disable : 4996) // GetDesktopDpi is deprecated.
		_d2dFactory->GetDesktopDpi(&dpiX, &dpiY);
#pragma warning(pop)
		bitmapProperties = D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			dpiX,
			dpiY
		);
	}
	// デスクリプタヒープを作成
	// ヒープの記述を用意
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	// レンダーターゲットビューなのでRTV
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	// GPUを１つだけ使用する想定
	heapDesc.NodeMask = 0;
	// 表裏の２つ
	heapDesc.NumDescriptors = FrameCount;
	// 特に指定なし[シェーダー側から見える必要がない]
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	// デスクリプタヒープを作成
	hr = _dev->CreateDescriptorHeap(&heapDesc
		, IID_PPV_ARGS(_rtvHeaps.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// スワップチェーン情報の取得
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	hr = _swapchain->GetDesc(&swcDesc);
	if (FAILED(hr))
		return hr;

	for (int idx = 0; idx < (int)swcDesc.BufferCount; ++idx)
	{
		// スワップチェーンとバッファーの関連付け
		hr = _swapchain->GetBuffer(idx
			, IID_PPV_ARGS(_backBuffers[idx].ReleaseAndGetAddressOf()));
		if (FAILED(hr))
			return hr;

		// レンダーターゲットビューを作成
		// デスクリプタヒープハンドルの先頭アドレスを取得
		D3D12_CPU_DESCRIPTOR_HANDLE handle = _rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		// デスクリプタヒープのサイズ分アドレスを移動させる
		handle.ptr += static_cast<UINT64>(idx) * _dev->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		_dev->CreateRenderTargetView(_backBuffers[idx].Get()
			, nullptr, handle);

		if (_use11On12)
		{
			// 11On12リソースを作成
			D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET };
			hr = _d3d11On12Device->CreateWrappedResource(
				_backBuffers[idx].Get(),
				&d3d11Flags,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PRESENT,
				IID_PPV_ARGS(&_wrappedBackBuffers[idx])
			);

			// D2Dのレンダー ターゲットを作成してこのバックバッファーに直接描画
			ComPtr<IDXGISurface> surface;
			hr = _wrappedBackBuffers[idx].As(&surface);
			hr = _d2dDeviceContext->CreateBitmapFromDxgiSurface(
				surface.Get(),
				&bitmapProperties,
				&_d2dRenderTargets[idx]
			);
		}
	}
	if (_use11On12)
	{
		// コマンドアロケーターとリストの生成
		hr = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_commandAllocators));
	}

	return hr;
}
// fontなどのアセットをロードする
void DX12::LoadAssets()
{
	// コマンドリストの作成
	_dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocators.Get(), nullptr, IID_PPV_ARGS(&_cmdList));

	// テキストをレンダリングするための D2D/DWrite オブジェクトを作成
	{
		// C:\Windows\Fontsにあるフォントを読み込む
		_dWriteFactory->CreateTextFormat(
			L"装甲明朝",
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			_baseFontSize,
			L"ja-JP",
			&_textFormat
		);
		_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		
		_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

		// C:\Windows\Fontsにあるフォントを読み込む
		_dWriteFactory->CreateTextFormat(
			L"装甲明朝",
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			_baseFontMiniSize,
			L"ja-JP",
			&_miniTextFormat
		);
		_miniTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

		_miniTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	}

	// コマンドリストの実行
	_cmdList->Close();
	ID3D12CommandList* ppCommandLists[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

}

// フェンスの生成
HRESULT DX12::CreateFence()
{
	HRESULT hr = S_OK;

	// フェンスを作成
	hr = _dev->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_fence.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	return hr;
}

// 全体の描画準備
void DX12::BeginDraw()
{
	// コマンドアロケーターをリセット
	_cmdAllocator->Reset();
	// 再びコマンドリストをためる準備[クローズ状態の解除]
	_cmdList->Reset(_cmdAllocator.Get(), nullptr);

	// ビューポートの指定
	D3D12_VIEWPORT viewport = {};
	viewport.Width = (float)g_window_width;	// 出力先の幅(ピクセル数)
	viewport.Height = (float)g_window_height;// 出力先の高さ(ピクセル数)
	viewport.TopLeftX = 0;			// 出力先の左上座標X
	viewport.TopLeftY = 0;			// 出力先の左上座標Y
	viewport.MaxDepth = 1.0f;		// 深度最大値
	viewport.MinDepth = 0.0f;		// 深度最小値
	_cmdList->RSSetViewports(1, &viewport);

	// シザー矩形の指定
	D3D12_RECT scissorRect = {};
	scissorRect.top = 0;	// 切り抜き上座標
	scissorRect.left = 0;	// 切り抜き左座標
	scissorRect.right = scissorRect.left + g_window_width;    // 切り抜き右座標
	scissorRect.bottom = scissorRect.top + g_window_height;   // 切り抜き下座標
	_cmdList->RSSetScissorRects(1, &scissorRect);

	// レンダーターゲットビューのインデックスを取得[現在のバックバッファーインデックス]
	auto bbIdx = _swapchain->GetCurrentBackBufferIndex();
	// デスクリプタヒープハンドルの先頭アドレスを取得
	auto rtvH = _rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	// デスクリプタヒープのサイズ分アドレスを移動させる	
	rtvH.ptr += static_cast<UINT64>(bbIdx) * _dev->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// これから利用するレンダーターゲットビューとしてセット
	auto dsvH = _dsvHeaps->GetCPUDescriptorHandleForHeapStart();
	// 深度バッファーのクリア
	_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	_cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);
	
	// リソースバリアーの指定
	D3D12_RESOURCE_BARRIER BarrierDesc = {};
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	// 遷移
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;		// 特に指定なし
	BarrierDesc.Transition.pResource = _backBuffers[bbIdx].Get();	// バックバッファーリソース
	BarrierDesc.Transition.Subresource = 0;
	BarrierDesc.Transition.StateBefore
		= D3D12_RESOURCE_STATE_PRESENT;			// 直前はPRESENT状態
	BarrierDesc.Transition.StateAfter
		= D3D12_RESOURCE_STATE_RENDER_TARGET;	// 今からレンダーターゲット状態
	// リソースバリアーの実行
	_cmdList->ResourceBarrier(1, &BarrierDesc);

	// 画面クリア
	float clearColor[] = { 0.5f, 0.8f, 0.8f, 1.0f };	// 灰色
	_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

	if (_use11On12)
	{
		// レンダー ターゲットリソースを取得
		_d3d11On12Device->AcquireWrappedResources(_wrappedBackBuffers[bbIdx].GetAddressOf(), 1);

		// バックバッファに直接レンダリング
		_d2dDeviceContext->SetTarget(_d2dRenderTargets[bbIdx].Get());
		// D2D描画開始
		_d2dDeviceContext->BeginDraw();
		_d2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
	}
}

// 全体の描画終了
void DX12::EndDraw()
{
	// レンダーターゲットビューのインデックスを取得[現在のバックバッファーインデックス]
	auto bbIdx = _swapchain->GetCurrentBackBufferIndex();
	if (!_use11On12)
	{
		// リソースバリアーの指定
		D3D12_RESOURCE_BARRIER BarrierDesc = {};
		BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	// 遷移
		BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;		// 特に指定なし
		BarrierDesc.Transition.pResource = _backBuffers[bbIdx].Get();	// バックバッファーリソース
		BarrierDesc.Transition.Subresource = 0;
		BarrierDesc.Transition.StateBefore
			= D3D12_RESOURCE_STATE_RENDER_TARGET;	// 直前はレンダーターゲット状態
		BarrierDesc.Transition.StateAfter
			= D3D12_RESOURCE_STATE_PRESENT;			// 今からPRESENT状態
		// リソースバリアーの実行
		_cmdList->ResourceBarrier(1, &BarrierDesc);
	}
	// 命令のクローズ
	_cmdList->Close();

	// コマンドリストの実行
	ID3D12CommandList* cmdLists[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(1, cmdLists);

	if (_use11On12)
	{
		// D2D描画終了
		_d2dDeviceContext->EndDraw();

		// ラップされたレンダーターゲットリソースを解放
		_d3d11On12Device->ReleaseWrappedResources(_wrappedBackBuffers[bbIdx].GetAddressOf(), 1);

		// コマンドキューにサブミット
		_d3d11DeviceContext->Flush();
	}
	// フリップ
	_swapchain->Present(1, 0);

	// GPUが完了するまで待機
	WaitForPreviousFrame();
}

// デプスステンシルビューの生成
HRESULT DX12::CreateDepthStencilView()
{
	HRESULT hr = S_OK;

	// 深度バッファーの作成
	// リソースの設定
	D3D12_RESOURCE_DESC depthResDesc = {};
	// 利用方法[2次元のテクスチャーデータを指定]
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	// サイズ幅[幅と高さはレンダーターゲットと同じ]
	depthResDesc.Width = g_window_width;
	// サイズ高さ[同上]
	depthResDesc.Height = g_window_height;
	// 深度や配列サイズ[テクスチャー配列でも、3Dテクスチャーでもない]
	depthResDesc.DepthOrArraySize = 1;
	// フォーマット[深度値書き込み用フォーマット]
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;
	// アンチエイリアシングのパラメーター[アンチエイリアシングを行わないためSampleDesc.Count = 1]
	depthResDesc.SampleDesc.Count = 1;
	// フラグ[デブスステンシルとして使用]
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// ヒーププロパティの設定
	D3D12_HEAP_PROPERTIES depthHeapProp = {};
	// ヒープの種類[DEFAULTを選択]
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	// CPUのページング設定[考えなくてもよい]
	depthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	// メモリープールがどこかを示す[CUSTOM以外のときはこれでよい]
	depthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	// このクリアバリューが重要な意味を持つ
	D3D12_CLEAR_VALUE depthClearValue = {};
	depthClearValue.DepthStencil.Depth = 1.0f;	// 深さ1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;	// 32ビットfloat値としてクリア

	// リソースオブジェクトを作成
	hr = _dev->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	// 深度値書き込みに使用
		&depthClearValue,
		IID_PPV_ARGS(_depthBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// デスクリプタヒープを作成
	// ヒープの記述を用意
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	// デスクリプタヒープの数[深度ビューは1つ]
	dsvHeapDesc.NumDescriptors = 1;
	// 種類[デプスステンシルビュー用]
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	// 記述子ヒープを作成
	hr = _dev->CreateDescriptorHeap(&dsvHeapDesc
		, IID_PPV_ARGS(_dsvHeaps.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// 深度ステンシルビューを作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;	// 深度値に32ビット使用
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;	// ２Dテクスチャー
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;	// フラグは特になし

	_dev->CreateDepthStencilView(
		_depthBuffer.Get(),
		&dsvDesc,
		_dsvHeaps->GetCPUDescriptorHandleForHeapStart());

	return hr;
}

// GPUが完了するまで待機
void DX12::WaitForPreviousFrame()
{
	// GPUの処理が完了した後になるべき値(フェンス値)
	_cmdQueue->Signal(_fence.Get(), ++_fenceVal);

	if (_fence->GetCompletedValue() != _fenceVal)
	{
		// イベントの発生
		_fence->SetEventOnCompletion(_fenceVal, _fenceEvent);
		// イベントが発生するまで待ち続ける(INFINITE)
		WaitForSingleObject(_fenceEvent, INFINITE);
	}
}

// textLayout削除
void DX12::TextLayoutDeleate(int& entryPos)
{
	if (_use11On12)
	{
		if (entryPos != -1)
		{
			_textData[entryPos]._brush.Reset();
			_textData[entryPos]._textLayout.Reset();
			_textData[entryPos]._pos = {};
			_textData[entryPos]._fontSize = 0;
			_textDataCheck[entryPos] = false;
			entryPos = -1;
		}
	}
}

// textLayout作成
// entryPos:作成するvector番号を指定、Pos:クライアント座標位置、Color:色、pFotmat:描画文字、maxWidth:最大幅、maxHeight:最大高さ,
// underLine:下線の有無(true:有,false:無),fontWeight:文字の太さ,miniText:小さめのTextFormatの有無(true:有,false:無)
void DX12::SetTextLayout(int& entryPos, VECTOR2 Pos, VECTOR Color, const TCHAR* pFormat,
	float maxWidth, float maxHeight, bool underLine, DWRITE_FONT_WEIGHT fontWeight, bool miniText)
{
	// 半角文字などは_baseFontSizeのより小さいので-10しておく
	if (_use11On12)
	{
		TextData data = {};
		D2D1_POINT_2F p = D2D1::Point2<FLOAT>(Pos.x, Pos.y);
		data._pos = p;
		ComPtr<ID2D1SolidColorBrush> _textBrush;
		_d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(Color.x, Color.y, Color.z), &_textBrush);
		data._brush = _textBrush;

		std::wstring wst = pFormat;

		if (!miniText)
		{
			_dWriteFactory->CreateTextLayout(
				wst.c_str(),
				(UINT32)wst.size(),
				_textFormat.Get(),
				maxWidth,
				maxHeight,
				&data._textLayout
			);
		}
		else
		{
			_dWriteFactory->CreateTextLayout(
				wst.c_str(),
				(UINT32)wst.size(),
				_miniTextFormat.Get(),
				maxWidth,
				maxHeight,
				&data._textLayout
			);
		}
		// 文字サイズ取得
		DWRITE_TEXT_METRICS textMetrics;
		data._textLayout->GetMetrics(&textMetrics);

		float width = textMetrics.width;
		float height = textMetrics.height;

		DWRITE_TEXT_RANGE textRange = { 0,(UINT32)wst.size() };

		int i = 0;
		while(height > maxHeight)
		{
			i++;
			if (!miniText)
			{
				_dWriteFactory->CreateTextLayout(
					wst.c_str(),
					(UINT32)(wst.size() - i),
					_textFormat.Get(),
					maxWidth,
					maxHeight,
					&data._textLayout
				);
			}
			else
			{
				_dWriteFactory->CreateTextLayout(
					wst.c_str(),
					(UINT32)(wst.size() - i),
					_miniTextFormat.Get(),
					maxWidth,
					maxHeight,
					&data._textLayout
				);
			}

			data._textLayout->GetMetrics(&textMetrics);
			height = textMetrics.height;
		}

		textRange.length = (UINT32)(wst.size() - i);

		data._textLayout->SetUnderline(underLine, textRange);

		data._textLayout->SetFontWeight(fontWeight, textRange);

		if (!miniText)
		{
			data._fontSize = _baseFontSize;
		}
		else
		{
			data._fontSize = _baseFontMiniSize;
		}
		if (entryPos == -1)
		{
			bool Check = false;
			for (int i = 0; i < _textDataCheck.size(); i++)
			{
				if (_textDataCheck[i] == false)
				{
					_textData[i] = data;
					_textDataCheck[i] = true;
					entryPos = i;
					Check = true;
					break;
				}
			}
			if (!Check)
			{
				_textData.push_back(data);
				_textDataCheck.push_back(true);
				entryPos = (int)(_textData.size() - 1);
			}
		}
		else
		{
			_textData[entryPos] = data;
		}
	}
}

// 文字の描画
void DX12::DrawTextFormat()
{
	if (_use11On12)
	{
		for (int i = 0, size = (int)_textData.size(); i < size; ++i)
		{
			if (_textDataCheck[i] == true)
			{
				// 半角文字などはfontSizeのより小さいので-10しておく
				if (_textData[i]._textLayout->GetMaxWidth() >= _textData[i]._fontSize - 10 && _textData[i]._textLayout->GetMaxHeight() >= _textData[i]._fontSize - 10)
				{
					_d2dDeviceContext->DrawTextLayout(_textData[i]._pos, _textData[i]._textLayout.Get(), _textData[i]._brush.Get());
				}
			}
		}
	}
}
