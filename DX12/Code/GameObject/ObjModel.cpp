// --------------------- Include File --------------------
#include "ObjModel.h"
#include "GeometryData.h"
#include "../Camera/Camera.h"
#include "../DX12System.h"
#include "../VECTOR.h"
ObjModel* ObjModel::_instance = nullptr;			// インスタンスの実態
// コンストラクター
ObjModel::ObjModel() :
	_path(nullptr), _attributeEntryFlag(FALSE), _materialEntryFlag(FALSE)
{
}
// デストラクター
ObjModel::~ObjModel()
{
	for (auto fileName : _objMeshs)
	{
		for (auto mesh: fileName.second)
		{
			delete mesh;
		}
	}
}
// 生成
void ObjModel::Create()
{
	if (!_instance)
	{
		_instance = new ObjModel();

		_instance->CreateObjModel();
	}
}
// 破棄
void ObjModel::Destroy()
{
	delete _instance;
}


// 生成
HRESULT ObjModel::CreateObjModel()
{
	HRESULT hr = S_OK;

	_dev = DX12::GetInstance()->GetDevice();
	_cmdList = DX12::GetInstance()->CommandList();

	// ルートシグネチャーの生成
	if (FAILED(CreateRootSignature()))
		return E_FAIL;

	// 描画パイプラインステートの生成
	if (FAILED(CreatePipelineState()))
		return E_FAIL;

	return S_OK;
}

