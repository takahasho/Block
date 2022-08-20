// --------------- Include File ---------------
#include "DX12System.h"

DX12* DX12::_instance = nullptr;			// �C���X�^���X�̎���
// �R���X�g���N�^
DX12::DX12()
	:_fenceVal(0), _fenceEvent(nullptr), _use11On12(true), _baseFontSize(40), _baseFontMiniSize(18)
{

}

// �f�X�g���N�^�[
DX12::~DX12()
{
	if (_use11On12)
	{
		_imagingFactory.Reset();
	}
	// COM�I��
	CoUninitialize();
	// GPU����������܂őҋ@
	WaitForPreviousFrame();
	// �C�x���g�n���h�������
	CloseHandle(_fenceEvent);
}
// ����
void DX12::Create()
{
	if (!_instance)
	{
		_instance = new DX12();

		// DX12���쐬
		_instance->CreateDX12();
	}

}

// ����
HRESULT DX12::CreateDX12()
{
	HRESULT hr = S_OK;

	// �t�@�N�g���[���쐬
	hr = CreateFactory();
	if (FAILED(hr))
		return E_FAIL;

	// �f�o�C�X�̐���
	hr = CreateDevice();
	if (FAILED(hr))
		return E_FAIL;

	// �R�}���h�A���P�[�^�[�ƃ��X�g�̐���
	if (FAILED(CreateCommandAllocatorList()))
		return E_FAIL;

	// �R�}���h�L���[�̐���
	if (FAILED(CreateCommandQueue()))
		return E_FAIL;

	// �X���b�v�`�F�[���̐���
	if (FAILED(CreateSwapChain()))
		return E_FAIL;

	if (_use11On12)
	{
		// Direct3D11On12Device�ȂǍ쐬
		if (FAILED(CreateD3D11On12()))
			return E_FAIL;
	}

	// �����_�[�^�[�Q�b�g�r���[�̐���
	if (FAILED(CreateRenderTargetView()))
		return E_FAIL;

	if (_use11On12)
	{
		LoadAssets();	// font�Ȃǂ̓ǂݍ���
	}

	// �t�F���X�̐���
	if (FAILED(CreateFence()))
		return E_FAIL;

	// �f�v�X�X�e���V���r���[�̐���
	if (FAILED(CreateDepthStencilView()))
		return E_FAIL;

	return hr;
}

// �j��
void DX12::Destroy()
{
	delete _instance;
}
// �f�o�C�X�̐���
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

	// Direct3D�f�o�C�X�̏�����
	D3D_FEATURE_LEVEL featureLevel = {};

	// �O���t�B�b�N�{�[�h��������͂Ȃ�
	for (auto lv : levels)
	{
		if (D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(_dev.ReleaseAndGetAddressOf())) == S_OK)
		{
			featureLevel = lv;
			break;		// �����\�ȃo�[�W���������������烋�[�v��ł��؂�
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
// �t�@�N�g���[�̐���
HRESULT DX12::CreateFactory()
{
	HRESULT hr = S_OK;

	// �f�o�b�O���C���[��L��
#if defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugLayer;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(debugLayer.ReleaseAndGetAddressOf()));
		if (FAILED(hr))
			return hr;
		// �f�o�b�N���C���[��L����
		debugLayer->EnableDebugLayer();
		// �L����������C���^�[�t�F�C�X�����
		debugLayer->Release();
	}
#endif

	// �t�@�N�g���[���쐬
#if defined(_DEBUG)
	CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(_dxgiFactory.ReleaseAndGetAddressOf()));
#else
	CreateDXGIFactory1(IID_PPV_ARGS(_dxgiFactory.ReleaseAndGetAddressOf()));
#endif

	return hr;
}

