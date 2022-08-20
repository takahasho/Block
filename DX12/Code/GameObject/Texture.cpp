// --------------------- Include File --------------------
#include "Texture.h"
#include "../Camera/Camera.h"
#include "../DX12System.h"
Texture* Texture::_instance = nullptr;			// インスタンスの実態
// コンストラクター
Texture::Texture()
{

}
// デストラクター
Texture::~Texture()
{
	for (auto obj : _object)
	{
		delete obj.mesh;
	}

	for (auto mesh : _mesh)
	{
		delete mesh.second;
	}
}
// 生成
void Texture::Create()
{
	if (!_instance)
	{
		_instance = new Texture();

		// テクスチャーの生成
		_instance->CreateTexture();
	}
}
// 破棄
void Texture::Destroy()
{
	delete _instance;
}
// 生成
HRESULT Texture::CreateTexture()
{
	HRESULT hr = S_OK;

	_dev = DX12::GetInstance()->GetDevice();
	_cmdList = DX12::GetInstance()->CommandList();
	_cmdAllocator = DX12::GetInstance()->CommandAllocator();

	// ルートシグネチャーの生成
	if (FAILED(CreateRootSignature()))
		return E_FAIL;

	// 描画パイプラインステートの生成
	if (FAILED(CreateGraphicsPipelineState()))
		return E_FAIL;

	return S_OK;
}
// ルートシグネチャーの生成
HRESULT Texture::CreateRootSignature()
{
	HRESULT hr = S_OK;

	// ディスクリプターレンジを作成
	D3D12_DESCRIPTOR_RANGE descTblRange[2] = {};

	// テクスチャー用
	// ディスクリプターの数[１つ]
	descTblRange[0].NumDescriptors = 1;
	// レンジの種類[SRV:シェーダーリソースビュー]
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	// 先頭レジスター番号[0を選択]
	descTblRange[0].BaseShaderRegister = 0;
	// オフセットを指定[連続したディスクリプターレンジが前のディスクリプタレンジの直後にくる]
	descTblRange[0].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// 定数バッファー用
	// ディスクリプターの数[１つ]
	descTblRange[1].NumDescriptors = 1;
	// レンジの種類[CBV:コンスタントバッファービュー]
	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// 先頭レジスター番号[0を選択]
	descTblRange[1].BaseShaderRegister = 0;
	// オフセットを指定[連続したディスクリプターレンジが前のディスクリプタレンジの直後にくる]
	descTblRange[1].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ルートパラメーターを作成
	D3D12_ROOT_PARAMETER rootParam = {};
	// 種別の選択[ディスクリプターテーブルを選択]
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// どのシェーダーから利用可能か[全シェーダー]
	rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// ディスクリプターレンジのアドレス
	rootParam.DescriptorTable.pDescriptorRanges =
		descTblRange;
	// ディスクリプターレンジの数
	rootParam.DescriptorTable.NumDescriptorRanges = 2;

	// サンプラーを作成
	D3D12_STATIC_SAMPLER_DESC samplerDecs = {};
	// 横方向の繰り返し
	samplerDecs.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// 縦方向の繰り返し
	samplerDecs.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// 奥行方向の繰り返し
	samplerDecs.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// ボーダー色[黒色]
	samplerDecs.BorderColor =
		D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	// 線形補間[回りの色の平均値]
	samplerDecs.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	// ミップマップの最大値
	samplerDecs.MaxLOD = D3D12_FLOAT32_MAX;
	// ミップマップの最小値
	samplerDecs.MinLOD = 0.0f;
	// どのシェーダーから利用可能か[ピクセルシェーダー]
	samplerDecs.ShaderVisibility =
		D3D12_SHADER_VISIBILITY_PIXEL;
	// 比較関数[リサンプリングしない]
	samplerDecs.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

	// ルートシグネチャーの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	// フラグ[頂点情報(入力アセンブラ)がある]
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	// パラメーターの先頭アドレス
	rootSignatureDesc.pParameters = &rootParam;
	// パラメーターの数
	rootSignatureDesc.NumParameters = 1;
	// サンプラーの先頭アドレス
	rootSignatureDesc.pStaticSamplers = &samplerDecs;
	// サンプラーの数
	rootSignatureDesc.NumStaticSamplers = 1;

	// バイナリコードを作成
	ID3DBlob* rootSigBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	hr = D3D12SerializeRootSignature(
		&rootSignatureDesc,				// ルートシグネチャー設定
		D3D_ROOT_SIGNATURE_VERSION_1_0,	// ルートシグネチャーバージョン
		&rootSigBlob,					// 受け取るためのポインターのアドレス
		&errorBlob);					// エラー用ポインターのアドレス[エラー時はerrorBlobにメッセージが入る]
	if (FAILED(hr))
	{
		// エラーメッセージ
		BlobErrorMessange(hr, errorBlob);
		return E_FAIL;
	}

	// ルートシグネチャーオブジェクトの作成
	hr = _dev->CreateRootSignature(
		0,	// nodemask。０でよい
		rootSigBlob->GetBufferPointer(),	// アドレス
		rootSigBlob->GetBufferSize(),		// サイズ
		IID_PPV_ARGS(_rootSignature.GetAddressOf()));	// アドレスを措定
	if (hr != S_OK)
		return E_FAIL;

	rootSigBlob->Release();		// 不要になったので解放

	return hr;
}
// 描画パイプラインの生成
HRESULT Texture::CreateGraphicsPipelineState()
{
	HRESULT hr = S_OK;

	// シェーダーオブジェクト読み込み用
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// 頂点シェーダーの読み込み
	hr = D3DCompileFromFile(
		L".\\Shader\\Texture\\TextureVertexShader.hlsl",			// シェーダー名
		nullptr,							// defineはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// インクルードはデフォルト
		"TextureVS", "vs_5_0",				// 関数は BasicVS、対象シェーダーは vs_5_0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバック様および最適化なし	
		0,									// エフェクトコンパイルオプション[0を推奨]
		&vsBlob,							// 受け取るためのポインターのアドレス
		&errorBlob);						// エラー用ポインターのアドレス[エラー時はerrorBlobにメッセージが入る]
	if (FAILED(hr))
	{
		// エラーメッセージ
		BlobErrorMessange(hr, errorBlob);
		return E_FAIL;
	}

	// ピクセルシェーダーの読み込み
	hr = D3DCompileFromFile(
		L".\\Shader\\Texture\\TexturePixelShader.hlsl",			// シェーダー名
		nullptr,							// defineはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// インクルードはデフォルト
		"TexturePS", "ps_5_0",				// 関数は BasicPS、対象シェーダーは ps_5_0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバック様および最適化なし	
		0,									// エフェクトコンパイルオプション[0を推奨]
		&psBlob,							// 受け取るためのポインターのアドレス
		&errorBlob);						// エラー用ポインターのアドレス[エラー時はerrorBlobにメッセージが入る]
	if (FAILED(hr))
	{
		// エラーメッセージ
		BlobErrorMessange(hr, errorBlob);
		return E_FAIL;
	}

	// 頂点入力レイアウトを作成
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{	// 座標情報
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{	// UV情報
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	
	// グラフィックスパイプラインステート(PSO:pipeline state object)を作成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipelineDesc = {};
	// ルートシグネチャー
	gpipelineDesc.pRootSignature = _rootSignature.Get();
	// 頂点シェーダーのアドレスとサイズ
	gpipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	gpipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	// ピクセルシェーダーのアドレスとサイズ
	gpipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	gpipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// サンプルマスクとラスタライザーステートの設定
	// デフォルトのサンプルマスクを表す定数(0xffffffff)
	gpipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// まだアンチエイリアスは使わないためfalse
	gpipelineDesc.RasterizerState.MultisampleEnable = FALSE;
	// カリングしない(表裏描画)
	gpipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	// 中身を塗り潰す
	gpipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	// 深度方向のクリッピングは有効に
	gpipelineDesc.RasterizerState.DepthClipEnable = TRUE;

	// ブレンドステートの設定
	// アルファー網羅率の有無[なし]
	gpipelineDesc.BlendState.AlphaToCoverageEnable = FALSE;
	// レンダーターゲットにブレンドステートの割り当ての有無[なし]
	gpipelineDesc.BlendState.IndependentBlendEnable = FALSE;

	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	for (UINT i = 0; i < _countof(gpipelineDesc.BlendState.RenderTarget); ++i)
	{
		// ブレンドの有無[あり]
		renderTargetBlendDesc.BlendEnable = TRUE;
		// 論理演算の有無[なし]
		renderTargetBlendDesc.LogicOpEnable = FALSE;

		// 描画元係数[1乗算:ブレンディング係数を(As, As, As, As)]
		renderTargetBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		// 描画先係数[0乗算:ブレンディング係数を(1-As, 1-As, 1-As, 1-As)]
		renderTargetBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		// どうブレンドするのか[加算]
		renderTargetBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;

		// 描画元α係数[1乗算:素材の元の色]
		renderTargetBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		// 描画先α係数[0乗算:何でもゼロ]
		renderTargetBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		// αをどうブレンドするのか[加算]
		renderTargetBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;

		// どう論理演算するのか[描画元そのままの値]
		renderTargetBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;

		// RGBAそれぞれの値を書き込むか指定[全て]
		renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		// レンダーターゲットブレンドをPSOに紐づける
		gpipelineDesc.BlendState.RenderTarget[i] = renderTargetBlendDesc;
	}
	

	// 入力レイアウトの設定
	// 頂点入力レイアウトをPSOに紐づける
	gpipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	// 入力レイアウトのサイズを指定[レイアウト配列の要素数]
	gpipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	// インデックスバッファー利用時、ストリップカット値設定[カットなし]
	gpipelineDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

	// プリミティブのタイプ[三角形]
	gpipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// レンダーターゲットの設定
	// レンダーターゲット数[１でよい]
	gpipelineDesc.NumRenderTargets = 1;
	// フォーマット[0番のみR8G8B8A8_UNORMで設定]
	gpipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	// アンチエイリアシングのめのサンプル数の設定
	// マルチサンプリング数[サンプリングは１ピクセルにつき１]
	gpipelineDesc.SampleDesc.Count = 1;
	// マルチサンプリングのクオリティ[クオリティは最低]
	gpipelineDesc.SampleDesc.Quality = 0;

	// グラフィックパイプラインステートオブジェクトを生成
	hr = _dev->CreateGraphicsPipelineState(
		&gpipelineDesc, IID_PPV_ARGS(_pipelineState.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	return S_OK;
}
// PLANE頂点/インデックスバッファーの生成
HRESULT Texture::CreateVertexIndexBufferView(MESH* mesh, float width, float height)
{
	HRESULT hr = S_OK;
	LONG x = g_window_width;
	LONG y = g_window_height;
	TextureVertex vertices[4] = {};
	vertices[0] = { {  -width / x ,   height / y , 0.0f}, {0.0f, 1.0f} };	// 左下
	vertices[1] = { {  -width / x ,  -height / y , 0.0f}, {0.0f, 0.0f} };	// 左上
	vertices[2] = { {   width / x ,   height / y , 0.0f}, {1.0f, 1.0f} };	// 右下
	vertices[3] = { {   width / x ,  -height / y , 0.0f}, {1.0f, 0.0f} };	// 右上		

	// ヒーププロパティの設定
	D3D12_HEAP_PROPERTIES heapProp = {};
	// ヒープの種類[CPUからアクセスできる(マップできる)]
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// CPUのページング設定[考えなくてもよい]
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	// メモリープールがどこかを示す[CUSTOM以外のときはこれでよい]
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	// リソースの設定
	D3D12_RESOURCE_DESC resDesc = {};
	// 利用方法[バッファーに使うのでBUFFERを指定]
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	// サイズ幅[幅で全部まかなうのでsizeof(全頂点)とする]
	resDesc.Width = sizeof(TextureVertex) * PlaneNumVertices;
	// サイズ高さ[幅で表現しているので１とする]
	resDesc.Height = 1;
	// 深度や配列サイズ[１でよい]
	resDesc.DepthOrArraySize = 1;
	// ミップマップレベル[１でよい]
	resDesc.MipLevels = 1;
	// フォーマット[画像ではないのでUNKNOWNでよい]
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	// アンチエイリアシングのパラメーター[アンチエイリアシングを行わないためSampleDesc.Count = 1]
	resDesc.SampleDesc.Count = 1;
	// フラグ[NONEでよい]
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	// レイアウト[今回はテクスチャーのレイアウトではないので、メモリーが最初から最後迄連続していることを示す]
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// リソースオブジェクトを作成
	hr = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(mesh->vertexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// 頂点情報のコピー
	TextureVertex* vertexMap = nullptr;
	// 頂点バッファーのロック
	hr = mesh->vertexBuffer->Map(0, nullptr,
		(void**)&vertexMap);
	if (hr != S_OK)
		return hr;
	memcpy(vertexMap, vertices, sizeof(TextureVertex) * PlaneNumVertices);
	// 頂点バッファーの解除
	mesh->vertexBuffer->Unmap(0, nullptr);

	// 頂点バッファービューを作成
	// 頂点バッファーのアドレス[バッファーの仮想アドレス]
	mesh->vertexBufferView.BufferLocation = mesh->vertexBuffer->GetGPUVirtualAddress();
	// 総バイト数[全バイト数]
	mesh->vertexBufferView.SizeInBytes = sizeof(TextureVertex) * PlaneNumVertices;
	// 1頂点あたりのバイト数
	mesh->vertexBufferView.StrideInBytes = sizeof(TextureVertex);

	// サイズ幅[幅で全部まかなうのでsizeof(全インデックス)とする]
	resDesc.Width = sizeof(PlaneIndices);

	// リソースオブジェクトを作成
	hr = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(mesh->indexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// インデックス情報のコピー
	unsigned short* indexMap = nullptr;
	// インデックスバッファーのロック
	hr = mesh->indexBuffer->Map(0, nullptr,
		(void**)&indexMap);
	if (hr != S_OK)
		return hr;
	std::copy(std::begin(PlaneIndices), std::end(PlaneIndices),
		indexMap);
	// インデックスバッファーの解除
	mesh->indexBuffer->Unmap(0, nullptr);

	// インデックスバッファービューを作成
	// インデックスバッファーのアドレス[バッファーの仮想アドレス]
	mesh->indexBufferView.BufferLocation = mesh->indexBuffer->GetGPUVirtualAddress();
	// 総バイト数[全バイト数]
	mesh->indexBufferView.SizeInBytes = sizeof(PlaneIndices);
	// フォーマット
	mesh->indexBufferView.Format = DXGI_FORMAT_R16_UINT;

	return S_OK;
}
// コンスタントバッファービューの生成
HRESULT Texture::CreateConstantBufferView(MESH* mesh)
{
	HRESULT hr = S_OK;

	// 定数バッファーの作成
	// ヒーププロパティの設定
	D3D12_HEAP_PROPERTIES heapProp = {};
	// ヒープの種類[CPUからアクセスできる(マップできる)]
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// CPUのページング設定[考えなくてもよい]
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	// メモリープールがどこかを示す[CUSTOM以外のときはこれでよい]
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	// リソースの設定
	D3D12_RESOURCE_DESC resDesc = {};
	// 利用方法[バッファーに使うのでBUFFERを指定]
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	// サイズ幅[幅で全部まかなうのでsizeof(全情報):256の倍数とする]
	resDesc.Width = (sizeof(MetaData) + 0xff) & ~0xff;
	// サイズ高さ[幅で表現しているので１とする]
	resDesc.Height = 1;
	// 深度や配列サイズ[１でよい]
	resDesc.DepthOrArraySize = 1;
	// ミップマップレベル[１でよい]
	resDesc.MipLevels = 1;
	// フォーマット[画像ではないのでUNKNOWNでよい]
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	// アンチエイリアシングのパラメーター[アンチエイリアシングを行わないためSampleDesc.Count = 1]
	resDesc.SampleDesc.Count = 1;
	// フラグ[NONEでよい]
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	// レイアウト[今回はテクスチャーのレイアウトではないので、メモリーが最初から最後迄連続していることを示す]
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// リソースオブジェクトを作成
	hr = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(mesh->constBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;
	// デスクリプタヒープを作成
	// ヒープの記述を用意
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	// 種類の選択[CBV と SRVを利用]
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	// GPUを１つだけ使用する想定
	heapDesc.NodeMask = 0;
	// デスクリプタを指定[SRV１つ + CBV１つ]
	heapDesc.NumDescriptors = 2;
	// 特に指定なし[シェーダー側から見える必要あり]
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// デスクリプタヒープを作成
	hr = _dev->CreateDescriptorHeap(&heapDesc
		, IID_PPV_ARGS(mesh->basicDescHeap.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// ディスクリプタの先頭ハンドルを取得しておく
	auto basicHeapHandle = mesh->basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	// シェーダーリソースビューを作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	// フォーマットを指定[R8G8B8_UINT:RGBA(0.0f～1.0fに正規化)]
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// コンポーネントマッピングを指定[ディフォルトを選択]
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	// 共用体のどのメンバーとして扱うか[テクスチャー2Dを選択]
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	// ミップマップの数を指定[使用しないので1]
	srvDesc.Texture2D.MipLevels = 1;
	_dev->CreateShaderResourceView(
		mesh->textureBuffer[0].Get(),	// ビューと関連付けるバッファーを指定
		&srvDesc,				// テクスチャー選定情報を指定
		basicHeapHandle);		// ヒープのどこに割り当てるか

	// 次の場所に移動
	basicHeapHandle.ptr +=
		_dev->GetDescriptorHandleIncrementSize
		(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = mesh->constBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)mesh->constBuffer->GetDesc().Width;

	// 定数バッファービューの作成
	_dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	return S_OK;
}
// シェーダーリソースの生成
HRESULT Texture::CreateShaderResource(const char* pFileName, ComPtr<ID3D12Resource>& textureBuffer, SIZE& size)
{

	HRESULT hr = S_OK;
	// 画像サイズの取得とイメージデーター確保
	BYTE* pImageData = nullptr;
	size = {};

	// 画像情報の読み込み
	if (FAILED(LoadTextureFromFile(pFileName,
		&pImageData, &size)))
		return E_FAIL;

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

	// リソースの設定
	D3D12_RESOURCE_DESC resDesc = {};
	// 利用方法[2Dテクスチャー]
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	// サイズ幅
	resDesc.Width = (UINT64)size.cx;
	// サイズ高さ
	resDesc.Height = (UINT64)size.cy;
	// 深度や配列サイズ[2Dで配列でもないので1]
	resDesc.DepthOrArraySize = 1;
	// ミップマップレベル[ミップマップしないのでミップ数は1つ]
	resDesc.MipLevels = 1;
	// フォーマット[RGBAフォーマット]
	resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// アンチエイリアシングのパラメーター[アンチエイリアシングを行わないためSampleDesc.Count = 1]
	resDesc.SampleDesc.Count = 1;
	// フラグ[NONEでよい]
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	// レイアウト[レイアウトは決定しない]
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	// 品質[クオリティーは最低]
	resDesc.SampleDesc.Quality = 1;

	// リソースオブジェクトを作成
	hr = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,	// テクスチャー用指定
		nullptr,
		IID_PPV_ARGS(textureBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// 1ピクセル当たりのバイト数
	UINT bytePerPixel = 4;

	// データ転送
	hr = textureBuffer->WriteToSubresource(
		0,			// サブリソースインデックス[0を指定]
		nullptr,	// 書き込み領域を指定[nullptrは全体]
		pImageData,	// 書き込みたいデータのアドレス
		(UINT)(bytePerPixel * size.cx),// １ラインサイズ
		(UINT)(size.cy));// １ライン高さのサイズ
	if (FAILED(hr))
		return hr;

	// イメージ情報の解放
	delete[] pImageData;
	return S_OK;
}
// 描画
void Texture::Draw()
{
	// グラフィックスパイプラインステートの指定
	_cmdList->SetPipelineState(_pipelineState.Get());
	// ルートシグネチャーの指定
	_cmdList->SetGraphicsRootSignature(_rootSignature.Get());

	// 頂点情報の指定[三角形]
	_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (int i = -2; i <= _orderNumMax; ++i)
	{
		for (int j = 0, size = (int)_drawOrder.size(); j < size; ++j)
		{
			if (_drawOrder[j] == i)
			{
				if (_drawflag[j])
				{
					if (!_object[j].info.uiFlag)
					{
						MetaData* mapMatrix = nullptr;	// マップ先を示すポインター
						_object[j].mesh->constBuffer->Map(0, nullptr, (void**)&mapMatrix);	// マップ
						XMMATRIX matrix = XMMatrixIdentity();

						// =====ワールド行列=====
						LONG x = g_window_width / 2;
						LONG y = g_window_height / 2;
						XMMATRIX world_matrix;
						world_matrix = XMMatrixIdentity();
						XMMATRIX translate = XMMatrixTranslation(_object[j].info.position.x, _object[j].info.position.y, _object[j].info.position.z);
						XMMATRIX rotate_x = XMMatrixRotationX(XMConvertToRadians(_object[j].info.angle.x));
						XMMATRIX rotate_y = XMMatrixRotationY(XMConvertToRadians(_object[j].info.angle.y));
						XMMATRIX rotate_z = XMMatrixRotationZ(XMConvertToRadians(_object[j].info.angle.z));
						XMMATRIX scale_mat = XMMatrixScaling(-_object[j].info.scale.x * x, -_object[j].info.scale.y * y, _object[j].info.scale.z);
						world_matrix = scale_mat * rotate_x * rotate_y * rotate_z * translate;

						// =====ビュー行列=====
						XMMATRIX viewMat = Camera::GetInstance()->GetView();

						// =====プロジェクション行列=====
						XMMATRIX projMat = Camera::GetInstance()->GetProjection();

						matrix = world_matrix * viewMat * projMat;

						mapMatrix->world = matrix;
						XMFLOAT4 color(_object[j].info.color.x, _object[j].info.color.y, _object[j].info.color.z, _object[j].info.alpha);
						mapMatrix->color = color;

						_object[j].mesh->constBuffer->Unmap(0, nullptr);		// アンマップ

						// ディスクリプターヒープの指定
						_cmdList->SetDescriptorHeaps(1, _object[j].mesh->basicDescHeap.GetAddressOf());

						// ルートパラメーターとディスクリプターヒープの関連付け
						_cmdList->SetGraphicsRootDescriptorTable(
							0,	// ルートパラメーターインデックス
							_object[j].mesh->basicDescHeap->GetGPUDescriptorHandleForHeapStart());


						// 頂点バッファーの指定
						_cmdList->IASetVertexBuffers(0, 1, &_object[j].mesh->vertexBufferView);

						// インデックスバッファーの指定
						_cmdList->IASetIndexBuffer(&_object[j].mesh->indexBufferView);

						// 描画命令
						_cmdList->DrawIndexedInstanced(PlaneNumIndices, 1, 0, 0, 0);
					}
				}
			}
		}
	}
	for (int i = -2; i <= _orderNumMax; ++i)
	{
		for (int j = 0, size = (int)_drawOrder.size(); j < size; ++j)
		{
			if (_drawOrder[j] == i)
			{
				if (_drawflag[j])
				{
					if (_object[j].info.uiFlag)
					{
						MetaData* mapMatrix = nullptr;	// マップ先を示すポインター
						_object[j].mesh->constBuffer->Map(0, nullptr, (void**)&mapMatrix);	// マップ
						XMMATRIX matrix = XMMatrixIdentity();

						// 画像を左上寄せするための変数
						LONG x = g_window_width / 2;
						LONG y = g_window_height / 2;
						FLOAT ratioX = (FLOAT)g_window_width / (FLOAT)g_window_height;
						FLOAT ratioY = (FLOAT)g_window_height / (FLOAT)g_window_width;		
						XMMATRIX translate = XMMatrixTranslation(_object[j].info.position.x / x - 1, -(_object[j].info.position.y / y - 1), 0);
						XMMATRIX rotationMat = XMMatrixRotationZ(XMConvertToRadians(_object[j].info.angle.z));
						// 引き伸ばされているので比率分掛ける
						rotationMat.r[0].m128_f32[1] *= ratioX;
						rotationMat.r[1].m128_f32[0] *= ratioY;
						XMMATRIX scale_mat = XMMatrixScaling(_object[j].info.scale.x, -_object[j].info.scale.y, 1);

						matrix = scale_mat * rotationMat * translate;

						mapMatrix->world = matrix;
						XMFLOAT4 color(_object[j].info.color.x, _object[j].info.color.y, _object[j].info.color.z, _object[j].info.alpha);
						mapMatrix->color = color;

						_object[j].mesh->constBuffer->Unmap(0, nullptr);		// アンマップ

						// ディスクリプターヒープの指定
						_cmdList->SetDescriptorHeaps(1, _object[j].mesh->basicDescHeap.GetAddressOf());

						// ルートパラメーターとディスクリプターヒープの関連付け
						_cmdList->SetGraphicsRootDescriptorTable(
							0,	// ルートパラメーターインデックス
							_object[j].mesh->basicDescHeap->GetGPUDescriptorHandleForHeapStart());


						// 頂点バッファーの指定
						_cmdList->IASetVertexBuffers(0, 1, &_object[j].mesh->vertexBufferView);

						// インデックスバッファーの指定
						_cmdList->IASetIndexBuffer(&_object[j].mesh->indexBufferView);

						// 描画命令
						_cmdList->DrawIndexedInstanced(PlaneNumIndices, 1, 0, 0, 0);
					}
				}
			}
		}
	}
}

void Texture::TextureDeleate(int& entryPos, const char* filename)
{
	if (entryPos != -1 && _mesh[filename])
	{
		_drawflag[entryPos] = false;
		entryPos = -1;
	}
}

void Texture::GetSize(const char* filename, float& width, float& height)
{
	width = _size[filename].width;
	height = _size[filename].height;
}

void Texture::SetTextureMetaData(int& entryPos, VECTOR position, VECTOR angle, VECTOR scale, VECTOR color, float a, int drawOrder, bool uiFlag, const char* filename)
{
	// 初めてエントリーしたならMesh生成そうでなければInfo書き換えのみ
	if (entryPos == -1)
	{
		if (!_mesh[filename])
		{
			SIZE size = {};
			_mesh[filename] = new MESH;
			// シェーダーリソースの生成
			CreateShaderResource(filename, _mesh[filename]->textureBuffer[0], size);

			float width = (float)size.cx;
			float height = (float)size.cy;

			_size[filename].width = width;
			_size[filename].height = height;

			// インデックスバーテックスバッファービューの生成
			CreateVertexIndexBufferView(_mesh[filename], width, height);
		}

		bool flag = false;
		// 空いてるエントリー番号を探す		
		for (int i = 0, size = (int)_drawflag.size(); i < size; i++)
		{
			if (_drawflag[i] == false)
			{
				entryPos = i;
				flag = true;
				break;
			}
		}
		if (!flag)
		{
			_drawflag.push_back(true);
			entryPos = (int)_drawflag.size() - 1;

			Object object;
			_object.push_back(object);
			_object.back().mesh = new MESH;
			_object.back().mesh->vertexBuffer = _mesh[filename]->vertexBuffer;
			_object.back().mesh->vertexBufferView = _mesh[filename]->vertexBufferView;
			_object.back().mesh->indexBuffer = _mesh[filename]->indexBuffer;
			_object.back().mesh->indexBufferView = _mesh[filename]->indexBufferView;
			_object.back().mesh->textureBuffer[0] = _mesh[filename]->textureBuffer[0];

			// コンスタントバッファービューの生成
			CreateConstantBufferView(_object.back().mesh);

			// 生成する情報代入
			_object.back().info.position = position;
			_object.back().info.angle = angle;
			_object.back().info.scale = scale;
			_object.back().info.color = color;
			_object.back().info.alpha = a;
			_object.back().info.uiFlag = uiFlag;
			_drawOrder.push_back(drawOrder);
		}
		else
		{
			_object[entryPos].mesh->vertexBuffer = _mesh[filename]->vertexBuffer;
			_object[entryPos].mesh->vertexBufferView = _mesh[filename]->vertexBufferView;
			_object[entryPos].mesh->indexBuffer = _mesh[filename]->indexBuffer;
			_object[entryPos].mesh->indexBufferView = _mesh[filename]->indexBufferView;
			_object[entryPos].mesh->textureBuffer[0] = _mesh[filename]->textureBuffer[0];

			// コンスタントバッファービューの生成
			CreateConstantBufferView(_object[entryPos].mesh);

			_drawflag[entryPos] = true;
			// 生成する情報代入
			_object[entryPos].info.position = position;
			_object[entryPos].info.angle = angle;
			_object[entryPos].info.scale = scale;
			_object[entryPos].info.color = color;
			_object[entryPos].info.alpha = a;
			_object[entryPos].info.uiFlag = uiFlag;
			_drawOrder[entryPos] = drawOrder;
		}
	}
	else
	{
		// 生成する情報代入
		_object[entryPos].info.position = position;
		_object[entryPos].info.angle = angle;
		_object[entryPos].info.scale = scale;
		_object[entryPos].info.color = color;
		_object[entryPos].info.alpha = a;
		_object[entryPos].info.uiFlag = uiFlag;
		_drawOrder[entryPos] = drawOrder;
	}	

	if (drawOrder > _orderNumMax)
	{
		_orderNumMax = drawOrder;
	}
}
