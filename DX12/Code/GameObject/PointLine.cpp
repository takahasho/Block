// -------------------- IncludeFile -----------------
#include "PointLine.h"
#include "../Camera/Camera.h"
#include "../DX12System.h"
PointLine* PointLine::_instance = nullptr;			// インスタンスの実態
// コンストラクタ
PointLine::PointLine() :
	_pointVertexBufferView(), _pointIndexBufferView(), _pointInstanceEntry(0),
	_lineVertexBufferView(), _lineIndexBufferView(), _lineInstanceEntry(0),
	_pointMetaData(), _lineMetaData()
{

}
// デストラクタ
PointLine::~PointLine()
{
	for (auto mesh : _pointMeshs)
	{
		delete mesh;
	}
	for (auto mesh : _lineMeshs)
	{
		delete mesh;
	}
}
// 作成
void PointLine::Create()
{
	if (!_instance)
	{
		_instance = new PointLine();

		_instance->CreatePointLine();
	}
}
// 破棄
void PointLine::Destroy()
{
	delete _instance;
}
// 生成
HRESULT PointLine::CreatePointLine()
{
	HRESULT hr = S_OK;

	PointLine::GetInstance()->_dev = DX12::GetInstance()->GetDevice();
	PointLine::GetInstance()->_cmdList = DX12::GetInstance()->CommandList();

	// ルートシグネチャーの生成
	if (FAILED(CreateRootSignature()))
		return E_FAIL;

	// 描画パイプラインステートの生成
	if (FAILED(CreatePipelineState()))
		return E_FAIL;

	// 頂点/インデックスバッファーの生成(点)
	if (FAILED(CreatePointVertexIndexBufferView()))
		return E_FAIL;

	// 頂点/インデックスバッファーの生成(点)
	if (FAILED(CreateLineVertexIndexBufferView()))
		return E_FAIL;


	return S_OK;
}
// ルートシグネチャーの生成
HRESULT PointLine::CreateRootSignature()
{
	HRESULT hr = S_OK;

	// ディスクリプターレンジを作成
	D3D12_DESCRIPTOR_RANGE descTblRange[1] = {};
	// ディスクリプターの数[１つ]
	descTblRange[0].NumDescriptors = 1;
	// レンジの種類[CBV:コンスタントバッファービュー]
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// 先頭レジスター番号[0を選択]
	descTblRange[0].BaseShaderRegister = 0;
	// オフセットを指定[連続したディスクリプターレンジが前のディスクリプタレンジの直後にくる]
	descTblRange[0].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ルートパラメーターを作成
	D3D12_ROOT_PARAMETER rootParam = {};
	// 種別の選択[ディスクリプターテーブルを選択]
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// どのシェーダーから利用可能か[全てのシェーダー]
	rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// ディスクリプターレンジのアドレス
	rootParam.DescriptorTable.pDescriptorRanges =
		&descTblRange[0];
	// ディスクリプターレンジの数
	rootParam.DescriptorTable.NumDescriptorRanges = 1;

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

	return S_OK;
}
// 描画パイプラインステートの生成
HRESULT PointLine::CreatePipelineState()
{
	HRESULT hr = S_OK;

	// シェーダーオブジェクト読み込み用
	ID3DBlob* vsPointBlob = nullptr;
	ID3DBlob* vsLineBlob = nullptr;
	ID3DBlob* gsLineBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// 頂点シェーダーの読み込み
	hr = D3DCompileFromFile(
		L".\\Shader\\PointLine\\PointVertexShader.hlsl",	// シェーダー名
		nullptr,							// defineはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// インクルードはデフォルト
		"PointVS", "vs_5_0",				// 関数は BasicVS、対象シェーダーは vs_5_0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバック様および最適化なし	
		0,									// エフェクトコンパイルオプション[0を推奨]
		&vsPointBlob,							// 受け取るためのポインターのアドレス
		&errorBlob);						// エラー用ポインターのアドレス[エラー時はerrorBlobにメッセージが入る]
	if (FAILED(hr))
	{
		// エラーメッセージ
		BlobErrorMessange(hr, errorBlob);
		return E_FAIL;
	}
	hr = D3DCompileFromFile(
		L".\\Shader\\PointLine\\LineVertexShader.hlsl",	// シェーダー名
		nullptr,							// defineはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// インクルードはデフォルト
		"LineVS", "vs_5_0",				// 関数は BasicVS、対象シェーダーは vs_5_0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバック様および最適化なし	
		0,									// エフェクトコンパイルオプション[0を推奨]
		&vsLineBlob,							// 受け取るためのポインターのアドレス
		&errorBlob);						// エラー用ポインターのアドレス[エラー時はerrorBlobにメッセージが入る]
	if (FAILED(hr))
	{
		// エラーメッセージ
		BlobErrorMessange(hr, errorBlob);
		return E_FAIL;
	}

	// ジオメトリーシェーダーの読み込み
	hr = D3DCompileFromFile(
		L".\\Shader\\PointLine\\LineGeometryShader.hlsl",	// シェーダー名
		nullptr,							// defineはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// インクルードはデフォルト
		"LineGS", "gs_5_0",				// 関数は BasicVS、対象シェーダーは vs_5_0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバック様および最適化なし	
		0,									// エフェクトコンパイルオプション[0を推奨]
		&gsLineBlob,							// 受け取るためのポインターのアドレス
		&errorBlob);						// エラー用ポインターのアドレス[エラー時はerrorBlobにメッセージが入る]
	if (FAILED(hr))
	{
		// エラーメッセージ
		BlobErrorMessange(hr, errorBlob);
		return E_FAIL;
	}

	// ピクセルシェーダーの読み込み
	hr = D3DCompileFromFile(
		L".\\Shader\\PointLine\\PointLinePixelShader.hlsl",	// シェーダー名
		nullptr,							// defineはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// インクルードはデフォルト
		"PointLinePS", "ps_5_0",			// 関数は BasicPS、対象シェーダーは ps_5_0
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
			"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	// グラフィックスパイプラインステート(PSO:pipeline state object)を作成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipelineDesc = {};
	// ルートシグネチャー
	gpipelineDesc.pRootSignature = _rootSignature.Get();
	// 頂点シェーダーのアドレスとサイズ
	gpipelineDesc.VS.pShaderBytecode = vsPointBlob->GetBufferPointer();
	gpipelineDesc.VS.BytecodeLength = vsPointBlob->GetBufferSize();
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

	// レンダーターゲットブレンドの設定
	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	// ブレンドの有無[なし]
	renderTargetBlendDesc.BlendEnable = FALSE;
	// 論理演算の有無[なし]
	renderTargetBlendDesc.LogicOpEnable = FALSE;
	// RGBAそれぞれの値を書き込むか指定[全て]
	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	// レンダーターゲットブレンドをPSOに紐づける
	gpipelineDesc.BlendState.RenderTarget[0] = renderTargetBlendDesc;

	// 入力レイアウトの設定
	// 頂点入力レイアウトをPSOに紐づける
	gpipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	// 入力レイアウトのサイズを指定[レイアウト配列の要素数]
	gpipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	// インデックスバッファー利用時、ストリップカット値設定[カットなし]
	gpipelineDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

	// プリミティブのタイプ[点]
	gpipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

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

	// 深度バッファーの有効の有無[有効]
	gpipelineDesc.DepthStencilState.DepthEnable = true;
	// 深度ステンシルバッファーの書き込み方法[書き込み有効]
	gpipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 深度ステンシルバッファーの制御方法[小さいほうを採用]
	gpipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	// フォーマットを指定
	gpipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// グラフィックパイプラインステートオブジェクトを生成
	hr = _dev->CreateGraphicsPipelineState(
		&gpipelineDesc, IID_PPV_ARGS(_pointPipelineState.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// 頂点シェーダーのアドレスとサイズ
	gpipelineDesc.VS.pShaderBytecode = vsLineBlob->GetBufferPointer();
	gpipelineDesc.VS.BytecodeLength = vsLineBlob->GetBufferSize();
	// ジオメトリーシェーダーのアドレスとサイズ
	gpipelineDesc.GS.pShaderBytecode = gsLineBlob->GetBufferPointer();
	gpipelineDesc.GS.BytecodeLength = gsLineBlob->GetBufferSize();

	// プリミティブのタイプ[線]
	gpipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	// グラフィックパイプラインステートオブジェクトを生成
	hr = _dev->CreateGraphicsPipelineState(
		&gpipelineDesc, IID_PPV_ARGS(_linePipelineState.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	return S_OK;
}
// 頂点/インデックスバッファーの生成(点)
HRESULT PointLine::CreatePointVertexIndexBufferView()
{
	HRESULT hr = S_OK;

	// 頂点データ構造体
	struct Vertex
	{
		XMFLOAT4 pos;	// xyz:座標
	};

	Vertex vertices[] =
	{
		{{ 0.0f, 0.0f, 0.0f, 0.0f}},
	};

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
	resDesc.Width = sizeof(vertices);
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
		IID_PPV_ARGS(_pointVertexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// 頂点情報のコピー
	Vertex* vertexMap = nullptr;
	// 頂点バッファーのロック
	hr = _pointVertexBuffer->Map(0, nullptr,
		(void**)&vertexMap);
	if (hr != S_OK)
		return hr;
	std::copy(std::begin(vertices), std::end(vertices),
		vertexMap);
	// 頂点バッファーの解除
	_pointVertexBuffer->Unmap(0, nullptr);

	// 頂点バッファービューを作成
	// 頂点バッファーのアドレス[バッファーの仮想アドレス]
	_pointVertexBufferView.BufferLocation = _pointVertexBuffer->GetGPUVirtualAddress();
	// 総バイト数[全バイト数]
	_pointVertexBufferView.SizeInBytes = sizeof(vertices);
	// 1頂点あたりのバイト数
	_pointVertexBufferView.StrideInBytes = sizeof(vertices[0]);

	// インデックス情報を作成
	unsigned short indices[] =
	{
		0,
	};

	// サイズ幅[幅で全部まかなうのでsizeof(全インデックス)とする]
	resDesc.Width = sizeof(indices);

	// リソースオブジェクトを作成
	hr = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_pointIndexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// インデックス情報のコピー
	unsigned short* indexMap = nullptr;
	// インデックスバッファーのロック
	hr = _pointIndexBuffer->Map(0, nullptr,
		(void**)&indexMap);
	if (hr != S_OK)
		return hr;
	std::copy(std::begin(indices), std::end(indices),
		indexMap);
	// インデックスバッファーの解除
	_pointIndexBuffer->Unmap(0, nullptr);

	// インデックスバッファービューを作成
	// インデックスバッファーのアドレス[バッファーの仮想アドレス]
	_pointIndexBufferView.BufferLocation = _pointIndexBuffer->GetGPUVirtualAddress();
	// 総バイト数[全バイト数]
	_pointIndexBufferView.SizeInBytes = sizeof(indices);
	// フォーマット
	_pointIndexBufferView.Format = DXGI_FORMAT_R16_UINT;

	return S_OK;
}
// 頂点/インデックスバッファーの生成(線)
HRESULT PointLine::CreateLineVertexIndexBufferView()
{
	HRESULT hr = S_OK;

	// 頂点データ構造体
	struct Vertex
	{
		XMFLOAT4 pos;	// xyz:座標
	};

	Vertex vertices[] =
	{
		{{ 0.0f, 0.0f, 0.0f, 0.0f}},
		{{ 0.0f, 0.0f, 0.0f, 0.0f}},
	};

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
	resDesc.Width = sizeof(vertices);
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
		IID_PPV_ARGS(_lineVertexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// 頂点情報のコピー
	Vertex* vertexMap = nullptr;
	// 頂点バッファーのロック
	hr = _lineVertexBuffer->Map(0, nullptr,
		(void**)&vertexMap);
	if (hr != S_OK)
		return hr;
	std::copy(std::begin(vertices), std::end(vertices),
		vertexMap);
	// 頂点バッファーの解除
	_lineVertexBuffer->Unmap(0, nullptr);

	// 頂点バッファービューを作成
	// 頂点バッファーのアドレス[バッファーの仮想アドレス]
	_lineVertexBufferView.BufferLocation = _lineVertexBuffer->GetGPUVirtualAddress();
	// 総バイト数[全バイト数]
	_lineVertexBufferView.SizeInBytes = sizeof(vertices);
	// 1頂点あたりのバイト数
	_lineVertexBufferView.StrideInBytes = sizeof(vertices[0]);

	// インデックス情報を作成
	unsigned short indices[] =
	{
		0, 1,
	};

	// サイズ幅[幅で全部まかなうのでsizeof(全インデックス)とする]
	resDesc.Width = sizeof(indices);

	// リソースオブジェクトを作成
	hr = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_lineIndexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// インデックス情報のコピー
	unsigned short* indexMap = nullptr;
	// インデックスバッファーのロック
	hr = _lineIndexBuffer->Map(0, nullptr,
		(void**)&indexMap);
	if (hr != S_OK)
		return hr;
	std::copy(std::begin(indices), std::end(indices),
		indexMap);
	// インデックスバッファーの解除
	_lineIndexBuffer->Unmap(0, nullptr);

	// インデックスバッファービューを作成
	// インデックスバッファーのアドレス[バッファーの仮想アドレス]
	_lineIndexBufferView.BufferLocation = _lineIndexBuffer->GetGPUVirtualAddress();
	// 総バイト数[全バイト数]
	_lineIndexBufferView.SizeInBytes = sizeof(indices);
	// フォーマット
	_lineIndexBufferView.Format = DXGI_FORMAT_R16_UINT;

	return S_OK;
}

// コンスタントバッファービューの生成(点)
HRESULT PointLine::CreatePointConstantBufferView(MESH* mesh)
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
	resDesc.Width = (sizeof(ConstantBuffer) + 0xff) & ~0xff;
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
	// デスクリプタを指定[CBV:１つ]
	heapDesc.NumDescriptors = 1;
	// 特に指定なし[シェーダー側から見える必要あり]
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// デスクリプタヒープを作成
	hr = _dev->CreateDescriptorHeap(&heapDesc
		, IID_PPV_ARGS(mesh->basicDescHeap.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// ディスクリプタの先頭ハンドルを取得しておく
	auto basicHeapHandle = mesh->basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = mesh->constBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)mesh->constBuffer->GetDesc().Width;

	// 定数バッファービューの作成
	_dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	return S_OK;
}
// コンスタントバッファービューの生成(線)
HRESULT PointLine::CreateLineConstantBufferView(MESH* mesh)
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
	resDesc.Width = (sizeof(ConstantBuffer) + 0xff) & ~0xff;
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
	// デスクリプタを指定[CBV:１つ]
	heapDesc.NumDescriptors = 1;
	// 特に指定なし[シェーダー側から見える必要あり]
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// デスクリプタヒープを作成
	hr = _dev->CreateDescriptorHeap(&heapDesc
		, IID_PPV_ARGS(mesh->basicDescHeap.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// ディスクリプタの先頭ハンドルを取得しておく
	auto basicHeapHandle = mesh->basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = mesh->constBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)mesh->constBuffer->GetDesc().Width;

	// 定数バッファービューの作成
	_dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	return S_OK;
}
// 描画(点)
void PointLine::DrawPoint()
{
	if (_pointInstanceEntry == 0) {
		return;
	}
	for (int m = 0, size = (int)_pointMeshs.size(); m < size; ++m)
	{
		// ビュー行列
		XMMATRIX viewMat = Camera::GetInstance()->GetView();

		// プロジェクション行列
		XMMATRIX projectionMat = Camera::GetInstance()->GetProjection();

		// 定数情報の更新

		int EntryNum = _pointInstanceEntry - m * 1000;
		if (EntryNum >= instanceMax)
		{
			EntryNum = instanceMax;
		}

		ConstantBuffer* mapMetaData = nullptr;	// マップ先を示すポインター
		_pointMeshs[m]->constBuffer->Map(0, nullptr, (void**)&mapMetaData);	// マップ
		XMStoreFloat4x4(&mapMetaData->viewMat, viewMat);
		XMStoreFloat4x4(&mapMetaData->projectionMat, projectionMat);
		for (int i = 0; i < EntryNum; ++i)
		{
			mapMetaData->startPos[i] = _pointMetaData[m].startPos[i];
			mapMetaData->endPos[i] = _pointMetaData[m].endPos[i];
			mapMetaData->color[i] = _pointMetaData[m].color[i];
		}
		_pointMeshs[m]->constBuffer->Unmap(0, nullptr);		// アンマップ

		// グラフィックスパイプラインステートの指定
		_cmdList->SetPipelineState(_pointPipelineState.Get());
		// ルートシグネチャーの指定
		_cmdList->SetGraphicsRootSignature(_rootSignature.Get());

		// ディスクリプターヒープの指定
		_cmdList->SetDescriptorHeaps(1, _pointMeshs[m]->basicDescHeap.GetAddressOf());

		// ルートパラメーターとディスクリプターヒープの関連付け
		_cmdList->SetGraphicsRootDescriptorTable(
			0,	// ルートパラメーターインデックス
			_pointMeshs[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart());

		// 頂点情報の指定[線]
		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		// 頂点バッファーの指定
		_cmdList->IASetVertexBuffers(0, 1, &_pointMeshs[m]->vertexBufferView);

		// インデックスバッファーの指定
		_cmdList->IASetIndexBuffer(&_pointMeshs[m]->indexBufferView);

		// 描画命令
		_cmdList->DrawIndexedInstanced(2, EntryNum, 0, 0, 0);
	}
}
// 描画(線)
void PointLine::DrawLine()
{
	if (_lineInstanceEntry == 0) {
		return;
	}
	for (int m = 0, size = (int)_lineMeshs.size(); m < size; ++m)
	{
		// ビュー行列
		XMMATRIX viewMat = Camera::GetInstance()->GetView();

		// プロジェクション行列
		XMMATRIX projectionMat = Camera::GetInstance()->GetProjection();

		// 定数情報の更新
		int EntryNum = _lineInstanceEntry - m * 1000;
		if (EntryNum >= instanceMax)
		{
			EntryNum = instanceMax;
		}

		ConstantBuffer* mapMetaData = nullptr;	// マップ先を示すポインター
		_lineMeshs[m]->constBuffer->Map(0, nullptr, (void**)&mapMetaData);	// マップ
		XMStoreFloat4x4(&mapMetaData->viewMat, viewMat);
		XMStoreFloat4x4(&mapMetaData->projectionMat, projectionMat);
		for (int i = 0; i < EntryNum; ++i)
		{
			mapMetaData->startPos[i] = _lineMetaData[m].startPos[i];
			mapMetaData->endPos[i] = _lineMetaData[m].endPos[i];
			mapMetaData->color[i] = _lineMetaData[m].color[i];
		}
		_lineMeshs[m]->constBuffer->Unmap(0, nullptr);		// アンマップ

		// グラフィックスパイプラインステートの指定
		_cmdList->SetPipelineState(_linePipelineState.Get());
		// ルートシグネチャーの指定
		_cmdList->SetGraphicsRootSignature(_rootSignature.Get());

		// ディスクリプターヒープの指定
		_cmdList->SetDescriptorHeaps(1, _lineMeshs[m]->basicDescHeap.GetAddressOf());

		// ルートパラメーターとディスクリプターヒープの関連付け
		_cmdList->SetGraphicsRootDescriptorTable(
			0,	// ルートパラメーターインデックス
			_lineMeshs[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart());

		// 頂点情報の指定[線]
		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		// 頂点バッファーの指定
		_cmdList->IASetVertexBuffers(0, 1, &_lineMeshs[m]->vertexBufferView);

		// インデックスバッファーの指定
		_cmdList->IASetIndexBuffer(&_lineMeshs[m]->indexBufferView);

		// 描画命令
		_cmdList->DrawIndexedInstanced(2, EntryNum, 0, 0, 0);
	}
}

// 点
// 指定した番号を削除
void PointLine::PointDeleate(int& entryPos) {
	if (entryPos != -1)
	{
		int EntryPos = entryPos % instanceMax;
		int ArrayNum = entryPos / instanceMax;
		_pointDataCheck[ArrayNum]._dataCheck[EntryPos] = false;
		// 使ってないEntryPosは一時的に飛ばす
		_pointMetaData[ArrayNum].startPos[EntryPos].x = 0;
		_pointMetaData[ArrayNum].startPos[EntryPos].y = -100;
		_pointMetaData[ArrayNum].startPos[EntryPos].z = 0;
		_pointMetaData[ArrayNum].endPos[EntryPos].x = 0;
		_pointMetaData[ArrayNum].endPos[EntryPos].y = -100;
		_pointMetaData[ArrayNum].endPos[EntryPos].z = 0;
		entryPos = -1;
	}
}

void PointLine::SetPointMetaData(int& entryPos,
	VECTOR pos, VECTOR color) {
	// 最初もしくは1000番目まで使い切ったら次のバッファーを用意する
	int num = _pointInstanceEntry % instanceMax;
	if (num == 0 && entryPos == -1)
	{
		_pointMeshs.push_back(new MESH);

		_pointMeshs.back()->vertexBuffer = _pointVertexBuffer;
		_pointMeshs.back()->vertexBufferView = _pointVertexBufferView;
		_pointMeshs.back()->indexBuffer = _pointIndexBuffer;
		_pointMeshs.back()->indexBufferView = _pointIndexBufferView;
		// コンスタントバッファービューの生成
		CreateLineConstantBufferView(_pointMeshs.back());
		_pointMetaData.resize(_pointMeshs.size());
		_pointDataCheck.resize(_pointMeshs.size());
	}
	// エントリー番号
	int EntryPos;
	// 使用するメッシュの番号
	int ArrayNum;
	if (entryPos == -1)
	{
		// 空いてるエントリー番号を探す
		for (int i = 0; i < _pointDataCheck.size(); i++)
		{
			for (int j = 0; j < instanceMax; j++)
			{
				if (_pointDataCheck[i]._dataCheck[j] == false)
				{
					entryPos = i * instanceMax + j;
					if ((UINT)entryPos >= _pointInstanceEntry)
					{
						_pointInstanceEntry = entryPos + 1;
					}
					_pointDataCheck[i]._dataCheck[j] = true;
					break;
				}
			}
		}

		EntryPos = entryPos % instanceMax;
		ArrayNum = entryPos / instanceMax;

		// 生成するライン情報代入
		_pointMetaData[ArrayNum].startPos[EntryPos].x = pos.x;
		_pointMetaData[ArrayNum].startPos[EntryPos].y = pos.y;
		_pointMetaData[ArrayNum].startPos[EntryPos].z = pos.z;
		_pointMetaData[ArrayNum].endPos[EntryPos].x = pos.x;
		_pointMetaData[ArrayNum].endPos[EntryPos].y = pos.y;
		_pointMetaData[ArrayNum].endPos[EntryPos].z = pos.z;
		_pointMetaData[ArrayNum].color[EntryPos].x = color.x;
		_pointMetaData[ArrayNum].color[EntryPos].y = color.y;
		_pointMetaData[ArrayNum].color[EntryPos].z = color.z;
	}
	else
	{
		EntryPos = entryPos % instanceMax;
		ArrayNum = entryPos / instanceMax;
		// 生成するライン情報代入
		_pointMetaData[ArrayNum].startPos[EntryPos].x = pos.x;
		_pointMetaData[ArrayNum].startPos[EntryPos].y = pos.y;
		_pointMetaData[ArrayNum].startPos[EntryPos].z = pos.z;
		_pointMetaData[ArrayNum].endPos[EntryPos].x = pos.x;
		_pointMetaData[ArrayNum].endPos[EntryPos].y = pos.y;
		_pointMetaData[ArrayNum].endPos[EntryPos].z = pos.z;
		_pointMetaData[ArrayNum].color[EntryPos].x = color.x;
		_pointMetaData[ArrayNum].color[EntryPos].y = color.y;
		_pointMetaData[ArrayNum].color[EntryPos].z = color.z;
	}
}

// 線
// 指定した番号を削除
void PointLine::LineDeleate(int& entryPos) {
	if (entryPos != -1)
	{
		int EntryPos = entryPos % instanceMax;
		int ArrayNum = entryPos / instanceMax;
		_lineDataCheck[ArrayNum]._dataCheck[EntryPos] = false;
		// 使ってないEntryPosは一時的に飛ばす
		_lineMetaData[ArrayNum].startPos[EntryPos].x = 0;
		_lineMetaData[ArrayNum].startPos[EntryPos].y = -100;
		_lineMetaData[ArrayNum].startPos[EntryPos].z = 0;
		_lineMetaData[ArrayNum].endPos[EntryPos].x = 0;
		_lineMetaData[ArrayNum].endPos[EntryPos].y = -100;
		_lineMetaData[ArrayNum].endPos[EntryPos].z = 0;
		entryPos = -1;
	}
}

void PointLine::SetLineMetaData(int& entryPos,
	VECTOR startPos, VECTOR endPos, VECTOR color) {
	// 最初もしくは1000番目まで使い切ったら次のバッファーを用意する
	int num = _lineInstanceEntry % instanceMax;
	if (num == 0 && entryPos == -1)
	{
		_lineMeshs.push_back(new MESH);

		_lineMeshs.back()->vertexBuffer = _lineVertexBuffer;
		_lineMeshs.back()->vertexBufferView = _lineVertexBufferView;
		_lineMeshs.back()->indexBuffer = _lineIndexBuffer;
		_lineMeshs.back()->indexBufferView = _lineIndexBufferView;
		// コンスタントバッファービューの生成
		CreateLineConstantBufferView(_lineMeshs.back());
		_lineMetaData.resize(_lineMeshs.size());
		_lineDataCheck.resize(_lineMeshs.size());
	}
	// エントリー番号
	int EntryPos;
	// 使用するメッシュの番号
	int ArrayNum;
	if (entryPos == -1)
	{
		// 空いてるエントリー番号を探す
		for (int i = 0; i < _lineDataCheck.size(); i++)
		{
			for (int j = 0; j < instanceMax; j++)
			{
				if (_lineDataCheck[i]._dataCheck[j] == false)
				{
					entryPos = i * instanceMax + j;
					if ((UINT)entryPos >= _lineInstanceEntry)
					{
						_lineInstanceEntry = entryPos + 1;
					}
					_lineDataCheck[i]._dataCheck[j] = true;
					break;
				}
			}
		}

		EntryPos = entryPos % instanceMax;
		ArrayNum = entryPos / instanceMax;

		// 生成するライン情報代入
		_lineMetaData[ArrayNum].startPos[EntryPos].x = startPos.x;
		_lineMetaData[ArrayNum].startPos[EntryPos].y = startPos.y;
		_lineMetaData[ArrayNum].startPos[EntryPos].z = startPos.z;
		_lineMetaData[ArrayNum].endPos[EntryPos].x = endPos.x;
		_lineMetaData[ArrayNum].endPos[EntryPos].y = endPos.y;
		_lineMetaData[ArrayNum].endPos[EntryPos].z = endPos.z;
		_lineMetaData[ArrayNum].color[EntryPos].x = color.x;
		_lineMetaData[ArrayNum].color[EntryPos].y = color.y;
		_lineMetaData[ArrayNum].color[EntryPos].z = color.z;
	}
	else
	{
		EntryPos = entryPos % instanceMax;
		ArrayNum = entryPos / instanceMax;
		// 生成するライン情報代入
		_lineMetaData[ArrayNum].startPos[EntryPos].x = startPos.x;
		_lineMetaData[ArrayNum].startPos[EntryPos].y = startPos.y;
		_lineMetaData[ArrayNum].startPos[EntryPos].z = startPos.z;
		_lineMetaData[ArrayNum].endPos[EntryPos].x = endPos.x;
		_lineMetaData[ArrayNum].endPos[EntryPos].y = endPos.y;
		_lineMetaData[ArrayNum].endPos[EntryPos].z = endPos.z;
		_lineMetaData[ArrayNum].color[EntryPos].x = color.x;
		_lineMetaData[ArrayNum].color[EntryPos].y = color.y;
		_lineMetaData[ArrayNum].color[EntryPos].z = color.z;
	}
}
