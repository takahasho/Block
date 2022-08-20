// --------------------- Include File --------------------
#include "PmdModel.h"
#include "../Camera/Camera.h"
#include "../DX12System.h"
PmdModel* PmdModel::_instance = nullptr;			// インスタンスの実態
// コンストラクター
PmdModel::PmdModel()
{

}
// デストラクター
PmdModel::~PmdModel()
{

}
// 生成
void PmdModel::Create()
{
	if (!_instance)
	{
		_instance = new PmdModel();

		// ルートシグネチャーの生成
		_instance->CreateRootSignature();

		// 描画パイプラインの生成
		_instance->CreateGraphicsPipelineState();

		// 白色テクスチャーの生成
		_instance->CreateWhiteTexture();

		// 黒色テクスチャーの生成
		_instance->CreateBlackTexture();
	}
}
// 破棄
void PmdModel::Destroy()
{
	delete _instance;
}
// ルートシグネチャーの生成
HRESULT PmdModel::CreateRootSignature()
{
	HRESULT hr = S_OK;

	// ディスクリプターレンジを作成
	D3D12_DESCRIPTOR_RANGE descTblRange[3] = {};

	// 定数バッファー(座標)
	// ディスクリプターの数[１つ]
	descTblRange[0].NumDescriptors = 1;
	// レンジの種類[CBV:コンスタントバッファービュー]
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// 先頭レジスター番号[0を選択]
	descTblRange[0].BaseShaderRegister = 0;
	// オフセットを指定[連続したディスクリプターレンジが前のディスクリプタレンジの直後にくる]
	descTblRange[0].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// 定数バッファー(マテリアル)
	// ディスクリプターの数[１つ]
	descTblRange[1].NumDescriptors = 1;
	// レンジの種類[CBV:コンスタントバッファービュー]
	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// 先頭レジスター番号[1を選択]
	descTblRange[1].BaseShaderRegister = 1;
	// オフセットを指定[連続したディスクリプターレンジが前のディスクリプタレンジの直後にくる]
	descTblRange[1].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// テクスチャーバッファービュー
	// ディスクリプターの数[3つ]
	descTblRange[2].NumDescriptors = 3;
	// レンジの種類[SRV:シェーダーリソースビュー]
	descTblRange[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	// 先頭レジスター番号[0と1を選択]
	descTblRange[2].BaseShaderRegister = 0;
	// オフセットを指定[連続したディスクリプターレンジが前のディスクリプタレンジの直後にくる]
	descTblRange[2].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	// ルートパラメーターを作成
	D3D12_ROOT_PARAMETER rootParam[2] = {};
	// 種別の選択[ディスクリプターテーブルを選択]
	rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// どのシェーダーから利用可能か[全シェーダー]
	rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// ディスクリプターレンジのアドレス
	rootParam[0].DescriptorTable.pDescriptorRanges = &descTblRange[0];
	// ディスクリプターレンジの数
	rootParam[0].DescriptorTable.NumDescriptorRanges = 1;
	// 種別の選択[ディスクリプターテーブルを選択]
	rootParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// どのシェーダーから利用可能か[全シェーダー]
	rootParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// ディスクリプターレンジのアドレス
	rootParam[1].DescriptorTable.pDescriptorRanges = &descTblRange[1];
	// ディスクリプターレンジの数
	rootParam[1].DescriptorTable.NumDescriptorRanges = 2;

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
	rootSignatureDesc.pParameters = rootParam;
	// パラメーターの数
	rootSignatureDesc.NumParameters = 2;
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
	hr = DX12::GetInstance()->GetDevice()->CreateRootSignature(
		0,	// nodemask。０でよい
		rootSigBlob->GetBufferPointer(),	// アドレス
		rootSigBlob->GetBufferSize(),		// サイズ
		IID_PPV_ARGS(_rootSignature.GetAddressOf()));	// アドレスを措定
	if (hr != S_OK)
		return E_FAIL;

	return S_OK;
}

// 描画パイプラインの生成
HRESULT PmdModel::CreateGraphicsPipelineState()
{
	HRESULT hr = S_OK;

	// シェーダーオブジェクト読み込み用
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// 頂点シェーダーの読み込み
	hr = D3DCompileFromFile(
		L".\\Shader\\Pmd\\PmdVertexShader.hlsl",			// シェーダー名
		nullptr,							// defineはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// インクルードはデフォルト
		"PmdVS", "vs_5_0",				// 関数は PmdVS、対象シェーダーは vs_5_0
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
		L".\\Shader\\Pmd\\PmdPixelShader.hlsl",			// シェーダー名
		nullptr,							// defineはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// インクルードはデフォルト
		"PmdPS", "ps_5_0",				// 関数は PmdPS、対象シェーダーは ps_5_0
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
		{	// normal情報
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{	// uv情報
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{	// ボーン番号情報
			"BONE_NO", 0, DXGI_FORMAT_R16G16_UINT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{	// ボーンウェイト情報
			"WEIGHT", 0, DXGI_FORMAT_R8_UINT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{	// エッジ情報
			"EDGE_FLG", 0, DXGI_FORMAT_R8_UINT, 0,
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

	// 深度ステンシルバッファーの有効の有無[有効]
	gpipelineDesc.DepthStencilState.DepthEnable = true;
	// 深度ステンシルバッファーの書き込み方法[書き込み有効]
	gpipelineDesc.DepthStencilState.DepthWriteMask =
		D3D12_DEPTH_WRITE_MASK_ALL;
	// 深度ステンシルバッファーの制御方法[小さい方を採用]
	gpipelineDesc.DepthStencilState.DepthFunc =
		D3D12_COMPARISON_FUNC_LESS;
	// フォーマットを指定
	gpipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// グラフィックパイプラインステートオブジェクトを生成
	hr = DX12::GetInstance()->GetDevice()->CreateGraphicsPipelineState(
		&gpipelineDesc, IID_PPV_ARGS(_pipelineState.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	return S_OK;
}
// 頂点/インデックスバッファーの生成
HRESULT PmdModel::CreateVertexIndexBufferView(PMDMODEL& model)
{
	HRESULT hr = S_OK;
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
	resDesc.Width = model.mesh.vertices.size();
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
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(model.mesh.vertexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// 頂点情報のコピー
	unsigned char* vertexMap = nullptr;
	// 頂点バッファーのロック
	hr = model.mesh.vertexBuffer->Map(0, nullptr,
		(void**)&vertexMap);
	if (hr != S_OK)
		return hr;
	std::copy(std::begin(model.mesh.vertices), std::end(model.mesh.vertices),
		vertexMap);
	// 頂点バッファーの解除
	model.mesh.vertexBuffer->Unmap(0, nullptr);

	// 頂点バッファービューを作成
	// 頂点バッファーのアドレス[バッファーの仮想アドレス]
	model.mesh.vertexBufferView.BufferLocation = model.mesh.vertexBuffer->GetGPUVirtualAddress();
	// 総バイト数[全バイト数]
	model.mesh.vertexBufferView.SizeInBytes = (UINT)model.mesh.vertices.size();
	// 1頂点あたりのバイト数
	model.mesh.vertexBufferView.StrideInBytes = _pmdVertex_size;

	// サイズ幅[幅で全部まかなうのでsizeof(全インデックス)とする]
	resDesc.Width = model.mesh.indices.size() * sizeof(model.mesh.indices[0]);

	// リソースオブジェクトを作成
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(model.mesh.indexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// インデックス情報のコピー
	unsigned short* indexMap = nullptr;
	// インデックスバッファーのロック
	hr = model.mesh.indexBuffer->Map(0, nullptr,
		(void**)&indexMap);
	if (hr != S_OK)
		return hr;
	std::copy(std::begin(model.mesh.indices), std::end(model.mesh.indices),
		indexMap);
	// インデックスバッファーの解除
	model.mesh.indexBuffer->Unmap(0, nullptr);

	// インデックスバッファービューを作成
	// インデックスバッファーのアドレス[バッファーの仮想アドレス]
	model.mesh.indexBufferView.BufferLocation = model.mesh.indexBuffer->GetGPUVirtualAddress();
	// 総バイト数[全バイト数]
	model.mesh.indexBufferView.SizeInBytes = (UINT)model.mesh.indices.size() * sizeof(model.mesh.indices[0]);
	// フォーマット
	model.mesh.indexBufferView.Format = DXGI_FORMAT_R16_UINT;

	return S_OK;
}
// シェーダーリソースの生成
HRESULT PmdModel::CreateShaderResource(
	ComPtr<ID3D12Resource>* texBuffer,
	const std::string fileName)
{
	HRESULT hr = S_OK;

	auto it = _resourceTable.find(fileName);
	if (it != _resourceTable.end())
	{
		// テーブル内にあったらロードするのではなく
		// マップ内のリソースを返す
		texBuffer = it->second;
		return S_OK;
	}
	// 画像サイズの取得とイメージデーター確保
	BYTE* pImageData = nullptr;
	SIZE size = {};

	// 画像情報の読み込み
	if (FAILED(LoadTextureFromFile(fileName.c_str(),
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
	resDesc.Width = (UINT64)(
		AlignmentSize(size.cx, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * AlignmentSize(size.cy, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * bytePerPixel);
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
	resDesc.Width = (UINT64)AlignmentSize(size.cx, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
	// サイズ高さ
	resDesc.Height = (UINT)AlignmentSize(size.cy, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
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
		IID_PPV_ARGS(texBuffer->ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// アップロードリソースへのマップ
	uint8_t* mapForImg = nullptr;	// image->pixelsと同じ型にする
	hr = _uploadBuffer->Map(0, nullptr, (void**)&mapForImg);	// マップ
	if (FAILED(hr))
		return hr;
	std::copy_n(pImageData, AlignmentSize(size.cx, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * AlignmentSize(size.cy, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * bytePerPixel, mapForImg);	// コピー
	_uploadBuffer->Unmap(0, nullptr);	// アンマップ

	// イメージ情報の解放
	delete[] pImageData;

	// コピーテクスチャーレギオンの設定
	// コピー元(アップロード側)設定
	D3D12_TEXTURE_COPY_LOCATION src = {};
	src.pResource = _uploadBuffer.Get();	// 中間バッファー
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;	// プットプリント指定[バッファーリソース]
	src.PlacedFootprint.Offset = 0;							// オフセット
	src.PlacedFootprint.Footprint.Width = (UINT)AlignmentSize(size.cx, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);		// 幅
	src.PlacedFootprint.Footprint.Height = (UINT)AlignmentSize(size.cy, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);	// 高さ
	src.PlacedFootprint.Footprint.Depth = 1;				// 深さ
	src.PlacedFootprint.Footprint.RowPitch =
		(UINT)AlignmentSize(size.cx, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * bytePerPixel;// 1行あたりのバイト数
	src.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// フォーマット

	// コピー先設定
	D3D12_TEXTURE_COPY_LOCATION dst = {};
	dst.pResource = texBuffer->Get();
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
	BarrierDesc.Transition.pResource = texBuffer->Get();				// テクスチャーリソース
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

	_resourceTable[fileName] = texBuffer;

	return S_OK;
}

// コンスタントバッファービューの生成
HRESULT PmdModel::CreateConstantBufferView(PMDMODEL& model)
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
	resDesc.Width = (sizeof(SceneMatrix) + 0xff) & ~0xff;
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
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(model.mesh.constBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// デスクリプタヒープを作成
	// ヒープの記述を用意
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	// 種類の選択[CBVを利用]
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	// GPUを１つだけ使用する想定
	heapDesc.NodeMask = 0;
	// デスクリプタを指定[CBV１つ + マテリアルの数(質感+テクスチャー+スフィアマップ) + スフィアマップ(加算)]
	heapDesc.NumDescriptors = 1 + model.mesh.materialNum * 4;
	// 特に指定なし[シェーダー側から見える必要あり]
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// デスクリプタヒープを作成
	hr = DX12::GetInstance()->GetDevice()->CreateDescriptorHeap(&heapDesc
		, IID_PPV_ARGS(model.mesh.basicDescHeap.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	// ディスクリプタの先頭ハンドルを取得しておく
	auto basicHeapHandle = model.mesh.basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = model.mesh.constBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)model.mesh.constBuffer->GetDesc().Width;

	// 定数バッファービューの作成(座標と透視投影変換)
	DX12::GetInstance()->GetDevice()->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	// 次の場所に移動
	basicHeapHandle.ptr +=
		DX12::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize
		(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	auto inc = DX12::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize
	(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 定数バッファービューの作成(マテリアル情報)
	cbvDesc.BufferLocation = model.mesh.materialBuffer->GetGPUVirtualAddress();
	auto materialBufferSize = sizeof(MaterialForHlsl);
	materialBufferSize = (materialBufferSize + 0xff) & ~0xff;
	cbvDesc.SizeInBytes = (UINT)materialBufferSize;// マテリアルの256アライメントサイズ
	for (int i = 0; i < (int)model.mesh.materialNum; ++i)
	{
		// マテリアル
		DX12::GetInstance()->GetDevice()->CreateConstantBufferView(
			&cbvDesc, basicHeapHandle);
		cbvDesc.BufferLocation += materialBufferSize;
		// 次の場所に移動
		basicHeapHandle.ptr += inc;

		// シェーダーリソースビュー作成
		// テクスチャー
		if (model.mesh.texBuffer[i])
		{
			DX12::GetInstance()->GetDevice()->CreateShaderResourceView(
				model.mesh.texBuffer[i].Get(),
				&srvDesc,
				basicHeapHandle
			);
		}
		else
		{
			DX12::GetInstance()->GetDevice()->CreateShaderResourceView(
				_texWhiteBuffer.Get(),
				&srvDesc,
				basicHeapHandle
			);
		}

		// 次の場所に移動
		basicHeapHandle.ptr += inc;

		// スフィアマップ( 乗算)
		if (model.mesh.sphTexBuffer[i])
		{
			DX12::GetInstance()->GetDevice()->CreateShaderResourceView(
				model.mesh.sphTexBuffer[i].Get(),
				&srvDesc,
				basicHeapHandle
			);
		}
		else
		{
			DX12::GetInstance()->GetDevice()->CreateShaderResourceView(
				_texWhiteBuffer.Get(),
				&srvDesc,
				basicHeapHandle
			);
		}

		// 次の場所に移動
		basicHeapHandle.ptr += inc;

		// スフィアマップ( 乗算)
		if (model.mesh.spaTexBuffer[i])
		{
			DX12::GetInstance()->GetDevice()->CreateShaderResourceView(
				model.mesh.spaTexBuffer[i].Get(),
				&srvDesc,
				basicHeapHandle
			);
		}
		else
		{
			DX12::GetInstance()->GetDevice()->CreateShaderResourceView(
				_texBlackBuffer.Get(),
				&srvDesc,
				basicHeapHandle
			);
		}

		// 次の場所に移動
		basicHeapHandle.ptr += inc;
	}
	return S_OK;
}
// 描画
void PmdModel::Draw(PMDMODEL model)
{
	XMMATRIX world_matrix;
	XMMATRIX translate = XMMatrixTranslation(model.position.x, model.position.y, model.position.z);
	XMMATRIX rotate_x = XMMatrixRotationX(DirectX::XMConvertToRadians(model.angle.x));
	XMMATRIX rotate_y = XMMatrixRotationY(DirectX::XMConvertToRadians(model.angle.y));
	XMMATRIX rotate_z = XMMatrixRotationZ(DirectX::XMConvertToRadians(model.angle.z));
	XMMATRIX scale_mat = XMMatrixScaling(model.size.x, model.size.y, model.size.z);
	world_matrix = scale_mat * rotate_x * rotate_y * rotate_z * translate;

	// =====ビュー行列=====
	XMMATRIX viewMat = Camera::GetInstance()->GetView();
	XMFLOAT3 eyePos = Camera::GetInstance()->GetEyePosition();
	// =====プロジェクション行列=====
	XMMATRIX projMat = Camera::GetInstance()->GetProjection();

	// 定数情報の更新
	SceneMatrix* mapMatrix = nullptr;	// マップ先を示すポインター
	model.mesh.constBuffer->Map(0, nullptr, (void**)&mapMatrix);	// マップ
	mapMatrix->world = world_matrix;
	mapMatrix->view = viewMat;
	mapMatrix->Projection = projMat;
	mapMatrix->eye = eyePos;
	mapMatrix->LightVector = Camera::GetInstance()->GetLightVector();
	model.mesh.constBuffer->Unmap(0, nullptr);		// アンマップ

	// グラフィックスパイプラインステートの指定
	DX12::GetInstance()->CommandList()->SetPipelineState(_pipelineState.Get());
	// ルートシグネチャーの指定
	DX12::GetInstance()->CommandList()->SetGraphicsRootSignature(_rootSignature.Get());

	// ディスクリプターヒープの指定
	DX12::GetInstance()->CommandList()->SetDescriptorHeaps(1, model.mesh.basicDescHeap.GetAddressOf());

	// ルートパラメーターとディスクリプターヒープの関連付け
	DX12::GetInstance()->CommandList()->SetGraphicsRootDescriptorTable(
		0,	// ルートパラメーターインデックス
		model.mesh.basicDescHeap->GetGPUDescriptorHandleForHeapStart());

	// 頂点情報の指定[三角形]
	DX12::GetInstance()->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点バッファーの指定
	DX12::GetInstance()->CommandList()->IASetVertexBuffers(0, 1, &model.mesh.vertexBufferView);

	// インデックスバッファーの指定
	DX12::GetInstance()->CommandList()->IASetIndexBuffer(&model.mesh.indexBufferView);

	// ディスクリプタの先頭ハンドルを取得しておく
	auto basicHeapHandle = model.mesh.basicDescHeap->GetGPUDescriptorHandleForHeapStart();
	// 次の場所に移動
	basicHeapHandle.ptr +=
		DX12::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize
		(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 最初はオフセットなし
	unsigned int idxOffset = 0;

	auto cbvsrvIncSize = DX12::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize
	(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 4;


	for (auto& m : model.mesh.materials)
	{
		// ルートパラメーターとディスクリプターヒープの関連付け
		DX12::GetInstance()->CommandList()->SetGraphicsRootDescriptorTable(
			1,					// ルートパラメーターインデックス
			basicHeapHandle);	// ヒープアドレス

		// 描画命令
		DX12::GetInstance()->CommandList()->DrawIndexedInstanced(m.indicesNum, 1, idxOffset, 0, 0);

		// ヒープポインターとインデックスを次に進める
		basicHeapHandle.ptr += cbvsrvIncSize;
		idxOffset += m.indicesNum;
	}
}

HRESULT PmdModel::CreatePMD(PMDMODEL& model, const char* file_name)
{
	HRESULT hr = S_OK;

	// PMFファイル読み込み
	if (FAILED(LoadPMD(model, file_name)))
		return E_FAIL;

	// 頂点インデックスバッファービューの生成
	if (FAILED(CreateVertexIndexBufferView(model)))
		return E_FAIL;

	// マテリアルバッファーの生成
	if (FAILED(CreateMaterialBuffer(model, file_name)))
		return E_FAIL;

	// テクスチャの生成
	if (FAILED(CreateTexture(model)))
		return E_FAIL;

	// コンスタントバッファービューの生成
	if (FAILED(CreateConstantBufferView(model)))
		return E_FAIL;

	return S_OK;
}
// pmdファイルの読み込み
HRESULT PmdModel::LoadPMD(PMDMODEL& model, const char* file_name)
{
	HRESULT hr = S_OK;

	FILE* fp = nullptr;	// ファイルポインター
	fopen_s(&fp, file_name, "rb");
	if (fp == nullptr)
	{
		return E_FAIL;
	}

	// 先頭の3文字がシグネチャー"Pmd"があるので、読み飛ばす
	char signature[3] = {};	// シグネチャー
	fread(signature, sizeof(signature), 1, fp);

	// ヘッダーの読み込み
	PMDHeader pmdHeader = {};
	fread(&pmdHeader, sizeof(pmdHeader), 1, fp);

	// 頂点数の読み込み
	fread(&model.mesh.verticesNum, sizeof(model.mesh.verticesNum), 1, fp);

	// 頂点情報の読み込み
	// バッファーの確保
	model.mesh.vertices.resize(model.mesh.verticesNum * _pmdVertex_size);
	fread(model.mesh.vertices.data(), model.mesh.vertices.size(), 1, fp);

	// インデックス数の読み込み
	fread(&model.mesh.indicesNum, sizeof(model.mesh.indicesNum), 1, fp);

	// インデックス情報の読み込み
	// バッファーの確保
	model.mesh.indices.resize(model.mesh.indicesNum);
	fread(model.mesh.indices.data(), model.mesh.indices.size() * sizeof(model.mesh.indices[0]), 1, fp);

	// マテリアル数の読み込み
	fread(&model.mesh.materialNum, sizeof(model.mesh.materialNum), 1, fp);

	// マテリアル情報の読み込み
	// バッファーの確保
	model.mesh.pmdMaterials.resize(model.mesh.materialNum);
	fread(model.mesh.pmdMaterials.data(),
		model.mesh.pmdMaterials.size() * sizeof(PMDMaterial), 1, fp);

	fclose(fp);

	return S_OK;
}
// マテリアルバッファーの生成
HRESULT PmdModel::CreateMaterialBuffer(PMDMODEL& model, const char* file_name)
{
	HRESULT hr = S_OK;

	model.mesh.toonTexBuffer.resize(model.mesh.pmdMaterials.size());
	for (int i = 0; i < model.mesh.pmdMaterials.size(); ++i)
	{
		// トゥーンリソースの読み込み
		std::string toonFilePath = "Resouse\\toon/";

		char toonFileName[16];

		int Idex = (int)model.mesh.pmdMaterials[i].toonIdx;
		if (Idex == 255)
		{
			Idex = 1;
		}
		else
		{
			Idex++;
		}
		sprintf_s(
			toonFileName,
			"toon%02d.bmp",
			Idex);

		toonFilePath += toonFileName;

		if (FAILED(CreateShaderResource(&model.mesh.toonTexBuffer[i], toonFilePath)))
			return hr;
	}
	// バッファーの確保
	model.mesh.materials.resize(model.mesh.pmdMaterials.size());
	// コピー
	for (int i = 0; i < model.mesh.pmdMaterials.size(); ++i)
	{
		model.mesh.materials[i].indicesNum = model.mesh.pmdMaterials[i].indicesNum;
		model.mesh.materials[i].material.diffuse = model.mesh.pmdMaterials[i].diffuse;
		model.mesh.materials[i].material.alpha = model.mesh.pmdMaterials[i].alpha;
		model.mesh.materials[i].material.specular = model.mesh.pmdMaterials[i].specular;
		model.mesh.materials[i].material.specularity = model.mesh.pmdMaterials[i].specularity;
		model.mesh.materials[i].material.ambient = model.mesh.pmdMaterials[i].ambient;

		model.mesh.materials[i].additional.toonIdx = model.mesh.pmdMaterials[i].toonIdx;
		model.mesh.materials[i].additional.edgeFlg = model.mesh.pmdMaterials[i].edgeFlg;

		if (strlen(model.mesh.pmdMaterials[i].texFilePath) == 0)
		{
			model.mesh.materials[i].additional.texPath = "";
		}
		else
		{
			std::string modelPath = file_name;
			auto texFilePath = GetTexturePathFromModelAndTexPath(modelPath, model.mesh.pmdMaterials[i].texFilePath);
			model.mesh.materials[i].additional.texPath = texFilePath;
		}
	}

	// マテリアル用バッファーを作成
	auto materialBufferSize = sizeof(MaterialForHlsl);
	materialBufferSize = (materialBufferSize + 0xff) & ~0xff;

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
	// サイズ幅[幅で全部まかなうのでマテリアル情報サイズ*数とする]
	resDesc.Width = materialBufferSize * model.mesh.materialNum;
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
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(model.mesh.materialBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// マテリアル情報のコピー
	char* mapMaterial = nullptr;
	// マテリアルバッファーのロック
	hr = model.mesh.materialBuffer->Map(0, nullptr,
		(void**)&mapMaterial);
	if (hr != S_OK)
		return hr;
	for (auto& m : model.mesh.materials)
	{
		*((MaterialForHlsl*)mapMaterial) = m.material;// データコピー
		mapMaterial += materialBufferSize;// 次のアライメント位置まで進める(256の倍数)
	}
	// マテリアルバッファーの解除
	model.mesh.materialBuffer->Unmap(0, nullptr);

	return S_OK;
}
// テクスチャの生成
HRESULT PmdModel::CreateTexture(PMDMODEL& model)
{
	HRESULT hr = S_OK;

	// テクスチャーの生成
	// テクスチャー情報の確保
	model.mesh.texBuffer.resize(model.mesh.materialNum);
	model.mesh.sphTexBuffer.resize(model.mesh.materialNum);
	model.mesh.spaTexBuffer.resize(model.mesh.materialNum);
	for (UINT i = 0; i < model.mesh.materialNum; ++i)
	{
		std::string texFileName = model.mesh.materials[i].additional.texPath;
		if (std::count(texFileName.begin(), texFileName.end(), '*') > 0)
		{
			// スプリッタがある
			auto namepair = SplitFileName(texFileName);
			if (GetExtension(namepair.first) == "sph" ||
				GetExtension(namepair.first) == "spa")
			{
				texFileName = namepair.second;
			}
			else
			{
				texFileName = namepair.first;
			}
		}

		if (strnlen_s(model.mesh.materials[i].additional.texPath.c_str(), MAX_PATH) == 0)
		{
			model.mesh.texBuffer[i] = nullptr;
			model.mesh.sphTexBuffer[i] = nullptr;
			model.mesh.spaTexBuffer[i] = nullptr;
			continue;
		}

		auto namepair = SplitFileName(texFileName);
		if (GetExtension(namepair.first) == "sph" ||
			GetExtension(namepair.first) == "spa")
		{
			model.mesh.texBuffer[i] = nullptr;
			if (GetExtension(namepair.first) == "sph")
			{
				model.mesh.spaTexBuffer[i] = nullptr;
				if (FAILED(CreateShaderResource
				(&model.mesh.sphTexBuffer[i], texFileName)))
					return E_FAIL;
			}
			else
			{
				model.mesh.sphTexBuffer[i] = nullptr;
				if (FAILED(CreateShaderResource
				(&model.mesh.spaTexBuffer[i], texFileName)))
					return E_FAIL;
			}
		}
		else
		{
			if (FAILED(CreateShaderResource
			(&model.mesh.texBuffer[i], texFileName)))
				return E_FAIL;
		}
	}
	return hr;
}
