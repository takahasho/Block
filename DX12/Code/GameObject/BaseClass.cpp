#include "BaseClass.h"
#include "../Camera/Camera.h"
#include "../DX12System.h"
// ------------------------ Object基底クラス(cpp) ----------------------------
// 白色テクスチャーの生成(textureがないマテリアル用)
HRESULT BaseClass::CreateWhiteTexture()
{
	HRESULT hr = S_OK;

	// ヒーププロパティの設定
	D3D12_HEAP_PROPERTIES heapProp = {};
	// ヒープの種類[特殊な設定なのでDEFAULTでもUPLOADでもない]
	heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	// CPUのページング設定[ライトバック]
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	// メモリープールがどこかを示す[転送はL0、つまりCPU側から直接行う]
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// 単一アダプターのため0
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	// 中間バッファーとしてのアップロードヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProp = {};
	// マップ可能にするため、UPLOADにする
	uploadHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// アップロード用に使用すること前提なのでUNKNOWNでよい
	uploadHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	uploadHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	uploadHeapProp.CreationNodeMask = 0;	// 単一アダプターのため0
	uploadHeapProp.VisibleNodeMask = 0;	// 単一アダプターのため0


	// リソースの設定
	D3D12_RESOURCE_DESC resDesc = {};
	// フォーマット[単なるデータの塊なのでUNKNOWN]
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	// 利用方法[単なるバッファーとして指定]
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	// サイズ幅[データサイズ]
	resDesc.Width = dataSize;
	// サイズ高さ
	resDesc.Height = 1;
	// 深度や配列サイズ[2D配列でもないので]
	resDesc.DepthOrArraySize = 1;
	// ミップマップレベル[ミップマップしないのでミップ数は１つ]
	resDesc.MipLevels = 1;
	// レイアウト[連続したデータ]
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 特にフラグなし
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	// アンチエイリアシングのパラメーター[通常テクスチャーなのでアンチエイリアシングしない]
	resDesc.SampleDesc.Count = 1;
	// クオリティ[クオリティは最低]
	resDesc.SampleDesc.Quality = 0;

	// 中間バッファーを作成
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,	// CPUからの書き込み可能、GPUからは読み取りのみ
		nullptr,
		IID_PPV_ARGS(_uploadBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// テクスチャーのためのヒープの設定
	D3D12_HEAP_PROPERTIES texHeapProp = {};
	texHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;		// テクスチャー用
	// アップロード用に使用すること前提なのでUNKNOWNでよい
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	texHeapProp.CreationNodeMask = 0;	// 単一アダプターのため0
	texHeapProp.VisibleNodeMask = 0;	// 単一アダプターのため0

	// フォーマット[RGBA]
	resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// サイズ幅
	resDesc.Width = (UINT64)(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
	// サイズ高さ
	resDesc.Height = (UINT64)(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
	// 深度や配列サイズ[2D配列でもないので]
	resDesc.DepthOrArraySize = 1;
	// ミップマップレベル[ミップマップしないのでミップ数は１つ]
	resDesc.MipLevels = 1;
	// 利用方法[2Dテクスチャー用]
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	// レイアウト[レイアウトは決定しない]
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	// アンチエイリアシングのパラメーター[通常テクスチャーなのでアンチエイリアシングしない]
	resDesc.SampleDesc.Count = 1;
	// クオリティ[クオリティは最低]
	resDesc.SampleDesc.Quality = 0;
	// 特にフラグなし
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// リソースバッファーを作成
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,	// コピー先
		nullptr,
		IID_PPV_ARGS(_texWhiteBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	std::vector<unsigned char> data(dataSize);
	std::fill(data.begin(), data.end(), 0xff);

	// アップロードリソースへのマップ
	uint8_t* mapForImg = nullptr;	// image->pixelsと同じ型にする
	hr = _uploadBuffer->Map(0, nullptr, (void**)&mapForImg);	// マップ
	if (FAILED(hr))
		return hr;

	std::copy_n(&data[0], dataSize, mapForImg);
	_uploadBuffer->Unmap(0, nullptr);	// アンマップ

	// コピーテクスチャーレギオンの設定
	// コピー元(アップロード側)設定
	D3D12_TEXTURE_COPY_LOCATION src = {};
	src.pResource = _uploadBuffer.Get();	// 中間バッファー
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;	// プットプリント指定[バッファーリソース]
	src.PlacedFootprint.Offset = 0;							// オフセット
	src.PlacedFootprint.Footprint.Width = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;		// 幅
	src.PlacedFootprint.Footprint.Height = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;	// 高さ
	src.PlacedFootprint.Footprint.Depth = 1;				// 深さ
	src.PlacedFootprint.Footprint.RowPitch = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT * bytePerPixel;// 1行あたりのバイト数
	src.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// フォーマット

	// コピー先設定
	D3D12_TEXTURE_COPY_LOCATION dst = {};
	dst.pResource = _texWhiteBuffer.Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;	// インデックス[テクスチャーリソース]
	dst.SubresourceIndex = 0;								// インデックス番号

	// コマンドアロケーターをリセット
	//DX12::GetInstance()->CommandAllocator()->Reset();
	// 再びコマンドリストをためる準備[クローズ状態の解除]
	DX12::GetInstance()->CommandList()->Reset(DX12::GetInstance()->CommandAllocator().Get(), nullptr);

	// コマンドリストにコピーテクスチャーを追加
	DX12::GetInstance()->CommandList()->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

	// リソースバリアーの指定
	D3D12_RESOURCE_BARRIER BarrierDesc = {};
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	// 遷移
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;		// 特に指定なし
	BarrierDesc.Transition.pResource = _texWhiteBuffer.Get();				// テクスチャーリソース
	BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	BarrierDesc.Transition.StateBefore
		= D3D12_RESOURCE_STATE_COPY_DEST;						// コピーを選択
	BarrierDesc.Transition.StateAfter
		= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;			// ピクセルシェーダーリソースを選択
	// リソースバリアーの実行
	DX12::GetInstance()->CommandList()->ResourceBarrier(1, &BarrierDesc);

	// 命令のクローズ
	DX12::GetInstance()->CommandList()->Close();

	// コマンドリストの実行
	ID3D12CommandList* cmdLists[] = { DX12::GetInstance()->CommandList().Get() };
	DX12::GetInstance()->CommandQueue()->ExecuteCommandLists(1, cmdLists);

	DX12::GetInstance()->WaitForPreviousFrame();

	return S_OK;
}
// 黒色テクスチャーの生成(textureがないマテリアル用)
HRESULT BaseClass::CreateBlackTexture()
{
	HRESULT hr = S_OK;

	// ヒーププロパティの設定
	D3D12_HEAP_PROPERTIES heapProp = {};
	// ヒープの種類[特殊な設定なのでDEFAULTでもUPLOADでもない]
	heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	// CPUのページング設定[ライトバック]
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	// メモリープールがどこかを示す[転送はL0、つまりCPU側から直接行う]
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// 単一アダプターのため0
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	// 中間バッファーとしてのアップロードヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProp = {};
	// マップ可能にするため、UPLOADにする
	uploadHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// アップロード用に使用すること前提なのでUNKNOWNでよい
	uploadHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	uploadHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	uploadHeapProp.CreationNodeMask = 0;	// 単一アダプターのため0
	uploadHeapProp.VisibleNodeMask = 0;	// 単一アダプターのため0

	// 1ピクセル当たりのバイト数
	UINT bytePerPixel = 4;

	// リソースの設定
	D3D12_RESOURCE_DESC resDesc = {};
	// フォーマット[単なるデータの塊なのでUNKNOWN]
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	// 利用方法[単なるバッファーとして指定]
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	// サイズ幅[データサイズ]
	resDesc.Width = dataSize;
	// サイズ高さ
	resDesc.Height = 1;
	// 深度や配列サイズ[2D配列でもないので]
	resDesc.DepthOrArraySize = 1;
	// ミップマップレベル[ミップマップしないのでミップ数は１つ]
	resDesc.MipLevels = 1;
	// レイアウト[連続したデータ]
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 特にフラグなし
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	// アンチエイリアシングのパラメーター[通常テクスチャーなのでアンチエイリアシングしない]
	resDesc.SampleDesc.Count = 1;
	// クオリティ[クオリティは最低]
	resDesc.SampleDesc.Quality = 0;

	// 中間バッファーを作成
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,	// CPUからの書き込み可能、GPUからは読み取りのみ
		nullptr,
		IID_PPV_ARGS(_uploadBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// テクスチャーのためのヒープの設定
	D3D12_HEAP_PROPERTIES texHeapProp = {};
	texHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;		// テクスチャー用
	// アップロード用に使用すること前提なのでUNKNOWNでよい
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	texHeapProp.CreationNodeMask = 0;	// 単一アダプターのため0
	texHeapProp.VisibleNodeMask = 0;	// 単一アダプターのため0

	// フォーマット[RGBA]
	resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// サイズ幅
	resDesc.Width = (UINT64)(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
	// サイズ高さ
	resDesc.Height = (UINT64)(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
	// 深度や配列サイズ[2D配列でもないので]
	resDesc.DepthOrArraySize = 1;
	// ミップマップレベル[ミップマップしないのでミップ数は１つ]
	resDesc.MipLevels = 1;
	// 利用方法[2Dテクスチャー用]
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	// レイアウト[レイアウトは決定しない]
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	// アンチエイリアシングのパラメーター[通常テクスチャーなのでアンチエイリアシングしない]
	resDesc.SampleDesc.Count = 1;
	// クオリティ[クオリティは最低]
	resDesc.SampleDesc.Quality = 0;
	// 特にフラグなし
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// リソースバッファーを作成
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,	// コピー先
		nullptr,
		IID_PPV_ARGS(_texBlackBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	std::vector<unsigned char> data(dataSize);
	std::fill(data.begin(), data.end(), 0x00);

	// アップロードリソースへのマップ
	uint8_t* mapForImg = nullptr;	// image->pixelsと同じ型にする
	hr = _uploadBuffer->Map(0, nullptr, (void**)&mapForImg);	// マップ
	if (FAILED(hr))
		return hr;

	std::copy_n(&data[0], D3D12_TEXTURE_DATA_PITCH_ALIGNMENT * D3D12_TEXTURE_DATA_PITCH_ALIGNMENT * bytePerPixel, mapForImg);
	_uploadBuffer->Unmap(0, nullptr);	// アンマップ

	// コピーテクスチャーレギオンの設定
	// コピー元(アップロード側)設定
	D3D12_TEXTURE_COPY_LOCATION src = {};
	src.pResource = _uploadBuffer.Get();	// 中間バッファー
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;	// プットプリント指定[バッファーリソース]
	src.PlacedFootprint.Offset = 0;							// オフセット
	src.PlacedFootprint.Footprint.Width = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;		// 幅
	src.PlacedFootprint.Footprint.Height = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;	// 高さ
	src.PlacedFootprint.Footprint.Depth = 1;				// 深さ
	src.PlacedFootprint.Footprint.RowPitch = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT * bytePerPixel;// 1行あたりのバイト数
	src.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// フォーマット

	// コピー先設定
	D3D12_TEXTURE_COPY_LOCATION dst = {};
	dst.pResource = _texBlackBuffer.Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;	// インデックス[テクスチャーリソース]
	dst.SubresourceIndex = 0;								// インデックス番号

	// コマンドアロケーターをリセット
	DX12::GetInstance()->CommandAllocator()->Reset();
	// 再びコマンドリストをためる準備[クローズ状態の解除]
	DX12::GetInstance()->CommandList()->Reset(DX12::GetInstance()->CommandAllocator().Get(), nullptr);

	// コマンドリストにコピーテクスチャーを追加
	DX12::GetInstance()->CommandList()->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

	// リソースバリアーの指定
	D3D12_RESOURCE_BARRIER BarrierDesc = {};
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	// 遷移
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;		// 特に指定なし
	BarrierDesc.Transition.pResource = _texBlackBuffer.Get();				// テクスチャーリソース
	BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	BarrierDesc.Transition.StateBefore
		= D3D12_RESOURCE_STATE_COPY_DEST;						// コピーを選択
	BarrierDesc.Transition.StateAfter
		= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;			// ピクセルシェーダーリソースを選択
	// リソースバリアーの実行
	DX12::GetInstance()->CommandList()->ResourceBarrier(1, &BarrierDesc);

	// 命令のクローズ
	DX12::GetInstance()->CommandList()->Close();

	// コマンドリストの実行
	ID3D12CommandList* cmdLists[] = { DX12::GetInstance()->CommandList().Get() };
	DX12::GetInstance()->CommandQueue()->ExecuteCommandLists(1, cmdLists);

	DX12::GetInstance()->WaitForPreviousFrame();

	return S_OK;
}

// テクスチャ読み込み(char*)
HRESULT BaseClass::LoadTextureFromFile(const char* fileName,
	BYTE** pImageData, SIZE* pSize)
{
	TCHAR pFileName[256] = {};
#ifdef UNICODE
	MultiByteToWideChar(CP_OEMCP, MB_PRECOMPOSED, fileName, (int)strlen(fileName), pFileName, (sizeof pFileName) / 2);
#else
	strcpy(pFileName, fileName);
#endif
	// 画像の読み込み
	if (!FAILED(LoadTextureFromFile(pFileName, pImageData, pSize)))
		return S_OK;
	return E_FAIL;
}
// 画像の読み込み(本体)(TCHAR*)
HRESULT  BaseClass::LoadTextureFromFile(const TCHAR* pFileName, BYTE** pImageData, SIZE* pSize)
{
	HRESULT hr = S_OK;

	// イメージングファクトリー生成
	CComPtr<IWICImagingFactory> pImagingFactory;
	if (FAILED(CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		reinterpret_cast<void**>(&pImagingFactory))))
	{
		OutputDebugString(_T("ImagingFactory生成に失敗\n"));
		return E_FAIL;
	}

	// デコーダー生成
	CComPtr<IWICBitmapDecoder> pDecoder;
	hr = pImagingFactory->CreateDecoderFromFilename(
		pFileName,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad, &pDecoder);
	if (FAILED(hr))
	{
		pImagingFactory.Release();
		OutputDebugString(_T("Decoder生成に失敗\n"));
		return E_FAIL;
	}


	// フレーム取得 
	//IWICBitmapFrameDecode* pFrame = nullptr;
	CComPtr<IWICBitmapFrameDecode> pFrame;
	hr = pDecoder->GetFrame(0, &pFrame);
	if (FAILED(hr))
	{
		pImagingFactory.Release();
		pDecoder.Release();
		OutputDebugString(_T("Frame取得に失敗\n"));
		return E_FAIL;
	}

	// コンバーターによりフォーマット変換 
	//IWICFormatConverter* pConverter = nullptr;
	CComPtr<IWICFormatConverter> pConverter;
	hr = pImagingFactory->CreateFormatConverter(&pConverter);
	if (FAILED(hr))
	{
		pImagingFactory.Release();
		pDecoder.Release();
		pFrame.Release();
		OutputDebugString(_T("Converter生成に失敗\n"));
		return E_FAIL;
	}

	// フォーマット情報の取得
	WICPixelFormatGUID PixelFormat;
	pFrame->GetPixelFormat(&PixelFormat);

	// コンバーターの初期化(32Bitフォーマット)
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
		OutputDebugString(_T("Converter生成に失敗\n"));
		return E_FAIL;
	}

	// 画像の幅、高さを指定
	UINT ImageWidth, ImageHeight;
	pFrame->GetSize(&ImageWidth, &ImageHeight);
	pSize->cx = ImageWidth;
	pSize->cy = ImageHeight;

	// 1ピクセル当たりのバイト数
	UINT BytePerPixel = 4;
	// データサイズの決定(RGBA)
	UINT ImageSize = ImageWidth * ImageHeight * BytePerPixel;
	// 保存領域作成
	*pImageData = new uint8_t[ImageSize];

	WICRect Rect = { 0, 0, static_cast<int>(ImageWidth), static_cast<int>(ImageHeight) };
	UINT Size = ImageWidth * ImageHeight;
	pConverter->CopyPixels(&Rect, ImageWidth * BytePerPixel, Size * BytePerPixel, *pImageData);


	if (PixelFormat == GUID_WICPixelFormat24bppBGR)
	{
		// 抜け色を指定(黒色)
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
// アライメントにそろえたサイズを返す
// @param size 元のサイズ
// @param alignment アライメントサイズ
// @param アライメントそろえたサイズ
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
// 識別子を取得
std::wstring BaseClass::GetWideStringFromString(const std::string& str)
{
	// 呼び出し一回目(文字列数を得る)
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

	// 呼び出し二回目(確保済みのwstrに変換文字列をコピー)
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

// エラーメッセージ
HRESULT BaseClass::BlobErrorMessange(HRESULT hr,
	ID3DBlob* errorBlob)
{
	if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
	{
		::OutputDebugStringA("ファイルが見当たりません\n");
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