// ルートシグネチャーの生成
HRESULT ObjModel::CreateRootSignature()
{
	HRESULT hr = S_OK;

	// ディスクリプターレンジを作成
	D3D12_DESCRIPTOR_RANGE descTblRange[3] = {};
	// ディスクリプターの数[１つ]
	descTblRange[0].NumDescriptors = 1;
	// レンジの種類[CBV:コンスタントバッファービュー]
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// 先頭レジスター番号[0を選択]
	descTblRange[0].BaseShaderRegister = 0;
	// オフセットを指定[連続したディスクリプターレンジが前のディスクリプタレンジの直後にくる]
	descTblRange[0].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// ディスクリプターの数[１つ]
	descTblRange[1].NumDescriptors = 1;
	// レンジの種類[CBV:コンスタントバッファービュー]
	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// 先頭レジスター番号[1を選択]
	descTblRange[1].BaseShaderRegister = 1;
	// オフセットを指定[連続したディスクリプターレンジが前のディスクリプタレンジの直後にくる]
	descTblRange[1].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// ディスクリプターの数[１つ]
	descTblRange[2].NumDescriptors = 1;
	// レンジの種類[SRV:シェーダーリソースビュー]
	descTblRange[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	// 先頭レジスター番号[0を選択]
	descTblRange[2].BaseShaderRegister = 0;
	// オフセットを指定[連続したディスクリプターレンジが前のディスクリプタレンジの直後にくる]
	descTblRange[2].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	// ルートパラメーターを作成
	D3D12_ROOT_PARAMETER rootParam[2] = {};
	// 種別の選択[ディスクリプターテーブルを選択]
	rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// どのシェーダーから利用可能か[全てのシェーダー]
	rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// ディスクリプターレンジのアドレス
	rootParam[0].DescriptorTable.pDescriptorRanges =
		&descTblRange[0];
	// ディスクリプターレンジの数
	rootParam[0].DescriptorTable.NumDescriptorRanges = 1;
	// 種別の選択[ディスクリプターテーブルを選択]
	rootParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// どのシェーダーから利用可能か[全てのシェーダー]
	rootParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// ディスクリプターレンジのアドレス
	rootParam[1].DescriptorTable.pDescriptorRanges =
		&descTblRange[1];
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
	rootSignatureDesc.pParameters = &rootParam[0];
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
HRESULT ObjModel::CreatePipelineState()
{
	HRESULT hr = S_OK;

	// シェーダーオブジェクト読み込み用
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// 頂点シェーダーの読み込み
	hr = D3DCompileFromFile(
		L".\\Shader\\Obj\\ObjVertexShader.hlsl",// シェーダー名
		nullptr,							// defineはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// インクルードはデフォルト
		"ObjVS", "vs_5_0",			// 関数は BasicVS、対象シェーダーは vs_5_0
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
		L".\\Shader\\Obj\\ObjPixelShader.hlsl",	// シェーダー名
		nullptr,							// defineはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// インクルードはデフォルト
		"ObjPS", "ps_5_0",			// 関数は BasicPS、対象シェーダーは ps_5_0
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
		{	// 法線情報
			"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
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
	// カリングの設定(表描画)
	gpipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
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
	hr = _dev->CreateGraphicsPipelineState(
		&gpipelineDesc, IID_PPV_ARGS(_pipelineState.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	return S_OK;
}

// マテリアルバッファーの生成
HRESULT ObjModel::CreateMaterialBuffer(MESH* mesh)
{
	HRESULT hr = S_OK;

	// マテリアル用バッファーを作成
	auto materialBufferSize = sizeof(MetaData);
	materialBufferSize = (materialBufferSize + 0xff) & ~0xff;

	// 中間バッファーとしてのアップロードヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProp = {};
	// マップ可能にするため、UPLOADにする
	uploadHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// アップロード用に使用すること前提なのでUNKNOWNでよい
	uploadHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	uploadHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	uploadHeapProp.CreationNodeMask = 0;	// 単一アダプターのため0
	uploadHeapProp.VisibleNodeMask = 0;	// 単一アダプターのため0

	// リソースを設定
	D3D12_RESOURCE_DESC resDesc = {};
	// フォーマット[単なるデータの塊なのでUNKNOWN]
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	// 利用方法[単なるバッファーとして指定]
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	// サイズ幅[データサイズ]
	resDesc.Width = materialBufferSize * (UINT)_material.size();
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
	hr = _dev->CreateCommittedResource(
		&uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,	// 特に指定なし
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,	// CPUからの書き込み可能、GPUから見ると読み取りのみ
		nullptr,
		IID_PPV_ARGS(mesh->materialBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	return S_OK;
}
// シェーダーリソースの生成
HRESULT ObjModel::CreateShaderResource(
	const TCHAR* pFileName,
	ComPtr<ID3D12Resource>& textureBuffer)
{
	HRESULT hr = S_OK;

	// 画像サイズの取得とイメージデーター確保
	BYTE* pImageData = nullptr;
	SIZE size = {};
	if (lstrlenW(pFileName) > 0)
	{
		// 画像情報の読み込み
		if (FAILED(LoadTextureFromFile(pFileName,
			&pImageData, &size)))
			return E_FAIL;
	}
	else
	{
		// ダミー画像(白色)の生成
		size.cx = 4;
		size.cy = 4;
		// 1ピクセル当たりのバイト数
		UINT bytePerPixel = 4;
		// データサイズの決定(RGBA)
		UINT imageSize = size.cx * size.cy * bytePerPixel;
		// 保存領域作成
		pImageData = new BYTE[imageSize];
		::FillMemory(pImageData, imageSize, 0xFF);
	}

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

// コンスタントバッファービューの生成
HRESULT ObjModel::CreateConstantBufferView(MESH* mesh)
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

	ConstantBuffer constantBuffer = {};
	// リソースの設定
	D3D12_RESOURCE_DESC resDesc = {};
	// 利用方法[バッファーに使うのでBUFFERを指定]
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	// サイズ幅[幅で全部まかなうのでsizeof(全情報):256の倍数とする]
	resDesc.Width = (sizeof(constantBuffer) + 0xff) & ~0xff;
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
	// デスクリプタを指定[CBV:1つ (CBV:1つ + SRV:1つ) * 数]
	heapDesc.NumDescriptors = 1 + 2 * (UINT)_material.size();
	// 特に指定なし[シェーダー側から見える必要あり]
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// デスクリプタヒープを作成
	hr = _dev->CreateDescriptorHeap(&heapDesc
		, IID_PPV_ARGS(mesh->basicDescHeap.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

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

	// ディスクリプタの先頭ハンドルを取得しておく
	auto basicHeapHandle = mesh->basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = mesh->constBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)mesh->constBuffer->GetDesc().Width;

	// 定数バッファービューの作成
	_dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	// 次の場所に移動
	basicHeapHandle.ptr +=
		_dev->GetDescriptorHandleIncrementSize
		(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// マテリアル用バッファーを作成
	auto materialBufferSize = sizeof(MetaData);
	materialBufferSize = (materialBufferSize + 0xff) & ~0xff;
	cbvDesc.BufferLocation = mesh->materialBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)materialBufferSize;

	auto incSize = _dev->GetDescriptorHandleIncrementSize
	(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (UINT i = 0; i < _material.size(); ++i)
	{
		// 定数バッファービューの作成
		_dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
		// マテリアルのロケーションを移動
		cbvDesc.BufferLocation += materialBufferSize;

		// 登録先のアドレスを移動
		basicHeapHandle.ptr += incSize;

		// シェーダーリソースビューの生成
		_dev->CreateShaderResourceView(
			mesh->textureBuffer[i].Get(),	// ビューと関連付けるバッファーを指定
			&srvDesc,				// テクスチャー選定情報を指定
			basicHeapHandle);		// ヒープのどこに割り当てるか

		// 登録先のアドレスを移動
		basicHeapHandle.ptr += incSize;
	}

	return S_OK;
}

// 描画
void ObjModel::Draw()
{
	for (auto mesh : _objMeshs)
	{
		if (_objInstanceEntry[mesh.first] == 0) {
			continue;
		}
		for (int m = 0, size = (int)mesh.second.size(); m < size; ++m)
		{

			// マテリアル用バッファーを作成
			auto materialBufferSize = sizeof(MetaData);
			materialBufferSize = (materialBufferSize + 0xff) & ~0xff;
			// マップマテリアルにコピー
			BYTE* mapMaterial = nullptr;
			mesh.second[m]->materialBuffer->Map(0, nullptr, (void**)&mapMaterial);	// マップ

			int EntryNum = _objInstanceEntry[mesh.first] - m * instanceMax;
			if (EntryNum >= instanceMax)
			{
				EntryNum = instanceMax;
			}
	
			for (std::vector<MATERIAL>::size_type k = 0, size = (int)_saveMaterial[mesh.first].size();
				k < size; ++k)
			{			
				*((MetaData*)mapMaterial) = _objMetaData[mesh.first][m][k];	// データーコピー
				mapMaterial += materialBufferSize;	// 次のアライメント位置まで進める(256の倍数)
			}
			mesh.second[m]->materialBuffer->Unmap(0, nullptr);	// アンマップ

			// =====ビュー行列=====
			XMMATRIX viewMat = Camera::GetInstance()->GetView();
			// =====プロジェクション行列=====
			XMMATRIX projMat = Camera::GetInstance()->GetProjection();
			// ===== ライトの向き =====
			XMFLOAT4 lightVector = Camera::GetInstance()->GetLightVector();
			// ===== ライトの色 =====
			XMFLOAT4 lightColor = Camera::GetInstance()->GetLightColor();
			// ===== カメラ座標 =====
			XMFLOAT3 eye = Camera::GetInstance()->GetEyePosition();
			// 定数情報の更新
			ConstantBuffer* mapMatrix = nullptr;	// マップ先を示すポインター
			mesh.second[m]->constBuffer->Map(0, nullptr, (void**)&mapMatrix);	// マップ
			// 行列の内容をコピー
			mapMatrix->view = viewMat;
			mapMatrix->projection = projMat;
			mapMatrix->lightVector = lightVector;
			mapMatrix->lightColor = lightColor;
			mapMatrix->eye = eye;
			mesh.second[m]->constBuffer->Unmap(0, nullptr);		// アンマップ

			// グラフィックスパイプラインステートの指定
			_cmdList->SetPipelineState(_pipelineState.Get());
			// ルートシグネチャーの指定
			_cmdList->SetGraphicsRootSignature(_rootSignature.Get());

			// ディスクリプターヒープの指定
			_cmdList->SetDescriptorHeaps(1, mesh.second[m]->basicDescHeap.GetAddressOf());

			// ルートパラメーターとディスクリプターヒープの関連付け
			_cmdList->SetGraphicsRootDescriptorTable(
				0,	// ルートパラメーターインデックス
				mesh.second[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart());

			// 頂点情報の指定[三角形]
			_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// 頂点バッファーの指定
			_cmdList->IASetVertexBuffers(0, 1, &mesh.second[m]->vertexBufferView);

			// インデックスバッファーの指定
			_cmdList->IASetIndexBuffer(&mesh.second[m]->indexBufferView);

			// ディスクリプタの先頭ハンドルを取得しておく
			auto basicHeapHandle = mesh.second[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart();

			// 次の場所に移動
			basicHeapHandle.ptr +=
				_dev->GetDescriptorHandleIncrementSize
				(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			// CBV + SRV で2つ
			auto incSize = _dev->GetDescriptorHandleIncrementSize
			(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 2;

			// 最初はオフセットなし
			UINT idxOffset = 0;

			for (std::vector<ATTRIBUTE>::size_type i = 0,size= _saveAttribute[mesh.first].size();
				i < size; ++i)
			{
				// ルートパラメーターとディスクリプターヒープの関連付け
				_cmdList->SetGraphicsRootDescriptorTable(
					1,	// ルートパラメーターインデックス
					basicHeapHandle);

				UINT verticesNum = _saveAttribute[mesh.first][i].verticesNum;

				// 描画命令
				_cmdList->DrawIndexedInstanced(verticesNum, _objInstanceEntry[mesh.first], idxOffset, 0, 0);

				// 呼び出し先のアドレスを移動
				basicHeapHandle.ptr += incSize;

				// オフセットを進ませる
				idxOffset += verticesNum;
			}
		}
	}
}
// OBJファイルの読み込み
BOOL ObjModel::LoadObjFromFile(const char* filename)
{

	INT size = (INT)strlen(filename) + 1;

	FILE* fp = nullptr;	// ファイルポインター

	// ファイルを開ける
	fopen_s(&fp, filename, "rt");
	if (fp == nullptr)
	{
		return FALSE;
	}

	TCHAR line[MAX_PATH] = _T("");
	TCHAR keyWord[MAX_PATH] = _T("");

	ATTRIBUTE attribute = {};

	BOOL faceTriangleStartFlag = TRUE;	// 面情報を初めて取得したかの有無
	BOOL faceQuadrangleStartFlag = TRUE;// 面情報を初めて取得したかの有無
	UINT triangleStartIndex = 0;		// 面情報の開始インデックス
	UINT quadrangleStartIndex = 0;		// 面情報の開始インデックス

	BOOL entryTriangleIndex = FALSE;	// Triangle登録の有無
	BOOL entryQuadrangleIndex = FALSE;	// Quad登録の有無

	while (!feof(fp))
	{
		::ZeroMemory(line, sizeof(line));
		::ZeroMemory(keyWord, sizeof(keyWord));
		fgetws(line, MAX_PATH, fp);

		// 余分な空白を読み飛ばす
		INT size = lstrlenW(line);
		int j = 0;
		for (INT i = 0; i < size; ++i)
		{
			if (line[i] != ' ')
			{
				memcpy_s(keyWord, MAX_PATH, &line[i], sizeof(TCHAR) * size);
				keyWord[size - j - 1] = '\0';
				break;
			}
			j++;
		}

		// #:コメント、\n:改行コード
		if (keyWord[0] == '#' ||
			keyWord[0] == '\n')
			;
		// "mtllib":マテリアル情報のファイル名
		else if (keyWord[0] == 'm' && keyWord[1] == 't' && keyWord[2] == 'l'
			&& keyWord[3] == 'l' && keyWord[4] == 'i' && keyWord[5] == 'b')
		{
			// ファイルを取得
			TCHAR mtlFilename[MAX_PATH] = _T("");
			// 6文字読み込み位置を移動させる
			for (INT i = 6; i < size; ++i)
			{
				if (keyWord[i] != ' ')
				{
					UINT copySize = static_cast<UINT>(size - i);
					memcpy_s(mtlFilename, MAX_PATH, &keyWord[i], sizeof(TCHAR) * copySize);
					break;
				}
			}

			// フォルダーが存在する場合は、ファイル名に付加
			TCHAR pathFilename[MAX_PATH] = {};
			UINT checkPos = 0;

			INT size = static_cast<INT>(strlen(filename));

			for (INT i = size; i >= 0; --i)
			{
				if (filename[i] == '\\')
				{
					checkPos = static_cast<UINT>(i);
					break;
				}
			}

			if (checkPos > 0)
			{
				char* folder = new char[MAX_PATH];
				checkPos++;
				const rsize_t copySize = checkPos;
				memcpy_s(folder, MAX_PATH, static_cast<const void*>(filename), copySize);
				folder[checkPos] = '\0';

				// フォルダーをchar->wcharに変換
				WCHAR* wFolder = new WCHAR[(strlen(folder) + 1) * 2];
				// ロケール指定
				setlocale(LC_ALL, "japanese");
				size_t len = 0;
				// CHARSTR -> LPCWSTR
				mbstowcs_s(&len, wFolder, strlen(folder) + 1, folder, _TRUNCATE);
				lstrcpyW(pathFilename, wFolder);
				// パスを保存
				_path = new TCHAR[strlen(folder) + 1];
				StrCpyNW(_path, pathFilename,
					static_cast<INT>(strlen(folder) + 1));
				delete[] wFolder;
				delete[] folder;
			}

			lstrcatW(pathFilename, mtlFilename);

			// マテリアル情報の読み込み
			if (!LoadMaterialFromFile(pathFilename))
			{
				// ファイルを閉じる
				fclose(fp);
				return FALSE;
			}
		}

		// usemtl:使用するマテリアル名
		if (keyWord[0] == 'u' && keyWord[1] == 's' && keyWord[2] == 'e' &&
			keyWord[3] == 'm' && keyWord[4] == 't' && keyWord[5] == 'l')
		{
			// マテリアル名を取得
			TCHAR name[MAX_PATH] = _T("");
			// 6文字読み込み位置を移動させる
			for (INT i = 6; i < size; ++i)
			{
				if (keyWord[i] != ' ')
				{
					UINT copySize = static_cast<UINT>(size - i);
					memcpy_s(name, MAX_PATH, &keyWord[i], sizeof(TCHAR) * copySize);
					break;
				}
			}
			if (_attributeEntryFlag)
			{
				// 前回迄の属性情報を保存
				UINT entryNumber = (UINT)_attribute.size() - 1;
				if (entryTriangleIndex)
				{
					_attribute[entryNumber].triangleStartIndex = triangleStartIndex;
					_attribute[entryNumber].triangleIndexNum = (UINT)_triangleIndex.size() - triangleStartIndex;
				}
				if (entryQuadrangleIndex)
				{
					_attribute[entryNumber].quadrangleStartIndex = quadrangleStartIndex;
					_attribute[entryNumber].quadrangleIndexNum = (UINT)_quadrangleIndex.size() - quadrangleStartIndex;
				}
				faceTriangleStartFlag = TRUE;
				faceQuadrangleStartFlag = TRUE;
			}
			_attributeEntryFlag = TRUE;
			::ZeroMemory(&attribute, sizeof(attribute));
			// 名前の登録
			StrCpyNW(attribute.materialName
				, name, lstrlenW(name) + 1);
			_attribute.push_back(attribute);
		}
		// "v ":頂点情報
		else if (keyWord[0] == 'v' && keyWord[1] == ' ')
		{
			XMFLOAT3 position = {};
			INT count = 0;

			count = swscanf_s(keyWord, _T("%*s%f%f%f")
				, &position.x, &position.y, &position.z);
			if (count == 3)
			{
				_position.push_back(position);
			}
		}
		// "vn":法線情報
		else if (keyWord[0] == 'v' && keyWord[1] == 'n')
		{
			XMFLOAT3 normal = {};
			INT count = 0;

			count = swscanf_s(keyWord, _T("%*s%f%f%f")
				, &normal.x, &normal.y, &normal.z);
			if (count == 3)
			{
				_normal.push_back(normal);
			}
		}
		// "vt":テクスチャーUV情報
		else if (keyWord[0] == 'v' && keyWord[1] == 't')
		{
			XMFLOAT2 textureUV = {};
			INT count = 0;

			count = swscanf_s(keyWord, _T("%*s%f%f")
				, &textureUV.x, &textureUV.y);
			if (count == 2)
			{
				_textureUV.push_back(textureUV);
			}
		}
		// "f ":面
		else if (keyWord[0] == 'f' && keyWord[1] == ' ')
		{
			UINT loadPos = 0;

			UINT triangleIndexSize = (UINT)_triangleIndex.size();
			UINT quadrangleIndexSize = (UINT)_quadrangleIndex.size();

			// keyWordから数値が出現する迄進ませる
			for (UINT i = 0; keyWord[i] != '\0'; ++i)
			{
				if ('0' <= keyWord[i] && keyWord[i] <= '9')
				{
					loadPos = i;
					break;
				}
			}

			// '/'の出現数を求める
			UINT count = 0;
			for (UINT i = loadPos; keyWord[i] != '\0'; ++i)
			{
				if (keyWord[i] == '/')
				{
					count++;
				}
			}

			// [種類 * 頂点の数]
			UINT indices[3 * 4] = { 0, };
			// カウントが0の場合:頂点のみ
			// カウントが0以外:頂点/UV値/法線

			// カウントが0以外の場合は、UVに値があるか
			BOOL textureUV = FALSE;
			if (count != 0)
			{
				// 仮に1頂点を読み込んで、[1]の値がない場合は、テクスチャーUV値なし
				swscanf_s(keyWord, _T("%*s%d/%d/%d")
					, &indices[0], &indices[1], &indices[2]);
				if (indices[1] > 0)
				{
					textureUV = TRUE;
				}
			}

			if (count == 0)
			{
				// テクスチャーUV値と法線がないので、ダミーで0を代入
				swscanf_s(&keyWord[loadPos], _T("%d %d %d %d")
					, &indices[0], &indices[3], &indices[6], &indices[9]);
				// Triangle
				if (indices[9] == 0)
				{
					for (UINT i = 0; i < 9; ++i)
					{
						--indices[i];
						_triangleIndex.push_back(indices[i]);
					}
					entryTriangleIndex = TRUE;
				}
				// Quadrangle
				else
				{
					for (UINT i = 0; i < 12; ++i)
					{
						--indices[i];
						_quadrangleIndex.push_back(indices[i]);
					}
					entryQuadrangleIndex = TRUE;
				}
			}
			else
			{
				// テクスチャーUV値あり
				if (textureUV)
				{
					swscanf_s(keyWord, _T("%*s%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d")
						, &indices[0], &indices[1], &indices[2]
						, &indices[3], &indices[4], &indices[5]
						, &indices[6], &indices[7], &indices[8]
						, &indices[9], &indices[10], &indices[11]);
					// Triangle
					if (indices[9] + indices[10] + indices[11] == 0)
					{
						for (UINT i = 0; i < 9; ++i)
						{
							--indices[i];
							_triangleIndex.push_back(indices[i]);
						}
						entryTriangleIndex = TRUE;
					}
					// Quadrangle
					else
					{
						for (UINT i = 0; i < 12; ++i)
						{
							--indices[i];
							_quadrangleIndex.push_back(indices[i]);
						}
						entryQuadrangleIndex = TRUE;
					}
				}
				else
				{
					swscanf_s(keyWord, _T("%*s%d//%d %d//%d %d//%d %d//%d")
						, &indices[0], &indices[2]
						, &indices[3], &indices[5]
						, &indices[6], &indices[8]
						, &indices[9], &indices[11]);
					// Triangle
					if (indices[9] + indices[11] == 0)
					{
						for (UINT i = 0; i < 9; ++i)
						{
							--indices[i];
							_triangleIndex.push_back(indices[i]);
						}
						entryTriangleIndex = TRUE;
					}
					// Quadrangle
					else
					{
						for (UINT i = 0; i < 12; ++i)
						{
							--indices[i];
							_quadrangleIndex.push_back(indices[i]);
						}
						entryQuadrangleIndex = TRUE;
					}
				}
			}
			if (faceTriangleStartFlag)
			{
				triangleStartIndex = triangleIndexSize;
				faceTriangleStartFlag = FALSE;
			}
			if (faceQuadrangleStartFlag)
			{
				quadrangleStartIndex = quadrangleIndexSize;
				faceQuadrangleStartFlag = FALSE;
			}
		}
	}

	// 前回迄の属性情報を保存
	UINT entryNumber = 0;
	if (_attribute.size() > 0)
	{
		UINT entryNumber = (UINT)_attribute.size() - 1;
		if (entryTriangleIndex)
		{
			_attribute[entryNumber].triangleStartIndex
				= triangleStartIndex;
			_attribute[entryNumber].triangleIndexNum
				= (UINT)_triangleIndex.size() - triangleStartIndex;
		}
		if (entryQuadrangleIndex)
		{
			_attribute[entryNumber].quadrangleStartIndex
				= quadrangleStartIndex;
			_attribute[entryNumber].quadrangleIndexNum
				= (UINT)_quadrangleIndex.size() - quadrangleStartIndex;
		}
	}

	// ファイルを閉じる
	fclose(fp);

	
	return TRUE;
}
// マテリアルの読み込み
BOOL ObjModel::LoadMaterialFromFile(const TCHAR* filename)
{
	// LPCWSTR -> CHARSTR変更
	size_t len = 0;
	INT size = lstrlenW(filename) + 1;
	char* newFilename = new char[size];
	wcstombs_s(&len, newFilename, size
		, filename, _TRUNCATE);

	FILE* fp = nullptr;	// ファイルポインター

	// ファイルを開ける
	fopen_s(&fp, newFilename, "rt");
	if (fp == nullptr)
	{
		return FALSE;
	}

	TCHAR line[MAX_PATH] = _T("");
	TCHAR keyWord[MAX_PATH] = _T("");

	MATERIAL material = {};

	while (!feof(fp))
	{
		::ZeroMemory(line, sizeof(line));
		::ZeroMemory(keyWord, sizeof(keyWord));

		fgetws(line, MAX_PATH, fp);

		// 余分な空白を読み飛ばす
		INT size = lstrlenW(line);
		int j = 0;
		for (INT i = 0; i < size; ++i)
		{
			if (!(line[i] == ' ' || line[i] == '\t'))
			{
				memcpy_s(keyWord, MAX_PATH, &line[i], sizeof(TCHAR) * size);
				keyWord[size - j - 1] = '\0';
				break;
			}
			j++;
		}

		// keyWordサイズを取得
		INT keyWordSize = lstrlenW(keyWord);

		// #:コメント、\n:改行コード
		if (keyWord[0] == '#' ||
			keyWord[0] == '\n')
			;
		// newmtl:使用するマテリアル名
		else if (keyWord[0] == 'n' && keyWord[1] == 'e' && keyWord[2] == 'w' &&
			keyWord[3] == 'm' && keyWord[4] == 't' && keyWord[5] == 'l')
		{
			// マテリアル名を取得
			TCHAR name[MAX_PATH] = _T("");
			UINT copySize = 0;
			// 6文字読み込み位置を移動させる
			for (INT i = 6; i < keyWordSize; ++i)
			{
				if (keyWord[i] != ' ')
				{
					copySize = static_cast<UINT>(keyWordSize - i);
					memcpy_s(name, MAX_PATH, &keyWord[i], sizeof(TCHAR) * copySize);
					break;
				}
			}
			if (_materialEntryFlag)
				_material.push_back(material);
			_materialEntryFlag = TRUE;
			::ZeroMemory(&material, sizeof(material));
			// 名前の登録
			memcpy_s(material.materialName, MAX_PATH, name, sizeof(TCHAR) * copySize);
		}
		// "Ns":スペキュラ指数
		else if (keyWord[0] == 'N' && keyWord[1] == 's')
		{
			swscanf_s(keyWord, _T("%*s%f"), &material.Ns);
		}
		// "d":ディゾルブル
		else if (keyWord[0] == 'd')
		{
			swscanf_s(keyWord, _T("%*s%f"), &material.dissolve);
		}
		// Tr:発光色
		else if (keyWord[0] == 'T' && keyWord[1] == 'r')
		{
			swscanf_s(keyWord, _T("%*s%f"), &material.Tr);
		}
		// "illum":照明モデル
		else if (keyWord[0] == 'i' && keyWord[1] == 'l' &&
			keyWord[2] == 'l' && keyWord[3] == 'u' && keyWord[4] == 'm')
		{
			swscanf_s(keyWord, _T("%*s%d"), &material.illum);
		}
		// "Ka":アンビエント
		else if (keyWord[0] == 'K' && keyWord[1] == 'a')
		{
			swscanf_s(keyWord, _T("%*s%f%f%f"), &material.Ka.x, &material.Ka.y, &material.Ka.z);
		}
		// "Kd":ディフューズ
		else if (keyWord[0] == 'K' && keyWord[1] == 'd')
		{
			swscanf_s(keyWord, _T("%*s%f%f%f"), &material.Kd.x, &material.Kd.y, &material.Kd.z);
		}
		// "Ks":スペキュラー
		else if (keyWord[0] == 'K' && keyWord[1] == 's')
		{
			swscanf_s(keyWord, _T("%*s%f%f%f"), &material.Ks.x, &material.Ks.y, &material.Ks.z);
		}
		// テクスチャー情報
		else if (keyWordSize > 6)
		{
			// map_Ka:テクスチャーアンビエント名
			if (keyWord[0] == 'm' && keyWord[1] == 'a' && keyWord[2] == 'p' &&
				keyWord[3] == '_' && keyWord[4] == 'K' && keyWord[5] == 'a')
			{
				// ファイル名
				TCHAR name[MAX_PATH] = _T("");
				UINT copySize = 0;
				// 6文字読み込み位置を移動させる
				for (INT i = 6; i < keyWordSize; i++)
				{
					if (keyWord[i] != ' ')
					{
						copySize = static_cast<UINT>(keyWordSize - i);
						memcpy_s(name, MAX_PATH, &keyWord[i], sizeof(TCHAR) * copySize);
						break;
					}
				}
				// ファイル名の登録
				TCHAR value = '\0';
				memcpy_s(&name[copySize], MAX_PATH, &value, sizeof(TCHAR));
				memcpy_s(material.textureNameKa, MAX_PATH, name, sizeof(TCHAR) * copySize);
			}
			// map_Kd:テクスチャーディフューズ名
			if (keyWord[0] == 'm' && keyWord[1] == 'a' && keyWord[2] == 'p' &&
				keyWord[3] == '_' && keyWord[4] == 'K' && keyWord[5] == 'd')
			{
				// ファイル名
				TCHAR name[MAX_PATH] = _T("");
				UINT copySize = 0;
				// 6文字読み込み位置を移動させる
				for (INT i = 6; i < keyWordSize; i++)
				{
					if (keyWord[i] != ' ')
					{
						copySize = static_cast<UINT>(keyWordSize - i);
						memcpy_s(name, MAX_PATH, &keyWord[i], sizeof(TCHAR) * copySize);
						break;
					}
				}
				// ファイル名の登録
				TCHAR value = '\0';
				memcpy_s(&name[copySize], MAX_PATH, &value, sizeof(TCHAR));
				memcpy_s(material.textureNameKd, MAX_PATH, name, sizeof(TCHAR) * copySize);
			}
			// map_Ks:テクスチャースペキュラー名
			if (keyWord[0] == 'm' && keyWord[1] == 'a' && keyWord[2] == 'p' &&
				keyWord[3] == '_' && keyWord[4] == 'K' && keyWord[5] == 's')
			{
				// ファイル名
				TCHAR name[MAX_PATH] = _T("");
				UINT copySize = 0;
				// 6文字読み込み位置を移動させる
				for (INT i = 6; i < keyWordSize; i++)
				{
					if (keyWord[i] != ' ')
					{
						copySize = static_cast<UINT>(keyWordSize - i);
						memcpy_s(name, MAX_PATH, &keyWord[i], sizeof(TCHAR) * copySize);
						break;
					}
				}
				// ファイル名の登録
				TCHAR value = '\0';
				memcpy_s(&name[copySize], MAX_PATH, &value, sizeof(TCHAR));
				memcpy_s(material.textureNameKs, MAX_PATH, name, sizeof(TCHAR) * copySize);
			}
			// map_Ns:テクスチャーハイライト成分名
			if (keyWord[0] == 'm' && keyWord[1] == 'a' && keyWord[2] == 'p' &&
				keyWord[3] == '_' && keyWord[4] == 'N' && keyWord[5] == 's')
			{
				// ファイル名
				TCHAR name[MAX_PATH] = _T("");
				UINT copySize = 0;
				// 6文字読み込み位置を移動させる
				for (INT i = 6; i < keyWordSize; i++)
				{
					if (keyWord[i] != ' ')
					{
						copySize = static_cast<UINT>(keyWordSize - i);
						memcpy_s(name, MAX_PATH, &keyWord[i], sizeof(TCHAR) * copySize);
						break;
					}
				}
				// ファイル名の登録
				TCHAR value = '\0';
				memcpy_s(&name[copySize], MAX_PATH, &value, sizeof(TCHAR));
				memcpy_s(material.textureNameNs, MAX_PATH, name, sizeof(TCHAR) * copySize);
			}
		}
	}

	if (_materialEntryFlag)
		_material.push_back(material);

	delete[] newFilename;

	// ファイルを閉じる
	fclose(fp);

	return TRUE;
}
// 変換
HRESULT ObjModel::Convert(MESH* mesh, std::vector<VECTOR>& saveVertex)
{
	HRESULT hr = S_OK;

	UINT quadrangleIndexNum = (UINT)_quadrangleIndex.size();
	UINT triangleIndexNum = (UINT)_triangleIndex.size();
	UINT entryPos = 0;

	// 属性情報の更新
	UINT polygonSize = 3;
	UINT verticesNum = 0;
	UINT attributeNum = (UINT)_attribute.size();
	UINT materialNum = (UINT)_material.size();

	for (UINT i = 0; i < attributeNum; ++i)
	{
		// マテリアル番号の決定
		for (UINT j = 0; j < materialNum; ++j)
		{
			if (lstrcmpW(_material[j].materialName,
				_attribute[i].materialName) == 0)
			{
				_attribute[i].materialNumber = j;
				break;
			}
		}
		// 頂点描画数の決定
		if (quadrangleIndexNum > 0
			&& triangleIndexNum > 0)
		{
			_attribute[i].verticesNum =
				_attribute[i].quadrangleIndexNum / 12 * 2 * polygonSize +
				_attribute[i].triangleIndexNum / 9 * polygonSize;
			verticesNum += _attribute[i].verticesNum;
		}
		if (quadrangleIndexNum > 0
			&& triangleIndexNum == 0)
		{
			_attribute[i].verticesNum =
				_attribute[i].quadrangleIndexNum / 12 * 2 * polygonSize;
			verticesNum += _attribute[i].verticesNum;
		}
		if (quadrangleIndexNum == 0
			&& triangleIndexNum > 0)
		{
			_attribute[i].verticesNum =
				_attribute[i].triangleIndexNum / 9 * polygonSize;
			verticesNum += _attribute[i].verticesNum;
		}
	}

	// Quadrangle -> Triangleの順番で登録
	VERTICES* vertices = new VERTICES[verticesNum];
	::ZeroMemory(vertices, sizeof(vertices[0]) * verticesNum);
	saveVertex.resize(verticesNum);
	for (UINT j = 0; j < attributeNum; ++j)
	{
		if (quadrangleIndexNum > 0)
		{
			// 4頂点から3頂点に変換(4頂点で2面とN/Tの分)
			for (UINT i = _attribute[j].quadrangleStartIndex;
				i < _attribute[j].quadrangleStartIndex + _attribute[j].quadrangleIndexNum;
				i += (4 * polygonSize), entryPos += (2 * polygonSize))
			{
				// 頂点
				vertices[entryPos + 0].position.x = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 0]].x;
				vertices[entryPos + 0].position.y = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 0]].y;
				vertices[entryPos + 0].position.z = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 0]].z;
				vertices[entryPos + 0].position.w = 1.0f;

				vertices[entryPos + 1].position.x = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 2]].x;
				vertices[entryPos + 1].position.y = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 2]].y;
				vertices[entryPos + 1].position.z = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 2]].z;
				vertices[entryPos + 1].position.w = 1.0f;

				vertices[entryPos + 2].position.x = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 1]].x;
				vertices[entryPos + 2].position.y = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 1]].y;
				vertices[entryPos + 2].position.z = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 1]].z;
				vertices[entryPos + 2].position.w = 1.0f;

				vertices[entryPos + 3].position.x = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 0]].x;
				vertices[entryPos + 3].position.y = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 0]].y;
				vertices[entryPos + 3].position.z = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 0]].z;
				vertices[entryPos + 3].position.w = 1.0f;

				vertices[entryPos + 4].position.x = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 3]].x;
				vertices[entryPos + 4].position.y = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 3]].y;
				vertices[entryPos + 4].position.z = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 3]].z;
				vertices[entryPos + 4].position.w = 1.0f;

				vertices[entryPos + 5].position.x = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 2]].x;
				vertices[entryPos + 5].position.y = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 2]].y;
				vertices[entryPos + 5].position.z = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 2]].z;
				vertices[entryPos + 5].position.w = 1.0f;

				// ModelMeshColliderを作るために必要
				// エラー避ける用
				int num = entryPos + 0;
				saveVertex[num].x = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 0]].x;
				saveVertex[num].y = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 0]].y;
				saveVertex[num].z = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 0]].z;

				num = entryPos + 1;
				saveVertex[num].x = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 2]].x;
				saveVertex[num].y = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 2]].y;
				saveVertex[num].z = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 2]].z;

				num = entryPos + 2;
				saveVertex[num].x = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 1]].x;
				saveVertex[num].y = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 1]].y;
				saveVertex[num].z = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 1]].z;

				num = entryPos + 3;
				saveVertex[num].x = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 0]].x;
				saveVertex[num].y = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 0]].y;
				saveVertex[num].z = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 0]].z;

				num = entryPos + 4;
				saveVertex[num].x = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 3]].x;
				saveVertex[num].y = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 3]].y;
				saveVertex[num].z = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 3]].z;

				num = entryPos + 4;
				saveVertex[num].x = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 2]].x;
				saveVertex[num].y = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 2]].y;
				saveVertex[num].z = _position[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 2]].z;

				for (UINT k = 0; k < 6; ++k)
				{
					vertices[entryPos + k].position.z *= -1.0f;
					num = entryPos + k;
					saveVertex[num].z *= -1.0f;
				}

				// 法線(再計算:flat)
				XMVECTOR v0 = XMLoadFloat4(&vertices[entryPos + 0].position);
				XMVECTOR v1 = XMLoadFloat4(&vertices[entryPos + 1].position);
				XMVECTOR v2 = XMLoadFloat4(&vertices[entryPos + 2].position);
				XMVECTOR v3 = v0 - v1;
				XMVECTOR v4 = v2 - v1;
				XMVECTOR cross = XMVector3Cross(v4, v3);
				cross = XMVector3Normalize(cross);
				for (UINT k = 0; k < 6; ++k)
				{
					XMStoreFloat4(&vertices[entryPos + k].normal, cross);
				}

				// テクスチャーUV値
				if (_textureUV.size() > 0)
				{
					vertices[entryPos + 0].textureUV = _textureUV[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 0 + 1]];
					vertices[entryPos + 1].textureUV = _textureUV[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 2 + 1]];
					vertices[entryPos + 2].textureUV = _textureUV[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 1 + 1]];
					vertices[entryPos + 3].textureUV = _textureUV[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 0 + 1]];
					vertices[entryPos + 4].textureUV = _textureUV[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 3 + 1]];
					vertices[entryPos + 5].textureUV = _textureUV[_quadrangleIndex[static_cast<UINT64>(i) + 3 * 2 + 1]];
				}
				else
				{
					for (UINT k = 0; k < 6; ++k)
					{
						vertices[entryPos + k].textureUV = XMFLOAT2(0.0f, 0.0f);
					}
				}
			}
		}
		if (triangleIndexNum > 0)
		{
			for (UINT i = _attribute[j].triangleStartIndex; i < _attribute[j].triangleStartIndex + _attribute[j].triangleIndexNum; i += (3 * polygonSize), entryPos += polygonSize)
			{
				// 頂点
				vertices[entryPos + 0].position.x = _position[_triangleIndex[static_cast<UINT64>(i) + 3 * 0]].x;
				vertices[entryPos + 0].position.y = _position[_triangleIndex[static_cast<UINT64>(i) + 3 * 0]].y;
				vertices[entryPos + 0].position.z = _position[_triangleIndex[static_cast<UINT64>(i) + 3 * 0]].z;
				vertices[entryPos + 0].position.w = 1.0f;

				vertices[entryPos + 1].position.x = _position[_triangleIndex[static_cast<UINT64>(i) + 3 * 2]].x;
				vertices[entryPos + 1].position.y = _position[_triangleIndex[static_cast<UINT64>(i) + 3 * 2]].y;
				vertices[entryPos + 1].position.z = _position[_triangleIndex[static_cast<UINT64>(i) + 3 * 2]].z;
				vertices[entryPos + 1].position.w = 1.0f;

				vertices[entryPos + 2].position.x = _position[_triangleIndex[static_cast<UINT64>(i) + 3 * 1]].x;
				vertices[entryPos + 2].position.y = _position[_triangleIndex[static_cast<UINT64>(i) + 3 * 1]].y;
				vertices[entryPos + 2].position.z = _position[_triangleIndex[static_cast<UINT64>(i) + 3 * 1]].z;
				vertices[entryPos + 2].position.w = 1.0f;

				// ModelMeshColliderを作るために必要
				// エラー避ける用
				int num = entryPos + 0;
				saveVertex[num].x = _position[_triangleIndex[static_cast<UINT64>(i) + 3 * 0]].x;
				saveVertex[num].y = _position[_triangleIndex[static_cast<UINT64>(i) + 3 * 0]].y;
				saveVertex[num].z = _position[_triangleIndex[static_cast<UINT64>(i) + 3 * 0]].z;

				num = entryPos + 1;
				saveVertex[num].x = _position[_triangleIndex[static_cast<UINT64>(i) + 3 * 2]].x;
				saveVertex[num].y = _position[_triangleIndex[static_cast<UINT64>(i) + 3 * 2]].y;
				saveVertex[num].z = _position[_triangleIndex[static_cast<UINT64>(i) + 3 * 2]].z;

				num = entryPos + 2;
				saveVertex[num].x = _position[_triangleIndex[static_cast<UINT64>(i) + 3 * 1]].x;
				saveVertex[num].y = _position[_triangleIndex[static_cast<UINT64>(i) + 3 * 1]].y;
				saveVertex[num].z = _position[_triangleIndex[static_cast<UINT64>(i) + 3 * 1]].z;

				for (UINT k = 0; k < 3; ++k)
				{
					vertices[entryPos + k].position.z *= -1.0f;
					num = entryPos + k;
					saveVertex[num].z *= -1.0f;
				}

				// 法線(再計算)
				XMVECTOR v0 = XMLoadFloat4(&vertices[entryPos + 0].position);
				XMVECTOR v1 = XMLoadFloat4(&vertices[entryPos + 1].position);
				XMVECTOR v2 = XMLoadFloat4(&vertices[entryPos + 2].position);
				XMVECTOR v3 = v0 - v1;
				XMVECTOR v4 = v2 - v1;
				XMVECTOR cross = XMVector3Cross(v4, v3);
				cross = XMVector3Normalize(cross);

				for (UINT k = 0; k < 3; ++k)
				{
					XMStoreFloat4(&vertices[entryPos + k].normal, cross);
				}

				// テクスチャーUV値
				if (_textureUV.size() > 0)
				{
					vertices[entryPos + 0].textureUV = _textureUV[_triangleIndex[static_cast<UINT64>(i) + 3 * 0 + 1]];
					vertices[entryPos + 1].textureUV = _textureUV[_triangleIndex[static_cast<UINT64>(i) + 3 * 2 + 1]];
					vertices[entryPos + 2].textureUV = _textureUV[_triangleIndex[static_cast<UINT64>(i) + 3 * 1 + 1]];
				}
				else
				{
					for (UINT k = 0; k < 3; ++k)
					{
						vertices[entryPos + k].textureUV = XMFLOAT2(0.0f, 0.0f);
					}
				}
			}
		}
	}

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
	resDesc.Width = sizeof(vertices[0]) * verticesNum;
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
		IID_PPV_ARGS(mesh->vertexBuffer .ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// 頂点情報のコピー
	VERTICES* vertexMap = nullptr;
	// 頂点バッファーのロック
	hr = mesh->vertexBuffer->Map(0, nullptr,
		(void**)&vertexMap);
	if (hr != S_OK)
		return hr;
	memcpy(vertexMap, vertices,
		sizeof(vertices[0]) * verticesNum);
	// 頂点バッファーの解除
	mesh->vertexBuffer->Unmap(0, nullptr);

	// 頂点バッファービューを作成
	// 頂点バッファーのアドレス[バッファーの仮想アドレス]
	mesh->vertexBufferView.BufferLocation = mesh->vertexBuffer->GetGPUVirtualAddress();
	// 総バイト数[全バイト数]
	mesh->vertexBufferView.SizeInBytes = sizeof(vertices[0]) * verticesNum;
	// 1頂点あたりのバイト数
	mesh->vertexBufferView.StrideInBytes = sizeof(vertices[0]);

	delete[] vertices;

	// インデックス情報を作成
	UINT indicesNum = verticesNum;
	unsigned short* indices = new unsigned short[indicesNum];
	::ZeroMemory(indices, sizeof(indices[0]) * indicesNum);
	for (UINT i = 0; i < indicesNum; ++i)
	{
		indices[i] = (unsigned short)i;
	}

	// サイズ幅[幅で全部まかなうのでsizeof(全インデックス)とする]
	resDesc.Width = sizeof(indices[0]) * indicesNum;

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
	memcpy(indexMap, indices, sizeof(indices[0]) * indicesNum);
	// インデックスバッファーの解除
	mesh->indexBuffer->Unmap(0, nullptr);

	// インデックスバッファービューを作成
	// インデックスバッファーのアドレス[バッファーの仮想アドレス]
	mesh->indexBufferView.BufferLocation = mesh->indexBuffer->GetGPUVirtualAddress();
	// 総バイト数[全バイト数]
	mesh->indexBufferView.SizeInBytes = sizeof(indices[0]) * indicesNum;
	// フォーマット
	mesh->indexBufferView.Format = DXGI_FORMAT_R16_UINT;

	delete[] indices;

	// マテリアルバッファーの生成
	if (FAILED(CreateMaterialBuffer(mesh)))
		return E_FAIL;

	// テクスチャーの登録
	// マテリアルが持つテクスチャー番号を設定
	// 但し、ディフューズテクスチャーのみ登録
	_fileTextures.resize(_material.size());
	mesh->textureBuffer.resize(_material.size());
	entryPos = 0;
	for (std::vector<MATERIAL>::size_type i = 0;
		i < _material.size(); ++i, ++entryPos)
	{
		// 登録されているか確認
		BOOL entryCheck = FALSE;	// 登録の有無
		UINT entryPosition = 0;
		for (std::vector<TEXTURE>::size_type j = 0;
			j < _fileTextures.size(); ++j, ++entryPosition)
		{
			if (lstrlenW(_fileTextures[j].filename) > 0)
			{
				if (lstrcmpW(_fileTextures[j].filename
					, _material[i].textureNameKd) == 0)
				{
					entryCheck = TRUE;
					break;
				}
			}
		}
		if (entryCheck)
		{
			mesh->textureBuffer[entryPos] =
				mesh->textureBuffer[entryPosition];
		}
		else
		{
			// パスとファイル名を結合
			TCHAR fillPathFilename[MAX_PATH] = _T("");
			if (lstrlenW(_material[i].textureNameKd) > 0)
			{
				lstrcpy(_fileTextures[entryPos].filename, _material[i].textureNameKd);
				lstrcpy(fillPathFilename, _path);
				lstrcat(fillPathFilename, _fileTextures[entryPos].filename);
			}

			// シェーダリソースの生成
			if (FAILED(CreateShaderResource(fillPathFilename, mesh->textureBuffer[entryPos])))
				return E_FAIL;
		}
	}

	return S_OK;
}