// �R�}���h�A���P�[�^�[�ƃ��X�g�̐���
HRESULT DX12::CreateCommandAllocatorList()
{
	HRESULT hr = S_OK;

	// �R�}���h�A���P�[�^�[���쐬
	hr = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(_cmdAllocator.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �R�}���h�A���P�[�^�[�ƍ��킹�ăR�}���h���X�g���쐬
	hr = _dev->CreateCommandList(0
		, D3D12_COMMAND_LIST_TYPE_DIRECT,
		_cmdAllocator.Get(), nullptr
		, IID_PPV_ARGS(_cmdList.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// ���߂̃N���[�Y
	_cmdList->Close();

	return hr;
}

// �R�}���h�L���[�̐���
HRESULT DX12::CreateCommandQueue()
{
	HRESULT hr = S_OK;

	// �R�}���h�L���[�̋L�q��p��
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	// �^�C���A�E�g�Ȃ�
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	// �A�_�v�^�[���P�����g��Ȃ��Ƃ��͂O�ł悢
	cmdQueueDesc.NodeMask = 0;
	// �v���C�I���e�B�͓��Ɏw��Ȃ�
	cmdQueueDesc.Priority =
		D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	// �R�}���h���X�g�Ƃ��č��킹��
	cmdQueueDesc.Type =
		D3D12_COMMAND_LIST_TYPE_DIRECT;

	// DIRECT �̃L���[�ƃR�}���h ���X�g�͂ǂ̂悤�ȃR�}���h���󂯎��
	// COMPUTE �̃L���[�ƃR�}���h ���X�g�͌v�Z�ƃR�s�[�Ɋ֘A����R�}���h���󂯎��
	// COPY �̃L���[�ƃR�}���h ���X�g�̓R�s�[�̃R�}���h�݂̂��󂯎��܂�

	// �R�}���h�L���[���쐬
	hr = _dev->CreateCommandQueue(
		&cmdQueueDesc, IID_PPV_ARGS(_cmdQueue.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	return hr;
}

// �X���b�v�`�F�[���̐���
HRESULT DX12::CreateSwapChain()
{
	HRESULT hr = S_OK;

	// �X���b�v�`�F�[���̋L�q��p��
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	// �摜�𑜓x[��]
	swapChainDesc.Width = g_window_width;
	// �摜�𑜓x[����]
	swapChainDesc.Height = g_window_height;
	// �s�N�Z���t�H�[�}�b�g[��,��,��,�A���t�@�[��8Bit]
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// �X�e���I�\���t���O[3D�f�B�X�v���C�̃X�e���I���[�h�Ȃ�]
	swapChainDesc.Stereo = FALSE;
	// �}���`�T���v���̎w��[Count = 1, Quality = 0�ł悢]
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	// �o�b�t�@�[�̗��p���@[DXGI_USAGE_BACK_BUFFER�ł悢]
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	// �o�b�t�@�[��[�_�u���o�b�t�@�[�Ȃ�Q�ł悢]
	swapChainDesc.BufferCount = 2;

	// �o�b�N�o�b�t�@�[�͐L�яk�݉\
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	// �t���b�v��͑��₩�ɔj��
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	// ���Ɏw��Ȃ�
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	// �E�B���h�E�̃t���X�N���[���؂�ւ��\
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// �X���b�v�`�F�[�����쐬
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

// Direct3D11On12Device�ȂǍ쐬
HRESULT DX12::CreateD3D11On12()
{
	HRESULT hr = S_OK;
	// DX12�f�o�C�X��DX11�f�o�C�X�����L�Ƀ��b�v���ꂽ���f�o�C�X�쐬
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
	// 11�f�o�C�X����11On12�f�o�C�X�𐮍�
	hr = d3d11Device.As(&_d3d11On12Device);
	// COM�J�n
	if (CoInitialize(NULL) != S_OK)
		return E_FAIL;
	// D2D/DWrite �R���|�[�l���g���쐬
	D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};
	{
		D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2dFactoryOptions, &_d2dFactory);
		ComPtr<IDXGIDevice> dxgiDevice;
		hr = _d3d11On12Device.As(&dxgiDevice);
		hr = _d2dFactory->CreateDevice(dxgiDevice.Get(), &_d2dDevice);
		hr = _d2dDevice->CreateDeviceContext(deviceOptions, &_d2dDeviceContext);
		// �C���[�W�t�@�N�g���[�̍쐬
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, &_imagingFactory);
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &_dWriteFactory);
	}

	return S_OK;
}

// �����_�[�^�[�Q�b�g�r���[�̐���
HRESULT DX12::CreateRenderTargetView()
{
	HRESULT hr = S_OK;

	D2D1_BITMAP_PROPERTIES1 bitmapProperties = {};
	if (_use11On12)
	{
		// D2D�̃����_�[�^�[�Q�b�g�B
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
	// �f�X�N���v�^�q�[�v���쐬
	// �q�[�v�̋L�q��p��
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	// �����_�[�^�[�Q�b�g�r���[�Ȃ̂�RTV
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	// GPU���P�����g�p����z��
	heapDesc.NodeMask = 0;
	// �\���̂Q��
	heapDesc.NumDescriptors = FrameCount;
	// ���Ɏw��Ȃ�[�V�F�[�_�[�����猩����K�v���Ȃ�]
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	// �f�X�N���v�^�q�[�v���쐬
	hr = _dev->CreateDescriptorHeap(&heapDesc
		, IID_PPV_ARGS(_rtvHeaps.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �X���b�v�`�F�[�����̎擾
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	hr = _swapchain->GetDesc(&swcDesc);
	if (FAILED(hr))
		return hr;

	for (int idx = 0; idx < (int)swcDesc.BufferCount; ++idx)
	{
		// �X���b�v�`�F�[���ƃo�b�t�@�[�̊֘A�t��
		hr = _swapchain->GetBuffer(idx
			, IID_PPV_ARGS(_backBuffers[idx].ReleaseAndGetAddressOf()));
		if (FAILED(hr))
			return hr;

		// �����_�[�^�[�Q�b�g�r���[���쐬
		// �f�X�N���v�^�q�[�v�n���h���̐擪�A�h���X���擾
		D3D12_CPU_DESCRIPTOR_HANDLE handle = _rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		// �f�X�N���v�^�q�[�v�̃T�C�Y���A�h���X���ړ�������
		handle.ptr += static_cast<UINT64>(idx) * _dev->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		_dev->CreateRenderTargetView(_backBuffers[idx].Get()
			, nullptr, handle);

		if (_use11On12)
		{
			// 11On12���\�[�X���쐬
			D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET };
			hr = _d3d11On12Device->CreateWrappedResource(
				_backBuffers[idx].Get(),
				&d3d11Flags,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PRESENT,
				IID_PPV_ARGS(&_wrappedBackBuffers[idx])
			);

			// D2D�̃����_�[ �^�[�Q�b�g���쐬���Ă��̃o�b�N�o�b�t�@�[�ɒ��ڕ`��
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
		// �R�}���h�A���P�[�^�[�ƃ��X�g�̐���
		hr = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_commandAllocators));
	}

	return hr;
}
// font�Ȃǂ̃A�Z�b�g�����[�h����
void DX12::LoadAssets()
{
	// �R�}���h���X�g�̍쐬
	_dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocators.Get(), nullptr, IID_PPV_ARGS(&_cmdList));

	// �e�L�X�g�������_�����O���邽�߂� D2D/DWrite �I�u�W�F�N�g���쐬
	{
		// C:\Windows\Fonts�ɂ���t�H���g��ǂݍ���
		_dWriteFactory->CreateTextFormat(
			L"���b����",
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

		// C:\Windows\Fonts�ɂ���t�H���g��ǂݍ���
		_dWriteFactory->CreateTextFormat(
			L"���b����",
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

	// �R�}���h���X�g�̎��s
	_cmdList->Close();
	ID3D12CommandList* ppCommandLists[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

}

// �t�F���X�̐���
HRESULT DX12::CreateFence()
{
	HRESULT hr = S_OK;

	// �t�F���X���쐬
	hr = _dev->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_fence.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	return hr;
}

// �S�̂̕`�揀��
void DX12::BeginDraw()
{
	// �R�}���h�A���P�[�^�[�����Z�b�g
	_cmdAllocator->Reset();
	// �ĂуR�}���h���X�g�����߂鏀��[�N���[�Y��Ԃ̉���]
	_cmdList->Reset(_cmdAllocator.Get(), nullptr);

	// �r���[�|�[�g�̎w��
	D3D12_VIEWPORT viewport = {};
	viewport.Width = (float)g_window_width;	// �o�͐�̕�(�s�N�Z����)
	viewport.Height = (float)g_window_height;// �o�͐�̍���(�s�N�Z����)
	viewport.TopLeftX = 0;			// �o�͐�̍�����WX
	viewport.TopLeftY = 0;			// �o�͐�̍�����WY
	viewport.MaxDepth = 1.0f;		// �[�x�ő�l
	viewport.MinDepth = 0.0f;		// �[�x�ŏ��l
	_cmdList->RSSetViewports(1, &viewport);

	// �V�U�[��`�̎w��
	D3D12_RECT scissorRect = {};
	scissorRect.top = 0;	// �؂蔲������W
	scissorRect.left = 0;	// �؂蔲�������W
	scissorRect.right = scissorRect.left + g_window_width;    // �؂蔲���E���W
	scissorRect.bottom = scissorRect.top + g_window_height;   // �؂蔲�������W
	_cmdList->RSSetScissorRects(1, &scissorRect);

	// �����_�[�^�[�Q�b�g�r���[�̃C���f�b�N�X���擾[���݂̃o�b�N�o�b�t�@�[�C���f�b�N�X]
	auto bbIdx = _swapchain->GetCurrentBackBufferIndex();
	// �f�X�N���v�^�q�[�v�n���h���̐擪�A�h���X���擾
	auto rtvH = _rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	// �f�X�N���v�^�q�[�v�̃T�C�Y���A�h���X���ړ�������	
	rtvH.ptr += static_cast<UINT64>(bbIdx) * _dev->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// ���ꂩ�痘�p���郌���_�[�^�[�Q�b�g�r���[�Ƃ��ăZ�b�g
	auto dsvH = _dsvHeaps->GetCPUDescriptorHandleForHeapStart();
	// �[�x�o�b�t�@�[�̃N���A
	_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	_cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);
	
	// ���\�[�X�o���A�[�̎w��
	D3D12_RESOURCE_BARRIER BarrierDesc = {};
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	// �J��
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;		// ���Ɏw��Ȃ�
	BarrierDesc.Transition.pResource = _backBuffers[bbIdx].Get();	// �o�b�N�o�b�t�@�[���\�[�X
	BarrierDesc.Transition.Subresource = 0;
	BarrierDesc.Transition.StateBefore
		= D3D12_RESOURCE_STATE_PRESENT;			// ���O��PRESENT���
	BarrierDesc.Transition.StateAfter
		= D3D12_RESOURCE_STATE_RENDER_TARGET;	// �����烌���_�[�^�[�Q�b�g���
	// ���\�[�X�o���A�[�̎��s
	_cmdList->ResourceBarrier(1, &BarrierDesc);

	// ��ʃN���A
	float clearColor[] = { 0.5f, 0.8f, 0.8f, 1.0f };	// �D�F
	_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

	if (_use11On12)
	{
		// �����_�[ �^�[�Q�b�g���\�[�X���擾
		_d3d11On12Device->AcquireWrappedResources(_wrappedBackBuffers[bbIdx].GetAddressOf(), 1);

		// �o�b�N�o�b�t�@�ɒ��ڃ����_�����O
		_d2dDeviceContext->SetTarget(_d2dRenderTargets[bbIdx].Get());
		// D2D�`��J�n
		_d2dDeviceContext->BeginDraw();
		_d2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
	}
}

// �S�̂̕`��I��
void DX12::EndDraw()
{
	// �����_�[�^�[�Q�b�g�r���[�̃C���f�b�N�X���擾[���݂̃o�b�N�o�b�t�@�[�C���f�b�N�X]
	auto bbIdx = _swapchain->GetCurrentBackBufferIndex();
	if (!_use11On12)
	{
		// ���\�[�X�o���A�[�̎w��
		D3D12_RESOURCE_BARRIER BarrierDesc = {};
		BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	// �J��
		BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;		// ���Ɏw��Ȃ�
		BarrierDesc.Transition.pResource = _backBuffers[bbIdx].Get();	// �o�b�N�o�b�t�@�[���\�[�X
		BarrierDesc.Transition.Subresource = 0;
		BarrierDesc.Transition.StateBefore
			= D3D12_RESOURCE_STATE_RENDER_TARGET;	// ���O�̓����_�[�^�[�Q�b�g���
		BarrierDesc.Transition.StateAfter
			= D3D12_RESOURCE_STATE_PRESENT;			// ������PRESENT���
		// ���\�[�X�o���A�[�̎��s
		_cmdList->ResourceBarrier(1, &BarrierDesc);
	}
	// ���߂̃N���[�Y
	_cmdList->Close();

	// �R�}���h���X�g�̎��s
	ID3D12CommandList* cmdLists[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(1, cmdLists);

	if (_use11On12)
	{
		// D2D�`��I��
		_d2dDeviceContext->EndDraw();

		// ���b�v���ꂽ�����_�[�^�[�Q�b�g���\�[�X�����
		_d3d11On12Device->ReleaseWrappedResources(_wrappedBackBuffers[bbIdx].GetAddressOf(), 1);

		// �R�}���h�L���[�ɃT�u�~�b�g
		_d3d11DeviceContext->Flush();
	}
	// �t���b�v
	_swapchain->Present(1, 0);

	// GPU����������܂őҋ@
	WaitForPreviousFrame();
}

// �f�v�X�X�e���V���r���[�̐���
HRESULT DX12::CreateDepthStencilView()
{
	HRESULT hr = S_OK;

	// �[�x�o�b�t�@�[�̍쐬
	// ���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC depthResDesc = {};
	// ���p���@[2�����̃e�N�X�`���[�f�[�^���w��]
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	// �T�C�Y��[���ƍ����̓����_�[�^�[�Q�b�g�Ɠ���]
	depthResDesc.Width = g_window_width;
	// �T�C�Y����[����]
	depthResDesc.Height = g_window_height;
	// �[�x��z��T�C�Y[�e�N�X�`���[�z��ł��A3D�e�N�X�`���[�ł��Ȃ�]
	depthResDesc.DepthOrArraySize = 1;
	// �t�H�[�}�b�g[�[�x�l�������ݗp�t�H�[�}�b�g]
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;
	// �A���`�G�C���A�V���O�̃p�����[�^�[[�A���`�G�C���A�V���O���s��Ȃ�����SampleDesc.Count = 1]
	depthResDesc.SampleDesc.Count = 1;
	// �t���O[�f�u�X�X�e���V���Ƃ��Ďg�p]
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// �q�[�v�v���p�e�B�̐ݒ�
	D3D12_HEAP_PROPERTIES depthHeapProp = {};
	// �q�[�v�̎��[DEFAULT��I��]
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	// CPU�̃y�[�W���O�ݒ�[�l���Ȃ��Ă��悢]
	depthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	// �������[�v�[�����ǂ���������[CUSTOM�ȊO�̂Ƃ��͂���ł悢]
	depthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	// ���̃N���A�o�����[���d�v�ȈӖ�������
	D3D12_CLEAR_VALUE depthClearValue = {};
	depthClearValue.DepthStencil.Depth = 1.0f;	// �[��1.0f(�ő�l)�ŃN���A
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;	// 32�r�b�gfloat�l�Ƃ��ăN���A

	// ���\�[�X�I�u�W�F�N�g���쐬
	hr = _dev->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	// �[�x�l�������݂Ɏg�p
		&depthClearValue,
		IID_PPV_ARGS(_depthBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �f�X�N���v�^�q�[�v���쐬
	// �q�[�v�̋L�q��p��
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	// �f�X�N���v�^�q�[�v�̐�[�[�x�r���[��1��]
	dsvHeapDesc.NumDescriptors = 1;
	// ���[�f�v�X�X�e���V���r���[�p]
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	// �L�q�q�q�[�v���쐬
	hr = _dev->CreateDescriptorHeap(&dsvHeapDesc
		, IID_PPV_ARGS(_dsvHeaps.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �[�x�X�e���V���r���[���쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;	// �[�x�l��32�r�b�g�g�p
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;	// �QD�e�N�X�`���[
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;	// �t���O�͓��ɂȂ�

	_dev->CreateDepthStencilView(
		_depthBuffer.Get(),
		&dsvDesc,
		_dsvHeaps->GetCPUDescriptorHandleForHeapStart());

	return hr;
}

// GPU����������܂őҋ@
void DX12::WaitForPreviousFrame()
{
	// GPU�̏���������������ɂȂ�ׂ��l(�t�F���X�l)
	_cmdQueue->Signal(_fence.Get(), ++_fenceVal);

	if (_fence->GetCompletedValue() != _fenceVal)
	{
		// �C�x���g�̔���
		_fence->SetEventOnCompletion(_fenceVal, _fenceEvent);
		// �C�x���g����������܂ő҂�������(INFINITE)
		WaitForSingleObject(_fenceEvent, INFINITE);
	}
}

// textLayout�폜
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

// textLayout�쐬
// entryPos:�쐬����vector�ԍ����w��APos:�N���C�A���g���W�ʒu�AColor:�F�ApFotmat:�`�敶���AmaxWidth:�ő啝�AmaxHeight:�ő卂��,
// underLine:�����̗L��(true:�L,false:��),fontWeight:�����̑���,miniText:�����߂�TextFormat�̗L��(true:�L,false:��)
void DX12::SetTextLayout(int& entryPos, VECTOR2 Pos, VECTOR Color, const TCHAR* pFormat,
	float maxWidth, float maxHeight, bool underLine, DWRITE_FONT_WEIGHT fontWeight, bool miniText)
{
	// ���p�����Ȃǂ�_baseFontSize�̂�菬�����̂�-10���Ă���
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
		// �����T�C�Y�擾
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

// �����̕`��
void DX12::DrawTextFormat()
{
	if (_use11On12)
	{
		for (int i = 0, size = (int)_textData.size(); i < size; ++i)
		{
			if (_textDataCheck[i] == true)
			{
				// ���p�����Ȃǂ�fontSize�̂�菬�����̂�-10���Ă���
				if (_textData[i]._textLayout->GetMaxWidth() >= _textData[i]._fontSize - 10 && _textData[i]._textLayout->GetMaxHeight() >= _textData[i]._fontSize - 10)
				{
					_d2dDeviceContext->DrawTextLayout(_textData[i]._pos, _textData[i]._textLayout.Get(), _textData[i]._brush.Get());
				}
			}
		}
	}
}
