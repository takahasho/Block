#include "BaseClass.h"
#include "../Camera/Camera.h"
#include "../DX12System.h"
// ------------------------ Object���N���X(cpp) ----------------------------
// ���F�e�N�X�`���[�̐���(texture���Ȃ��}�e���A���p)
HRESULT BaseClass::CreateWhiteTexture()
{
	HRESULT hr = S_OK;

	// �q�[�v�v���p�e�B�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp = {};
	// �q�[�v�̎��[����Ȑݒ�Ȃ̂�DEFAULT�ł�UPLOAD�ł��Ȃ�]
	heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	// CPU�̃y�[�W���O�ݒ�[���C�g�o�b�N]
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	// �������[�v�[�����ǂ���������[�]����L0�A�܂�CPU�����璼�ڍs��]
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// �P��A�_�v�^�[�̂���0
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	// ���ԃo�b�t�@�[�Ƃ��ẴA�b�v���[�h�q�[�v�̐ݒ�
	D3D12_HEAP_PROPERTIES uploadHeapProp = {};
	// �}�b�v�\�ɂ��邽�߁AUPLOAD�ɂ���
	uploadHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// �A�b�v���[�h�p�Ɏg�p���邱�ƑO��Ȃ̂�UNKNOWN�ł悢
	uploadHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	uploadHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	uploadHeapProp.CreationNodeMask = 0;	// �P��A�_�v�^�[�̂���0
	uploadHeapProp.VisibleNodeMask = 0;	// �P��A�_�v�^�[�̂���0


	// ���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC resDesc = {};
	// �t�H�[�}�b�g[�P�Ȃ�f�[�^�̉�Ȃ̂�UNKNOWN]
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	// ���p���@[�P�Ȃ�o�b�t�@�[�Ƃ��Ďw��]
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	// �T�C�Y��[�f�[�^�T�C�Y]
	resDesc.Width = dataSize;
	// �T�C�Y����
	resDesc.Height = 1;
	// �[�x��z��T�C�Y[2D�z��ł��Ȃ��̂�]
	resDesc.DepthOrArraySize = 1;
	// �~�b�v�}�b�v���x��[�~�b�v�}�b�v���Ȃ��̂Ń~�b�v���͂P��]
	resDesc.MipLevels = 1;
	// ���C�A�E�g[�A�������f�[�^]
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// ���Ƀt���O�Ȃ�
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	// �A���`�G�C���A�V���O�̃p�����[�^�[[�ʏ�e�N�X�`���[�Ȃ̂ŃA���`�G�C���A�V���O���Ȃ�]
	resDesc.SampleDesc.Count = 1;
	// �N�I���e�B[�N�I���e�B�͍Œ�]
	resDesc.SampleDesc.Quality = 0;

	// ���ԃo�b�t�@�[���쐬
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,	// CPU����̏������݉\�AGPU����͓ǂݎ��̂�
		nullptr,
		IID_PPV_ARGS(_uploadBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �e�N�X�`���[�̂��߂̃q�[�v�̐ݒ�
	D3D12_HEAP_PROPERTIES texHeapProp = {};
	texHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;		// �e�N�X�`���[�p
	// �A�b�v���[�h�p�Ɏg�p���邱�ƑO��Ȃ̂�UNKNOWN�ł悢
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	texHeapProp.CreationNodeMask = 0;	// �P��A�_�v�^�[�̂���0
	texHeapProp.VisibleNodeMask = 0;	// �P��A�_�v�^�[�̂���0

	// �t�H�[�}�b�g[RGBA]
	resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// �T�C�Y��
	resDesc.Width = (UINT64)(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
	// �T�C�Y����
	resDesc.Height = (UINT64)(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
	// �[�x��z��T�C�Y[2D�z��ł��Ȃ��̂�]
	resDesc.DepthOrArraySize = 1;
	// �~�b�v�}�b�v���x��[�~�b�v�}�b�v���Ȃ��̂Ń~�b�v���͂P��]
	resDesc.MipLevels = 1;
	// ���p���@[2D�e�N�X�`���[�p]
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	// ���C�A�E�g[���C�A�E�g�͌��肵�Ȃ�]
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	// �A���`�G�C���A�V���O�̃p�����[�^�[[�ʏ�e�N�X�`���[�Ȃ̂ŃA���`�G�C���A�V���O���Ȃ�]
	resDesc.SampleDesc.Count = 1;
	// �N�I���e�B[�N�I���e�B�͍Œ�]
	resDesc.SampleDesc.Quality = 0;
	// ���Ƀt���O�Ȃ�
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// ���\�[�X�o�b�t�@�[���쐬
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,	// �R�s�[��
		nullptr,
		IID_PPV_ARGS(_texWhiteBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	std::vector<unsigned char> data(dataSize);
	std::fill(data.begin(), data.end(), 0xff);

	// �A�b�v���[�h���\�[�X�ւ̃}�b�v
	uint8_t* mapForImg = nullptr;	// image->pixels�Ɠ����^�ɂ���
	hr = _uploadBuffer->Map(0, nullptr, (void**)&mapForImg);	// �}�b�v
	if (FAILED(hr))
		return hr;

	std::copy_n(&data[0], dataSize, mapForImg);
	_uploadBuffer->Unmap(0, nullptr);	// �A���}�b�v

	// �R�s�[�e�N�X�`���[���M�I���̐ݒ�
	// �R�s�[��(�A�b�v���[�h��)�ݒ�
	D3D12_TEXTURE_COPY_LOCATION src = {};
	src.pResource = _uploadBuffer.Get();	// ���ԃo�b�t�@�[
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;	// �v�b�g�v�����g�w��[�o�b�t�@�[���\�[�X]
	src.PlacedFootprint.Offset = 0;							// �I�t�Z�b�g
	src.PlacedFootprint.Footprint.Width = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;		// ��
	src.PlacedFootprint.Footprint.Height = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;	// ����
	src.PlacedFootprint.Footprint.Depth = 1;				// �[��
	src.PlacedFootprint.Footprint.RowPitch = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT * bytePerPixel;// 1�s������̃o�C�g��
	src.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// �t�H�[�}�b�g

	// �R�s�[��ݒ�
	D3D12_TEXTURE_COPY_LOCATION dst = {};
	dst.pResource = _texWhiteBuffer.Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;	// �C���f�b�N�X[�e�N�X�`���[���\�[�X]
	dst.SubresourceIndex = 0;								// �C���f�b�N�X�ԍ�

	// �R�}���h�A���P�[�^�[�����Z�b�g
	//DX12::GetInstance()->CommandAllocator()->Reset();
	// �ĂуR�}���h���X�g�����߂鏀��[�N���[�Y��Ԃ̉���]
	DX12::GetInstance()->CommandList()->Reset(DX12::GetInstance()->CommandAllocator().Get(), nullptr);

	// �R�}���h���X�g�ɃR�s�[�e�N�X�`���[��ǉ�
	DX12::GetInstance()->CommandList()->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

	// ���\�[�X�o���A�[�̎w��
	D3D12_RESOURCE_BARRIER BarrierDesc = {};
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	// �J��
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;		// ���Ɏw��Ȃ�
	BarrierDesc.Transition.pResource = _texWhiteBuffer.Get();				// �e�N�X�`���[���\�[�X
	BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	BarrierDesc.Transition.StateBefore
		= D3D12_RESOURCE_STATE_COPY_DEST;						// �R�s�[��I��
	BarrierDesc.Transition.StateAfter
		= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;			// �s�N�Z���V�F�[�_�[���\�[�X��I��
	// ���\�[�X�o���A�[�̎��s
	DX12::GetInstance()->CommandList()->ResourceBarrier(1, &BarrierDesc);

	// ���߂̃N���[�Y
	DX12::GetInstance()->CommandList()->Close();

	// �R�}���h���X�g�̎��s
	ID3D12CommandList* cmdLists[] = { DX12::GetInstance()->CommandList().Get() };
	DX12::GetInstance()->CommandQueue()->ExecuteCommandLists(1, cmdLists);

	DX12::GetInstance()->WaitForPreviousFrame();

	return S_OK;
}
// ���F�e�N�X�`���[�̐���(texture���Ȃ��}�e���A���p)
HRESULT BaseClass::CreateBlackTexture()
{
	HRESULT hr = S_OK;

	// �q�[�v�v���p�e�B�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp = {};
	// �q�[�v�̎��[����Ȑݒ�Ȃ̂�DEFAULT�ł�UPLOAD�ł��Ȃ�]
	heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	// CPU�̃y�[�W���O�ݒ�[���C�g�o�b�N]
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	// �������[�v�[�����ǂ���������[�]����L0�A�܂�CPU�����璼�ڍs��]
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// �P��A�_�v�^�[�̂���0
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	// ���ԃo�b�t�@�[�Ƃ��ẴA�b�v���[�h�q�[�v�̐ݒ�
	D3D12_HEAP_PROPERTIES uploadHeapProp = {};
	// �}�b�v�\�ɂ��邽�߁AUPLOAD�ɂ���
	uploadHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// �A�b�v���[�h�p�Ɏg�p���邱�ƑO��Ȃ̂�UNKNOWN�ł悢
	uploadHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	uploadHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	uploadHeapProp.CreationNodeMask = 0;	// �P��A�_�v�^�[�̂���0
	uploadHeapProp.VisibleNodeMask = 0;	// �P��A�_�v�^�[�̂���0

	// 1�s�N�Z��������̃o�C�g��
	UINT bytePerPixel = 4;

	// ���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC resDesc = {};
	// �t�H�[�}�b�g[�P�Ȃ�f�[�^�̉�Ȃ̂�UNKNOWN]
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	// ���p���@[�P�Ȃ�o�b�t�@�[�Ƃ��Ďw��]
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	// �T�C�Y��[�f�[�^�T�C�Y]
	resDesc.Width = dataSize;
	// �T�C�Y����
	resDesc.Height = 1;
	// �[�x��z��T�C�Y[2D�z��ł��Ȃ��̂�]
	resDesc.DepthOrArraySize = 1;
	// �~�b�v�}�b�v���x��[�~�b�v�}�b�v���Ȃ��̂Ń~�b�v���͂P��]
	resDesc.MipLevels = 1;
	// ���C�A�E�g[�A�������f�[�^]
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// ���Ƀt���O�Ȃ�
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	// �A���`�G�C���A�V���O�̃p�����[�^�[[�ʏ�e�N�X�`���[�Ȃ̂ŃA���`�G�C���A�V���O���Ȃ�]
	resDesc.SampleDesc.Count = 1;
	// �N�I���e�B[�N�I���e�B�͍Œ�]
	resDesc.SampleDesc.Quality = 0;

	// ���ԃo�b�t�@�[���쐬
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,	// CPU����̏������݉\�AGPU����͓ǂݎ��̂�
		nullptr,
		IID_PPV_ARGS(_uploadBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �e�N�X�`���[�̂��߂̃q�[�v�̐ݒ�
	D3D12_HEAP_PROPERTIES texHeapProp = {};
	texHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;		// �e�N�X�`���[�p
	// �A�b�v���[�h�p�Ɏg�p���邱�ƑO��Ȃ̂�UNKNOWN�ł悢
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	texHeapProp.CreationNodeMask = 0;	// �P��A�_�v�^�[�̂���0
	texHeapProp.VisibleNodeMask = 0;	// �P��A�_�v�^�[�̂���0

	// �t�H�[�}�b�g[RGBA]
	resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// �T�C�Y��
	resDesc.Width = (UINT64)(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
	// �T�C�Y����
	resDesc.Height = (UINT64)(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
	// �[�x��z��T�C�Y[2D�z��ł��Ȃ��̂�]
	resDesc.DepthOrArraySize = 1;
	// �~�b�v�}�b�v���x��[�~�b�v�}�b�v���Ȃ��̂Ń~�b�v���͂P��]
	resDesc.MipLevels = 1;
	// ���p���@[2D�e�N�X�`���[�p]
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	// ���C�A�E�g[���C�A�E�g�͌��肵�Ȃ�]
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	// �A���`�G�C���A�V���O�̃p�����[�^�[[�ʏ�e�N�X�`���[�Ȃ̂ŃA���`�G�C���A�V���O���Ȃ�]
	resDesc.SampleDesc.Count = 1;
	// �N�I���e�B[�N�I���e�B�͍Œ�]
	resDesc.SampleDesc.Quality = 0;
	// ���Ƀt���O�Ȃ�
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// ���\�[�X�o�b�t�@�[���쐬
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,	// �R�s�[��
		nullptr,
		IID_PPV_ARGS(_texBlackBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	std::vector<unsigned char> data(dataSize);
	std::fill(data.begin(), data.end(), 0x00);

	// �A�b�v���[�h���\�[�X�ւ̃}�b�v
	uint8_t* mapForImg = nullptr;	// image->pixels�Ɠ����^�ɂ���
	hr = _uploadBuffer->Map(0, nullptr, (void**)&mapForImg);	// �}�b�v
	if (FAILED(hr))
		return hr;

	std::copy_n(&data[0], D3D12_TEXTURE_DATA_PITCH_ALIGNMENT * D3D12_TEXTURE_DATA_PITCH_ALIGNMENT * bytePerPixel, mapForImg);
	_uploadBuffer->Unmap(0, nullptr);	// �A���}�b�v

	// �R�s�[�e�N�X�`���[���M�I���̐ݒ�
	// �R�s�[��(�A�b�v���[�h��)�ݒ�
	D3D12_TEXTURE_COPY_LOCATION src = {};
	src.pResource = _uploadBuffer.Get();	// ���ԃo�b�t�@�[
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;	// �v�b�g�v�����g�w��[�o�b�t�@�[���\�[�X]
	src.PlacedFootprint.Offset = 0;							// �I�t�Z�b�g
	src.PlacedFootprint.Footprint.Width = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;		// ��
	src.PlacedFootprint.Footprint.Height = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;	// ����
	src.PlacedFootprint.Footprint.Depth = 1;				// �[��
	src.PlacedFootprint.Footprint.RowPitch = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT * bytePerPixel;// 1�s������̃o�C�g��
	src.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// �t�H�[�}�b�g

	// �R�s�[��ݒ�
	D3D12_TEXTURE_COPY_LOCATION dst = {};
	dst.pResource = _texBlackBuffer.Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;	// �C���f�b�N�X[�e�N�X�`���[���\�[�X]
	dst.SubresourceIndex = 0;								// �C���f�b�N�X�ԍ�

	// �R�}���h�A���P�[�^�[�����Z�b�g
	DX12::GetInstance()->CommandAllocator()->Reset();
	// �ĂуR�}���h���X�g�����߂鏀��[�N���[�Y��Ԃ̉���]
	DX12::GetInstance()->CommandList()->Reset(DX12::GetInstance()->CommandAllocator().Get(), nullptr);

	// �R�}���h���X�g�ɃR�s�[�e�N�X�`���[��ǉ�
	DX12::GetInstance()->CommandList()->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

	// ���\�[�X�o���A�[�̎w��
	D3D12_RESOURCE_BARRIER BarrierDesc = {};
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	// �J��
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;		// ���Ɏw��Ȃ�
	BarrierDesc.Transition.pResource = _texBlackBuffer.Get();				// �e�N�X�`���[���\�[�X
	BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	BarrierDesc.Transition.StateBefore
		= D3D12_RESOURCE_STATE_COPY_DEST;						// �R�s�[��I��
	BarrierDesc.Transition.StateAfter
		= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;			// �s�N�Z���V�F�[�_�[���\�[�X��I��
	// ���\�[�X�o���A�[�̎��s
	DX12::GetInstance()->CommandList()->ResourceBarrier(1, &BarrierDesc);

	// ���߂̃N���[�Y
	DX12::GetInstance()->CommandList()->Close();

	// �R�}���h���X�g�̎��s
	ID3D12CommandList* cmdLists[] = { DX12::GetInstance()->CommandList().Get() };
	DX12::GetInstance()->CommandQueue()->ExecuteCommandLists(1, cmdLists);

	DX12::GetInstance()->WaitForPreviousFrame();

	return S_OK;
}

// �e�N�X�`���ǂݍ���(char*)
HRESULT BaseClass::LoadTextureFromFile(const char* fileName,
	BYTE** pImageData, SIZE* pSize)
{
	TCHAR pFileName[256] = {};
#ifdef UNICODE
	MultiByteToWideChar(CP_OEMCP, MB_PRECOMPOSED, fileName, (int)strlen(fileName), pFileName, (sizeof pFileName) / 2);
#else
	strcpy(pFileName, fileName);
#endif
	// �摜�̓ǂݍ���
	if (!FAILED(LoadTextureFromFile(pFileName, pImageData, pSize)))
		return S_OK;
	return E_FAIL;
}
// �摜�̓ǂݍ���(�{��)(TCHAR*)
HRESULT  BaseClass::LoadTextureFromFile(const TCHAR* pFileName, BYTE** pImageData, SIZE* pSize)
{
	HRESULT hr = S_OK;

	// �C���[�W���O�t�@�N�g���[����
	CComPtr<IWICImagingFactory> pImagingFactory;
	if (FAILED(CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		reinterpret_cast<void**>(&pImagingFactory))))
	{
		OutputDebugString(_T("ImagingFactory�����Ɏ��s\n"));
		return E_FAIL;
	}

	// �f�R�[�_�[����
	CComPtr<IWICBitmapDecoder> pDecoder;
	hr = pImagingFactory->CreateDecoderFromFilename(
		pFileName,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad, &pDecoder);
	if (FAILED(hr))
	{
		pImagingFactory.Release();
		OutputDebugString(_T("Decoder�����Ɏ��s\n"));
		return E_FAIL;
	}


	// �t���[���擾 
	//IWICBitmapFrameDecode* pFrame = nullptr;
	CComPtr<IWICBitmapFrameDecode> pFrame;
	hr = pDecoder->GetFrame(0, &pFrame);
	if (FAILED(hr))
	{
		pImagingFactory.Release();
		pDecoder.Release();
		OutputDebugString(_T("Frame�擾�Ɏ��s\n"));
		return E_FAIL;
	}

	// �R���o�[�^�[�ɂ��t�H�[�}�b�g�ϊ� 
	//IWICFormatConverter* pConverter = nullptr;
	CComPtr<IWICFormatConverter> pConverter;
	hr = pImagingFactory->CreateFormatConverter(&pConverter);
	if (FAILED(hr))
	{
		pImagingFactory.Release();
		pDecoder.Release();
		pFrame.Release();
		OutputDebugString(_T("Converter�����Ɏ��s\n"));
		return E_FAIL;
	}

	// �t�H�[�}�b�g���̎擾
	WICPixelFormatGUID PixelFormat;
	pFrame->GetPixelFormat(&PixelFormat);

	// �R���o�[�^�[�̏�����(32Bit�t�H�[�}�b�g)
	if (FAILED(pConverter->Initialize(
		pFrame,
		GUID_WICPixelFormat32bppRGBA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.f,
		WICBitmapPaletteTypeMedianCut)))
	{
		pImagingFactory.Release();
		pDecoder.Release();
		pFrame.Release();
		pConverter.Release();
		OutputDebugString(_T("Converter�����Ɏ��s\n"));
		return E_FAIL;
	}

	// �摜�̕��A�������w��
	UINT ImageWidth, ImageHeight;
	pFrame->GetSize(&ImageWidth, &ImageHeight);
	pSize->cx = ImageWidth;
	pSize->cy = ImageHeight;

	// 1�s�N�Z��������̃o�C�g��
	UINT BytePerPixel = 4;
	// �f�[�^�T�C�Y�̌���(RGBA)
	UINT ImageSize = ImageWidth * ImageHeight * BytePerPixel;
	// �ۑ��̈�쐬
	*pImageData = new uint8_t[ImageSize];

	WICRect Rect = { 0, 0, static_cast<int>(ImageWidth), static_cast<int>(ImageHeight) };
	UINT Size = ImageWidth * ImageHeight;
	pConverter->CopyPixels(&Rect, ImageWidth * BytePerPixel, Size * BytePerPixel, *pImageData);


	if (PixelFormat == GUID_WICPixelFormat24bppBGR)
	{
		// �����F���w��(���F)
		// R:0, G:0, B:0
		UINT Color = 0x00000000;
		for (UINT i = 0; i < ImageSize; i += BytePerPixel)
		{
			(*pImageData)[i + 3] = 0xFF;
			if (((*pImageData)[i + 0] | (*pImageData)[i + 1] | (*pImageData)[i + 2]) == Color)
			{
				(*pImageData)[i + 3] = 0x00;
			}
		}
	}

	pImagingFactory.Release();
	pDecoder.Release();
	pFrame.Release();
	pConverter.Release();

	return S_OK;
}
// �A���C�����g�ɂ��낦���T�C�Y��Ԃ�
// @param size ���̃T�C�Y
// @param alignment �A���C�����g�T�C�Y
// @param �A���C�����g���낦���T�C�Y
size_t BaseClass::AlignmentSize(size_t size, size_t alignment)
{
	if (size <= alignment)
		return alignment;
	else
	{
		if (size % alignment == 0)
			return (size / alignment) * alignment;
		else
			return ((size / alignment) + 1) * alignment;
	}

}
// ���ʎq���擾
std::wstring BaseClass::GetWideStringFromString(const std::string& str)
{
	// �Ăяo������(�����񐔂𓾂�)
	auto num1 = MultiByteToWideChar(
		CP_ACP,
		MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		str.c_str(),
		-1,
		nullptr,
		0
	);

	std::wstring wstr;
	wstr.resize(num1);

	// �Ăяo������(�m�ۍς݂�wstr�ɕϊ���������R�s�[)
	auto num2 = MultiByteToWideChar(
		CP_ACP,
		MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		str.c_str(),
		-1,
		&wstr[0],
		num1
	);

	assert(num1 == num2);
	return wstr;
}

// �G���[���b�Z�[�W
HRESULT BaseClass::BlobErrorMessange(HRESULT hr,
	ID3DBlob* errorBlob)
{
	if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
	{
		::OutputDebugStringA("�t�@�C������������܂���\n");
		return hr;
	}
	else
	{
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";

		::OutputDebugStringA(errstr.c_str());
	}

	return E_FAIL;
}