// ObjModel
void ObjModel::ObjModelDeleate(int& entryPos, const char* filename)
{
	if (entryPos != -1)
	{
		int EntryPos = entryPos % instanceMax;
		int ArrayNum = entryPos / instanceMax;
		for (std::vector<MATERIAL>::size_type k = 0;
			k < _saveMaterial[filename].size(); ++k)
		{
			_objMetaData[filename][ArrayNum][k].world[EntryPos] = {};
		}
		_objDataCheck[filename][ArrayNum]._dataCheck[EntryPos] = false;
		entryPos = -1;
	}
}

void ObjModel::SetObjModelMetaData(int& entryPos, VECTOR position, VECTOR angle, VECTOR scale, VECTOR color, const char* filename)
{

	int num = _objInstanceEntry[filename] % instanceMax;
	// 最初もしくは500番目まで使い切ったら、または初めてのfilenameなら次のバッファーを用意する
	if (num == 0 && entryPos == -1)
	{
		_objMeshs[filename].push_back(new MESH);

		// ファイル読み込み
		LoadObjFromFile(filename);
		// インデックスバーテックスバッファービューの生成
		Convert(_objMeshs[filename].back(), _saveVertices[filename]);
		// コンスタントバッファービューの生成
		CreateConstantBufferView(_objMeshs[filename].back());

		_objMetaData[filename].resize(_objMeshs[filename].size());
		_objDataCheck[filename].resize(_objMeshs[filename].size());

		// 情報を移す
		_saveAttribute[filename] = _attribute;
		_saveMaterial[filename] = _material;
		// 初期化
		_position.clear();	// 頂点座標
		_normal.clear();	// 法線
		_textureUV.clear();	// テクスチャーUV値
		_triangleIndex.clear();		// 三角形インデックス
		_quadrangleIndex.clear();	// 四角形インデックス
		_attribute.clear();	// 属性情報
		_material.clear();	// マテリアル情報		
		_attributeEntryFlag = FALSE;	// 属性情報登録の有無
		_materialEntryFlag = FALSE;		// マテリアル情報登録の有無
		delete _path;					// パス(フォルダー)

	}

	// エントリー番号
	int EntryPos;
	// 使用するメタデータの番号
	int ArrayNum;
	//// =====ワールド行列=====
	XMMATRIX world_matrix;
	XMMATRIX translate = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX rotate_x = XMMatrixRotationX(XMConvertToRadians(angle.x));
	XMMATRIX rotate_y = XMMatrixRotationY(XMConvertToRadians(angle.y));
	XMMATRIX rotate_z = XMMatrixRotationZ(XMConvertToRadians(angle.z));
	XMMATRIX scale_mat = XMMatrixScaling(scale.x, scale.y, scale.z);
	world_matrix = scale_mat * rotate_z * rotate_x * rotate_y * translate;
	if (entryPos == -1)
	{
		// 空いてるエントリー番号を探す
		for (int i = 0, size = (int)_objDataCheck[filename].size(); i < size; ++i)
		{
			for (int j = 0; j < instanceMax; ++j)
			{
				if (_objDataCheck[filename][i]._dataCheck[j] == false)
				{
					entryPos = i * instanceMax + j;
					if ((UINT)entryPos >= _objInstanceEntry[filename])
					{
						_objInstanceEntry[filename] = entryPos + 1;
					}
					_objDataCheck[filename][i]._dataCheck[j] = true;
					break;
				}
			}
		}

		EntryPos = entryPos % instanceMax;
		ArrayNum = entryPos / instanceMax;
		// 生成するライン情報代入
		_objMetaData[filename][ArrayNum].resize(_saveMaterial[filename].size());
		for (std::vector<MATERIAL>::size_type k = 0, size = (int)_saveMaterial[filename].size();
			k < size; ++k)
		{
			_objMetaData[filename][ArrayNum][k].world[EntryPos] = world_matrix;
			_objMetaData[filename][ArrayNum][k].diffuse[EntryPos] = _saveMaterial[filename][k].Kd;
			// あるなら代入無いなら仮に１
			if (_saveMaterial[filename][k].dissolve)
			{
				_objMetaData[filename][ArrayNum][k].diffuse[EntryPos].w = _saveMaterial[filename][k].dissolve;
			}
			else
			{
				_objMetaData[filename][ArrayNum][k].diffuse[EntryPos].w = 1;
			}
			_objMetaData[filename][ArrayNum][k].ambient[EntryPos] = _saveMaterial[filename][k].Ka;
			if (_saveMaterial[filename][k].textureNameKd[0] != '\0')
			{
				_objMetaData[filename][ArrayNum][k].ambient[EntryPos].x += 0.5f;
				_objMetaData[filename][ArrayNum][k].ambient[EntryPos].y += 0.5f;
				_objMetaData[filename][ArrayNum][k].ambient[EntryPos].z += 0.5f;
			}
			_objMetaData[filename][ArrayNum][k].specular[EntryPos] = _saveMaterial[filename][k].Ks;
			// あるなら代入無いなら仮に32
			if (_saveMaterial[filename][k].Ns)
			{
				_objMetaData[filename][ArrayNum][k].specular[EntryPos].w = _saveMaterial[filename][k].Ns;
			}
			else
			{
				_objMetaData[filename][ArrayNum][k].specular[EntryPos].w = 32;
			}
		}
	}
	else
	{
		EntryPos = entryPos % instanceMax;
		ArrayNum = entryPos / instanceMax;
		// 生成するライン情報代入
		_objMetaData[filename][ArrayNum].resize(_saveMaterial[filename].size());
		for (std::vector<MATERIAL>::size_type k = 0, size = (int)_saveMaterial[filename].size();
			k < size; ++k)
		{
			_objMetaData[filename][ArrayNum][k].world[EntryPos] = world_matrix;
			_objMetaData[filename][ArrayNum][k].diffuse[EntryPos] = _saveMaterial[filename][k].Kd;
			// あるなら代入無いなら仮に１
			if (_saveMaterial[filename][k].dissolve)
			{
				_objMetaData[filename][ArrayNum][k].diffuse[EntryPos].w = _saveMaterial[filename][k].dissolve;
			}
			else
			{
				_objMetaData[filename][ArrayNum][k].diffuse[EntryPos].w = 1;
			}
			_objMetaData[filename][ArrayNum][k].ambient[EntryPos] = _saveMaterial[filename][k].Ka;
			if (_saveMaterial[filename][k].textureNameKd[0] != '\0')
			{
				_objMetaData[filename][ArrayNum][k].ambient[EntryPos].x += 0.5f;
				_objMetaData[filename][ArrayNum][k].ambient[EntryPos].y += 0.5f;
				_objMetaData[filename][ArrayNum][k].ambient[EntryPos].z += 0.5f;
			}
			_objMetaData[filename][ArrayNum][k].specular[EntryPos] = _saveMaterial[filename][k].Ks;
			// あるなら代入無いなら仮に32
			if (_saveMaterial[filename][k].Ns)
			{
				_objMetaData[filename][ArrayNum][k].specular[EntryPos].w = _saveMaterial[filename][k].Ns;
			}
			else
			{
				_objMetaData[filename][ArrayNum][k].specular[EntryPos].w = 32;
			}
		}
	}
}