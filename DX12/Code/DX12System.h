#ifndef __DX12SYSTEM_H__
#define __DX12SYSTEM_H__
#pragma once
//--------------------- Define�錾 ------------------
#define DX					DX12::GetInstance()	//DX12Instance
// --------------------- Include File --------------------
#include "Global.h"
#include <stdexcept>
#include "VECTOR2.h"
#include "VECTOR.h"
// ---------------------- �N���X�錾 ---------------------

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
	bool _use11On12;		// false:�g��Ȃ�,true:�g��
	static const UINT FrameCount = 2;
	const float _baseFontSize;
	const float _baseFontMiniSize;
	// ---------------- �����o�ϐ��錾 ----------------
	ComPtr<ID3D12Device> _dev;				// �f�o�C�X
	ComPtr<IDXGIFactory6> _dxgiFactory;		// �t�@�N�g���[
	ComPtr<IDXGISwapChain4> _swapchain;		// �X���b�v�`�F�[��

	ComPtr<ID3D12CommandAllocator> _cmdAllocator;	// �R�}���h�A���P�[�^�[
	ComPtr<ID3D12GraphicsCommandList> _cmdList;		// �R�}���h���X�g

	ComPtr<ID3D12CommandQueue> _cmdQueue;	// �R�}���h�L���[

	ComPtr<ID3D12Resource> _backBuffers[FrameCount];	// �����_�[�^�[�Q�b�g

	ComPtr<ID3D12DescriptorHeap> _rtvHeaps;	// �����_�[�^�[�Q�b�g�r���[�̃f�B�X�N���v�^�[�q�[�v

	ComPtr<ID3D12Fence> _fence;				// �t�F���X
	UINT64 _fenceVal;						// �t�F���X�̃C���f�b�N�X
	HANDLE _fenceEvent;						// �t�F���X�C�x���g

	ComPtr<ID3D12Resource> _depthBuffer;	// �[�x�o�b�t�@�[
	ComPtr<ID3D12DescriptorHeap> _dsvHeaps;	// �[�x�X�e���V���o�b�t�@�[�r���[�q�[�v

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

	// ���\�[�X
	ComPtr<IWICImagingFactory>	_imagingFactory;	// �C���[�W�t�@�N�g���[
	ComPtr<IDWriteTextFormat> _textFormat;
	ComPtr<IDWriteTextFormat> _miniTextFormat;
	std::vector<TextData>	_textData;
	std::vector<bool>		_textDataCheck;
private:
	HRESULT CreateDX12();					// DX12�̐���
	HRESULT CreateDevice();					// �f�o�C�X�̐���
	HRESULT CreateFactory();				// �t�@�N�g���[�̐���
	HRESULT CreateCommandAllocatorList();	// �R�}���h�A���P�[�^�[�ƃ��X�g�̐���
	HRESULT CreateCommandQueue();			// �R�}���h�L���[�̐���
	HRESULT CreateSwapChain();				// �X���b�v�`�F�[���̐���
	HRESULT CreateD3D11On12();				// Direct3D11On12Device�ȂǍ쐬
	HRESULT CreateRenderTargetView();		// �����_�[�^�[�Q�b�g�r���[�̐���
	void LoadAssets();						// font�Ȃǂ̃A�Z�b�g�����[�h����
	HRESULT CreateFence();					// �t�F���X�̐���

	HRESULT CreateDepthStencilView();		// �f�v�X�X�e���V���r���[�̐���
public:
	// ---------------- �����o�ϐ��錾 ----------------
	static DX12* _instance;				// �V���O���g��
	DX12();		// �R���X�g���N�^�[
	~DX12();								// �f�X�g���N�^�[
	static void Create();					// ����
	static void Destroy();					// �j��
	void BeginDraw();						// �S�̂̕`�揀��
	void EndDraw();							// �S�̂̕`��I��
	void WaitForPreviousFrame();			// GPU����������܂őҋ@
	void TextLayoutDeleate(int& entryPos);				// textLayout�폜
	void SetTextLayout(int& entryPos, VECTOR2 Pos, VECTOR Color, const TCHAR* pFormat,
		float maxWidth, float maxHeight, bool underLine = false, DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_NORMAL, bool miniText = false);		// textLayout�쐬
	void DrawTextFormat();	// �����̕`��

	// �Q�b�^�[�Z�b�^�[
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