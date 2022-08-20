// --------------------- Include File --------------------
#include "Field.h"
#include "../Camera/Camera.h"
#include "../DX12System.h"
Field* Field::_instance = nullptr;			// インスタンスの実態
// コンストラクター
Field::Field()
	: _vertexBufferView(), _indexBufferView()
{

}
// デストラクター
Field::~Field()
{

}
// 生成
void Field::Create()
{
	if (!_instance)
		_instance = new Field();

	// ルートシグネチャーの生成
	_instance->CreateRootSignature();

	// 描画パイプラインの生成
	_instance->CreateGraphicsPipelineState();

	// 頂点/インデクスバッファービューの生成
	_instance->CreateVertexIndexBufferView();

	// コンスタントバッファービューの生成
	_instance->CreateConstantBufferView();

}
// 破棄
void Field::Destroy()
{
	delete _instance;
}

// ルートシグネチャーの生成
HRESULT Field::CreateRootSignature()
{
	HRESULT hr = S_OK;

	// ディスクリプターレンジの設定
	D3D12_DESCRIPTOR_RANGE descTblRange[1] = {};
	// ディスクリプターの数[コンスタントバッファー]
	descTblRange[0].NumDescriptors = 1;
	// 種類[定数]
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// 先頭レジスター番号[0番スロットから]
	descTblRange[0].BaseShaderRegister = 0;
	// 連続したディスクリプターレンジが前のディスクリプターレンジの直後に来る
	descTblRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ルートパラメーターの設定
	D3D12_ROOT_PARAMETER rootParam[1] = {};
	// ルートパラメーターの種類[今回はディスクリプターテーブルとして利用]
	rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// どのシェーダーから利用可能か[全シェーダー]
	rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// ディスクリプターレンジのアドレス
	rootParam[0].DescriptorTable.pDescriptorRanges = &descTblRange[0];
	// ディスクリプターレンジの数
	rootParam[0].DescriptorTable.NumDescriptorRanges = 1;

	// サンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc[2] = {};
	// 横方向の繰り返し
	samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// 縦方向の繰り返し
	samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// 奥行きの繰り返し
	samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// ボーダーは黒
	samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	// 線形補間
	samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	// ミップマップ最大値
	samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
	// ミップマップ最小値
	samplerDesc[0].MinLOD = 0.0f;
	// ピクセルシェーダーから見える
	samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	// シェーダースロット番号
	samplerDesc[0].ShaderRegister = 0;
	// リサンプリングしない
	samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	// 変更点以外をコピー
	samplerDesc[1] = samplerDesc[0];
	// 横方向の繰り返し
	samplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	// 縦方向の繰り返し
	samplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	// 奥行きの繰り返し
	samplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	// シェーダースロット番号
	samplerDesc[1].ShaderRegister = 1;

	// ルートシグネチャーの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	// フラグ[頂点情報(入力アセンブラ)がある]
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	// 作成するルートパラメーター配列の先頭アドレス
	rootSignatureDesc.pParameters = rootParam;
	// ルートパラメーターの数
	rootSignatureDesc.NumParameters = 1;
	// 作成するルートサンプラー配列の先頭アドレス
	rootSignatureDesc.pStaticSamplers = samplerDesc;
	// ルートサンプラーの数
	rootSignatureDesc.NumStaticSamplers = 2;

	ID3DBlob* rootSigBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// バイナリコードを作成
	hr = D3D12SerializeRootSignature(
		&rootSignatureDesc,				// ルートシグネチャー設定
		D3D_ROOT_SIGNATURE_VERSION_1_0,	// ルートシグネチャーバージョン
		&rootSigBlob,					// 受け取るためのポインターのアドレス
		&errorBlob);					// エラー用ポインターのアドレス[エラー時はerrorBlobにメッセージが入る]
	if (FAILED(hr))
		return hr;

	// ルートシグネチャーオブジェクトの作成
	hr = DX12::GetInstance()->GetDevice()->CreateRootSignature(
		0,	// nodemask。０でよい
		rootSigBlob->GetBufferPointer(),	// アドレス
		rootSigBlob->GetBufferSize(),		// サイズ
		IID_PPV_ARGS(_rootSignature.ReleaseAndGetAddressOf()));		// アドレスを措定
	if (FAILED(hr))
		return hr;

	rootSigBlob->Release();		// 不要になったので解放

	return hr;
}
// 描画パイプラインの生成
HRESULT Field::CreateGraphicsPipelineState()
{
	HRESULT hr = S_OK;

	// シェーダーオブジェクト読み込み用
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// 頂点シェーダーの読み込み
	hr = D3DCompileFromFile(
		L".\\Shader\\Field\\FieldVertexShader.hlsl",	// シェーダー名
		nullptr,							// defineはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// インクルードはデフォルト
		"FieldVS", "vs_5_0",				// 関数は BasicVS、対象シェーダーは vs_5_0
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
		L".\\Shader\\Field\\FieldPixelShader.hlsl",	// シェーダー名
		nullptr,							// defineはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// インクルードはデフォルト
		"FieldPS", "ps_5_0",				// 関数は BasicPS、対象シェーダーは ps_5_0
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
		{	// color情報
			"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
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

	// プリミティブのタイプ[ライン]
	gpipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

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

	// 深度バッファーの設定
	gpipelineDesc.DepthStencilState.DepthEnable = TRUE;	// 深度バッファーを使う
	gpipelineDesc.DepthStencilState.DepthWriteMask
		= D3D12_DEPTH_WRITE_MASK_ALL;	// 書き込む
	gpipelineDesc.DepthStencilState.DepthFunc =
		D3D12_COMPARISON_FUNC_LESS;		// 小さいほうを採用
	gpipelineDesc.DSVFormat =
		DXGI_FORMAT_D32_FLOAT;	// 32bit floatを深度値として使用

	// グラフィックパイプラインステートオブジェクトを生成
	hr = DX12::GetInstance()->GetDevice()->CreateGraphicsPipelineState(
		&gpipelineDesc, IID_PPV_ARGS(_pipelineState.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	return hr;
}
// 頂点/インデックスバッファービューの生成
HRESULT Field::CreateVertexIndexBufferView()
{
	HRESULT hr = S_OK;

	const float gridSize = 1.0f;// グリッドサイズ
	const int divideNum = 99;	// 分割数(奇数)

	// 頂点情報を作成
	struct VECTOR4 : public XMFLOAT4
	{
	public:
		VECTOR4() {};
		VECTOR4(FLOAT fx, FLOAT fy, FLOAT fz, FLOAT fw)
		{
			x = fx;
			y = fy;
			z = fz;
			w = fw;
		}

		// 代入演算子
		VECTOR4& operator += (CONST VECTOR4& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
			w = 1.0f;
			return *this;
		}
		// 2 項演算子
		VECTOR4 operator + (CONST VECTOR4& v) const
		{
			return VECTOR4(x + v.x, y + v.y, z + v.z, 1.0f);
		}
	};

	// 頂点バッファ生成
	FIELDVERTEX vertices[(divideNum + 2) * 2 * 2] = {};
	// 原点を中心としてXZ平面上に格子状の枠生成
	UINT entryPos = 0;
	// 左～X軸(左->右)を生成
	VECTOR4 startPosition(-(divideNum + 2) / 2 * gridSize, -0.001f, 0.0f, 0.0f);
	for (UINT i = 0; i < (divideNum + 2); i++, entryPos += 2)
	{
		vertices[entryPos + 0].position = startPosition + VECTOR4(static_cast<FLOAT>(i) * gridSize, 0.0f, +(divideNum + 2) / 2 * gridSize, 0.0f);
		vertices[entryPos + 1].position = startPosition + VECTOR4(static_cast<FLOAT>(i) * gridSize, 0.0f, -(divideNum + 2) / 2 * gridSize, 0.0f);
		vertices[entryPos + 0].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertices[entryPos + 1].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		if ((divideNum + 2) / 2 == i)
		{
			vertices[entryPos + 0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
			vertices[entryPos + 1].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		}
	}

	// 上～Y軸(奥->手前)を生成
	startPosition = VECTOR4(0.0f, -0.001f, (divideNum + 2) / 2 * gridSize, 0.0f);
	for (UINT i = 0; i < (divideNum + 2); i++, entryPos += 2)
	{
		vertices[entryPos + 0].position = startPosition + VECTOR4(-(divideNum + 2) / 2 * gridSize, 0.0f, -static_cast<FLOAT>(i) * gridSize, 0.0f);
		vertices[entryPos + 1].position = startPosition + VECTOR4(+(divideNum + 2) / 2 * gridSize, 0.0f, -static_cast<FLOAT>(i) * gridSize, 0.0f);
		vertices[entryPos + 0].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertices[entryPos + 1].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		if ((divideNum + 2) / 2 == i)
		{
			vertices[entryPos + 0].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
			vertices[entryPos + 1].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
		}
	}
	for (int i = 0; i < (divideNum + 2) * 2 * 2; ++i)
	{
		_vertices.push_back(vertices[i]);
	}

	// 頂点インデクス情報を作成
	for (int i = 0; i < (divideNum + 2) * 2 * 2; ++i)
	{
		_indices.push_back((unsigned short)i);
	}

	// 頂点バッファー
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
	resDesc.Width = _vertices.size() * sizeof(_vertices[0]);
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
		IID_PPV_ARGS(_vertexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// リソースオブジェクトに頂点情報をコピー
	FIELDVERTEX* vertexMap = nullptr;
	// バッファーのアドレスを取得し、ロック[全範囲指定]
	hr = _vertexBuffer->Map(0, nullptr, (void**)&vertexMap);
	if (FAILED(hr))
		return hr;
	// コピー
	std::copy(std::begin(_vertices), std::end(_vertices), vertexMap);
	// ロック解除
	_vertexBuffer->Unmap(0, nullptr);

	// 頂点バッファービューの設定
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	// 頂点バッファーのアドレス
	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
	// １頂点あたりのバイト数
	_vertexBufferView.StrideInBytes = (UINT)sizeof(_vertices[0]);	// 頂点１つあたりのサイズ
	// 総バイト数
	_vertexBufferView.SizeInBytes = (UINT)(_vertices.size() * sizeof(_vertices[0]));	// 全バイト数

	// インデックスバッファー
	// リソースサイズ変更
	resDesc.Width = _indices.size() * sizeof(_indices[0]);
	// リソースオブジェクトを作成
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_indexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// リソースオブジェクトにインデックス情報をコピー
	unsigned short* mappedIdx = nullptr;
	// バッファーのアドレスを取得し、ロック[全範囲指定]
	hr = _indexBuffer->Map(0, nullptr, (void**)&mappedIdx);
	if (FAILED(hr))
		return hr;
	// コピー
	std::copy(std::begin(_indices), std::end(_indices), mappedIdx);
	// ロック解除
	_indexBuffer->Unmap(0, nullptr);

	// インデックスバッファービューの設定
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	// インデックスバッファーのアドレス
	_indexBufferView.BufferLocation = _indexBuffer->GetGPUVirtualAddress();
	// フォーマット
	_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	// 全バイト数
	_indexBufferView.SizeInBytes = (UINT)(_indices.size() * sizeof(_indices[0]));

	return hr;
}
// コンスタントバッファービューの生成
HRESULT Field::CreateConstantBufferView()
{
	HRESULT hr = S_OK;

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
	resDesc.Width = (sizeof(SceneMatrix) + 0xff) & ~0xff;
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

	// コンスタントバッファーを作成
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,	// 特に指定なし
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,	// CPUからの書き込み可能、GPUから見ると読み取りのみ
		nullptr,
		IID_PPV_ARGS(_constBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// デスクリプタヒープを作成
	// ヒープの記述を用意
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	// フラグ[シェーダー側から見えるように]
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// GPUを１つだけ使用する想定
	descHeapDesc.NodeMask = 0;
	// デスクリプタヒープの数[CBV1つ]
	descHeapDesc.NumDescriptors = 1;
	// 種類[シェーダーリソースビュー用]
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	// 記述子ヒープを作成
	hr = DX12::GetInstance()->GetDevice()->CreateDescriptorHeap(&descHeapDesc
		, IID_PPV_ARGS(_basicDescHeap.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// ディスクリプタの先頭ハンドルを取得しておく
	auto basicHeapHandle = _basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = _constBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)_constBuffer->GetDesc().Width;

	// 定数バッファービューの作成
	DX12::GetInstance()->GetDevice()->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	return hr;
}
// 描画
void Field::Draw()
{
	XMMATRIX worldMat = XMMatrixIdentity();

	// ビュー行列
	XMMATRIX viewMat = Camera::GetInstance()->GetView();

	// プロジェクション行列
	XMMATRIX projectionMat = Camera::GetInstance()->GetProjection();

	// カメラ座標
	XMFLOAT3 eyePosition = Camera::GetInstance()->GetEyePosition();

	SceneMatrix* mapMatrix = nullptr;	// マップ先を示すポインター
	HRESULT hr = _constBuffer->Map(0, nullptr, (void**)&mapMatrix);	// マップ
	// 行列の内容をコピー
	mapMatrix->world = worldMat;
	mapMatrix->view = viewMat;
	mapMatrix->projection = projectionMat;
	mapMatrix->eye = eyePosition;

	// グラフィックスパイプラインステートの指定
	DX12::GetInstance()->CommandList()->SetPipelineState(_pipelineState.Get());
	// ルートシグネチャーの指定
	DX12::GetInstance()->CommandList()->SetGraphicsRootSignature(_rootSignature.Get());


	// ディスクリプターヒープの指定
	DX12::GetInstance()->CommandList()->SetDescriptorHeaps(1, _basicDescHeap.GetAddressOf());

	// ルートパラメーターとディスクリプターヒープの関連付け
	DX12::GetInstance()->CommandList()->SetGraphicsRootDescriptorTable(
		0,	// ルートパラメーターインデックス
		_basicDescHeap->GetGPUDescriptorHandleForHeapStart());    // ヒープアドレス

	// 頂点情報の指定[ライン]
	DX12::GetInstance()->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	// 描画(Draw関数)命令
	// 頂点バッファーの指定
	DX12::GetInstance()->CommandList()->IASetVertexBuffers(0, 1, &_vertexBufferView);
	// インデックスバッファーの指定
	DX12::GetInstance()->CommandList()->IASetIndexBuffer(&_indexBufferView);

	// 最初はオフセットなし
	unsigned int idxOffset = 0;

	// 描画命令
	DX12::GetInstance()->CommandList()->DrawIndexedInstanced((UINT)_indices.size(), 1, idxOffset, 0, 0);
}