// --------------------- Include File --------------------
#include "Mesh.h"
#include "GeometryData.h"
#include "../Camera/Camera.h"
#include "../DX12System.h"
#include "../VECTOR.h"
Mesh* Mesh::_instance = nullptr;			// インスタンスの実態
// コンストラクター
Mesh::Mesh()
{
	boxVertexBuffer = nullptr;			// 頂点バッファー
	boxIndexBuffer = nullptr;				// インデックスバッファー

	boxVertexBufferView = {};		// 頂点バッファービュー
	boxIndexBufferView = {};	    // インデックスバッファービュー

	cylinderVertexBuffer = nullptr;			// 頂点バッファー
	cylinderIndexBuffer = nullptr;				// インデックスバッファー

	cylinderVertexBufferView = {};		// 頂点バッファービュー
	cylinderIndexBufferView = {};	    // インデックスバッファービュー

	sphereVertexBuffer = nullptr;			// 頂点バッファー
	sphereIndexBuffer = nullptr;				// インデックスバッファー

	sphereVertexBufferView = {};		// 頂点バッファービュー
	sphereIndexBufferView = {};	    // インデックスバッファービュー


	circleVertexBuffer = nullptr;			// 頂点バッファー
	circleIndexBuffer = nullptr;				// インデックスバッファー

	circleVertexBufferView = {};		// 頂点バッファービュー
	circleIndexBufferView = {};	    // インデックスバッファービュー
}
// デストラクター
Mesh::~Mesh()
{
	for (auto mesh : _boxMeshs)
	{
		delete mesh;
	}
	for (auto mesh : _cylinderMeshs)
	{
		delete mesh;
	}
	for (auto mesh : _sphereMeshs)
	{
		delete mesh;
	}
	for (auto mesh : _circleMeshs)
	{
		delete mesh;
	}
}
// 生成
void Mesh::Create()
{
	if (!_instance)
	{
		_instance = new Mesh();

		// ルートシグネチャーの生成
		_instance->CreateRootSignature();

		// 描画パイプラインの生成
		_instance->CreateGraphicsPipelineState();

		// 頂点/インデクスバッファービューの生成
		_instance->CreateVertexIndexBufferView();

	}
}
// 破棄
void Mesh::Destroy()
{
	delete _instance;
}
// ルートシグネチャーの生成
HRESULT Mesh::CreateRootSignature()
{
	HRESULT hr = S_OK;

	// ディスクリプターレンジの設定
	D3D12_DESCRIPTOR_RANGE descTblRange[2] = {};
	// ディスクリプターの数[コンスタントバッファー]
	descTblRange[0].NumDescriptors = 1;
	// 種類[定数]
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// 先頭レジスター番号[0番スロットから]
	descTblRange[0].BaseShaderRegister = 0;
	// 連続したディスクリプターレンジが前のディスクリプターレンジの直後に来る
	descTblRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ディスクリプターの数[コンスタントバッファー]
	descTblRange[1].NumDescriptors = 1;
	// 種類[定数]
	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// 先頭レジスター番号[1番スロットから]
	descTblRange[1].BaseShaderRegister = 1;
	// 連続したディスクリプターレンジが前のディスクリプターレンジの直後に来る
	descTblRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ルートパラメーターの設定
	D3D12_ROOT_PARAMETER rootParam[2] = {};
	// ルートパラメーターの種類[今回はディスクリプターテーブルとして利用]
	rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// どのシェーダーから利用可能か[全シェーダー]
	rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// ディスクリプターレンジのアドレス
	rootParam[0].DescriptorTable.pDescriptorRanges = &descTblRange[0];
	// ディスクリプターレンジの数
	rootParam[0].DescriptorTable.NumDescriptorRanges = 1;
	// ルートパラメーターの種類[今回はディスクリプターテーブルとして利用]
	rootParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// どのシェーダーから利用可能か[全シェーダー]
	rootParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// ディスクリプターレンジのアドレス
	rootParam[1].DescriptorTable.pDescriptorRanges = &descTblRange[1];
	// ディスクリプターレンジの数
	rootParam[1].DescriptorTable.NumDescriptorRanges = 1;

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
	rootSignatureDesc.NumParameters = 2;
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

	return S_OK;
}

// 描画パイプラインの生成
HRESULT Mesh::CreateGraphicsPipelineState()
{
	HRESULT hr = S_OK;

	// シェーダーオブジェクト読み込み用
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// 頂点シェーダーの読み込み
	hr = D3DCompileFromFile(
		L".\\Shader\\Mesh\\MeshVertexShader.hlsl",			// シェーダー名
		nullptr,							// defineはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// インクルードはデフォルト
		"MeshVS", "vs_5_0",				// 関数は BasicVS、対象シェーダーは vs_5_0
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
		L".\\Shader\\Mesh\\MeshPixelShader.hlsl",			// シェーダー名
		nullptr,							// defineはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// インクルードはデフォルト
		"MeshPS", "ps_5_0",				// 関数は BasicPS、対象シェーダーは ps_5_0
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
		{	// COLOR情報
			"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
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

	// 深度バッファーの有効の有無[有効]
	gpipelineDesc.DepthStencilState.DepthEnable = true;
	// 深度ステンシルバッファーの書き込み方法[書き込み有効]
	gpipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 深度ステンシルバッファーの制御方法[小さいほうを採用]
	gpipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
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
HRESULT Mesh::CreateVertexIndexBufferView()
{
	HRESULT hr = S_OK;

	// ====================================== 箱 =========================================

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
	resDesc.Width = sizeof(MESHVERTEX) * BoxNumIndices;

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
		IID_PPV_ARGS(boxVertexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;
	UINT polygonNum = 3;		// 面形成のポリゴン数
	MESHVERTEX boxVertices[BoxNumVertices] = {};
	for (int i = 0; i < BoxNumVertices; ++i)
	{
		boxVertices[i].position.x = BoxVertices[i * polygonNum + 0];
		boxVertices[i].position.y = BoxVertices[i * polygonNum + 1];
		boxVertices[i].position.z = BoxVertices[i * polygonNum + 2];
		boxVertices[i].position.w = 1.0f;
		boxVertices[i].normal.x = BoxNormals[i * polygonNum + 0];
		boxVertices[i].normal.y = BoxNormals[i * polygonNum + 1];
		boxVertices[i].normal.z = BoxNormals[i * polygonNum + 2];
		boxVertices[i].normal.w = 1.0f;
	}

	// 頂点情報のコピー
	MESHVERTEX* vertexMap = nullptr;
	// 頂点バッファーのロック
	hr = boxVertexBuffer->Map(0, nullptr,
		(void**)&vertexMap);
	if (hr != S_OK)
		return hr;

	memcpy(vertexMap, boxVertices, sizeof(MESHVERTEX) * BoxNumVertices);

	// 頂点バッファーの解除
	boxVertexBuffer->Unmap(0, nullptr);

	// 頂点バッファービューを作成
	// 頂点バッファーのアドレス[バッファーの仮想アドレス]
	boxVertexBufferView.BufferLocation = boxVertexBuffer->GetGPUVirtualAddress();
	// 総バイト数[全バイト数]
	boxVertexBufferView.SizeInBytes = sizeof(MESHVERTEX) * BoxNumVertices;

	// 1頂点あたりのバイト数
	boxVertexBufferView.StrideInBytes = sizeof(MESHVERTEX);

	// サイズ幅[幅で全部まかなうのでsizeof(全インデックス)とする]
	resDesc.Width = sizeof(BoxIndices);


	// リソースオブジェクトを作成
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(boxIndexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// インデックス情報のコピー
	unsigned short* indexMap = nullptr;
	// インデックスバッファーのロック
	hr = boxIndexBuffer->Map(0, nullptr,
		(void**)&indexMap);
	if (hr != S_OK)
		return hr;
	std::copy(std::begin(BoxIndices), std::end(BoxIndices), indexMap);

	// インデックスバッファーの解除
	boxIndexBuffer->Unmap(0, nullptr);

	// インデックスバッファービューを作成
	// インデックスバッファーのアドレス[バッファーの仮想アドレス]
	boxIndexBufferView.BufferLocation = boxIndexBuffer->GetGPUVirtualAddress();
	// 総バイト数[全バイト数]
	boxIndexBufferView.SizeInBytes = sizeof(BoxIndices);
	// フォーマット
	boxIndexBufferView.Format = DXGI_FORMAT_R16_UINT;

	// ====================================== 円柱 =========================================

	// サイズ幅[幅で全部まかなうのでsizeof(全頂点)とする]
	resDesc.Width = sizeof(MESHVERTEX) * CylinderNumIndices;


	// リソースオブジェクトを作成
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(cylinderVertexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// 頂点バッファ生成
	MESHVERTEX cylinderVertices[CylinderNumVertices] = {};
	for (int i = 0; i < CylinderNumVertices; ++i)
	{
		cylinderVertices[i].position.x = CylinderVertices[i * polygonNum + 0];
		cylinderVertices[i].position.y = CylinderVertices[i * polygonNum + 1];
		cylinderVertices[i].position.z = CylinderVertices[i * polygonNum + 2];
		cylinderVertices[i].position.w = 1.0f;
		cylinderVertices[i].normal.x = CylinderNormals[i * polygonNum + 0];
		cylinderVertices[i].normal.y = CylinderNormals[i * polygonNum + 1];
		cylinderVertices[i].normal.z = CylinderNormals[i * polygonNum + 2];
		cylinderVertices[i].normal.w = 1.0f;
	}

	// 頂点情報のコピー
	vertexMap = nullptr;
	// 頂点バッファーのロック
	hr = cylinderVertexBuffer->Map(0, nullptr,
		(void**)&vertexMap);
	if (hr != S_OK)
		return hr;

	memcpy(vertexMap, cylinderVertices, sizeof(MESHVERTEX) * CylinderNumVertices);

	// 頂点バッファーの解除
	cylinderVertexBuffer->Unmap(0, nullptr);

	// 頂点バッファービューを作成
	// 頂点バッファーのアドレス[バッファーの仮想アドレス]
	cylinderVertexBufferView.BufferLocation = cylinderVertexBuffer->GetGPUVirtualAddress();
	// 総バイト数[全バイト数]
	cylinderVertexBufferView.SizeInBytes = sizeof(MESHVERTEX) * CylinderNumVertices;

	// 1頂点あたりのバイト数
	cylinderVertexBufferView.StrideInBytes = sizeof(MESHVERTEX);

	// サイズ幅[幅で全部まかなうのでsizeof(全インデックス)とする]
	resDesc.Width = sizeof(CylinderIndices);



	// リソースオブジェクトを作成
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(cylinderIndexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// インデックス情報のコピー
	indexMap = nullptr;
	// インデックスバッファーのロック
	hr = cylinderIndexBuffer->Map(0, nullptr,
		(void**)&indexMap);
	if (hr != S_OK)
		return hr;

	std::copy(std::begin(CylinderIndices), std::end(CylinderIndices), indexMap);

	// インデックスバッファーの解除
	cylinderIndexBuffer->Unmap(0, nullptr);

	// インデックスバッファービューを作成
	// インデックスバッファーのアドレス[バッファーの仮想アドレス]
	cylinderIndexBufferView.BufferLocation = cylinderIndexBuffer->GetGPUVirtualAddress();
	// 総バイト数[全バイト数]
	cylinderIndexBufferView.SizeInBytes = sizeof(CylinderIndices);
	// フォーマット
	cylinderIndexBufferView.Format = DXGI_FORMAT_R16_UINT;


	// ====================================== 球 =========================================
	
	// サイズ幅[幅で全部まかなうのでsizeof(全頂点)とする]
	resDesc.Width = sizeof(MESHVERTEX) * SphereNumVertices;

	// リソースオブジェクトを作成
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(sphereVertexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// 頂点バッファ生成
	MESHVERTEX sphereVertices[SphereNumVertices] = {};
	for (int i = 0; i < SphereNumVertices; ++i)
	{
		sphereVertices[i].position.x = SphereVertices[i * polygonNum + 0];
		sphereVertices[i].position.y = SphereVertices[i * polygonNum + 1];
		sphereVertices[i].position.z = SphereVertices[i * polygonNum + 2];
		sphereVertices[i].position.w = 1.0f;
		sphereVertices[i].normal.x = SphereNormals[i * polygonNum + 0];
		sphereVertices[i].normal.y = SphereNormals[i * polygonNum + 1];
		sphereVertices[i].normal.z = SphereNormals[i * polygonNum + 2];
		sphereVertices[i].normal.w = 1.0f;
	}
	// 頂点情報のコピー
	vertexMap = nullptr;
	// 頂点バッファーのロック
	hr = sphereVertexBuffer->Map(0, nullptr,
		(void**)&vertexMap);
	if (hr != S_OK)
		return hr;

	memcpy(vertexMap, sphereVertices, sizeof(MESHVERTEX) * SphereNumVertices);

	// 頂点バッファーの解除
	sphereVertexBuffer->Unmap(0, nullptr);

	// 頂点バッファービューを作成
	// 頂点バッファーのアドレス[バッファーの仮想アドレス]
	sphereVertexBufferView.BufferLocation = sphereVertexBuffer->GetGPUVirtualAddress();
	// 総バイト数[全バイト数]
	sphereVertexBufferView.SizeInBytes = sizeof(MESHVERTEX) * SphereNumVertices;

	// 1頂点あたりのバイト数
	sphereVertexBufferView.StrideInBytes = sizeof(MESHVERTEX);

	// サイズ幅[幅で全部まかなうのでsizeof(全インデックス)とする]
	resDesc.Width = sizeof(SphereIndices);



	// リソースオブジェクトを作成
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(sphereIndexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// インデックス情報のコピー
	indexMap = nullptr;
	// インデックスバッファーのロック
	hr = sphereIndexBuffer->Map(0, nullptr,
		(void**)&indexMap);
	if (hr != S_OK)
		return hr;

	std::copy(std::begin(SphereIndices), std::end(SphereIndices), indexMap);


	// インデックスバッファーの解除
	sphereIndexBuffer->Unmap(0, nullptr);

	// インデックスバッファービューを作成
	// インデックスバッファーのアドレス[バッファーの仮想アドレス]
	sphereIndexBufferView.BufferLocation = sphereIndexBuffer->GetGPUVirtualAddress();
	// 総バイト数[全バイト数]
	sphereIndexBufferView.SizeInBytes = sizeof(SphereIndices);
	// フォーマット
	sphereIndexBufferView.Format = DXGI_FORMAT_R16_UINT;

	// ====================================== 円 =========================================

	// 頂点バッファ生成
	const unsigned int  VerticesNum = 36 + 1;	// 1は中心点
	MESHVERTEX circleVertices[VerticesNum] = {};
	for (int i = 0; i < VerticesNum; ++i)
	{
		if (i == (VerticesNum - 1))
		{
			// 中心点
			circleVertices[i].position.x = 0;
			circleVertices[i].position.y = 0;
			circleVertices[i].position.z = 0;
			circleVertices[i].position.w = 1.0f;
			circleVertices[i].normal.x = 0;
			circleVertices[i].normal.y = 0;
			circleVertices[i].normal.z = 1;
			circleVertices[i].normal.w = 1.0f;
		}
		else
		{
			float radian = 360 / (float)(VerticesNum - 1) * i;
			circleVertices[i].position.x = cosf(XMConvertToRadians(radian));
			circleVertices[i].position.y = sinf(XMConvertToRadians(radian));
			circleVertices[i].position.z = 0;
			circleVertices[i].position.w = 1.0f;
			circleVertices[i].normal.x = 0;
			circleVertices[i].normal.y = 0;
			circleVertices[i].normal.z = -1;
			circleVertices[i].normal.w = 1.0f;
		}
	}

	// サイズ幅[幅で全部まかなうのでsizeof(全頂点)とする]
	resDesc.Width = sizeof(MESHVERTEX) * VerticesNum;

	// リソースオブジェクトを作成
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(circleVertexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// 頂点情報のコピー
	vertexMap = nullptr;
	// 頂点バッファーのロック
	hr = circleVertexBuffer->Map(0, nullptr,
		(void**)&vertexMap);
	if (hr != S_OK)
		return hr;

	memcpy(vertexMap, circleVertices, sizeof(MESHVERTEX) * VerticesNum);

	// 頂点バッファーの解除
	circleVertexBuffer->Unmap(0, nullptr);

	// 頂点バッファービューを作成
	// 頂点バッファーのアドレス[バッファーの仮想アドレス]
	circleVertexBufferView.BufferLocation = circleVertexBuffer->GetGPUVirtualAddress();
	// 総バイト数[全バイト数]
	circleVertexBufferView.SizeInBytes = sizeof(MESHVERTEX) * VerticesNum;

	// 1頂点あたりのバイト数
	circleVertexBufferView.StrideInBytes = sizeof(MESHVERTEX);

	_circleIndicesNum = (VerticesNum - 1) * polygonNum;
	std::vector<unsigned short> circleIndices(_circleIndicesNum);
	int loopNum = 0;	// ループ回数
	for (UINT i = 0; i < _circleIndicesNum; i += polygonNum, ++loopNum)
	{
		int num = i + 0;
		circleIndices[num] = loopNum + 0;
		num = i + 1;
		if ((loopNum) == VerticesNum - 2)
		{
			// 最後のループでは最初の位置に戻るので0
			circleIndices[num] = 0;
		}
		else
		{
			circleIndices[num] = loopNum + 1;
		}
		num = i + 2;
		circleIndices[num] = VerticesNum - 1;
	}

	// サイズ幅[幅で全部まかなうのでsizeof(全インデックス)とする]
	resDesc.Width = sizeof(unsigned short) * circleIndices.size();

	// リソースオブジェクトを作成
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(circleIndexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// インデックス情報のコピー
	indexMap = nullptr;
	// インデックスバッファーのロック
	hr = circleIndexBuffer->Map(0, nullptr,
		(void**)&indexMap);
	if (hr != S_OK)
		return hr;

	std::copy(std::begin(circleIndices), std::end(circleIndices), indexMap);


	// インデックスバッファーの解除
	circleIndexBuffer->Unmap(0, nullptr);

	// インデックスバッファービューを作成
	// インデックスバッファーのアドレス[バッファーの仮想アドレス]
	circleIndexBufferView.BufferLocation = circleIndexBuffer->GetGPUVirtualAddress();
	// 総バイト数[全バイト数]
	circleIndexBufferView.SizeInBytes = sizeof(unsigned short) * (unsigned short)circleIndices.size();
	// フォーマット
	circleIndexBufferView.Format = DXGI_FORMAT_R16_UINT;

	return S_OK;
}
// マテリアルバッファーの生成
HRESULT Mesh::CreateMaterialBuffer(MESH* mesh)
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
	resDesc.Width = materialBufferSize;
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
		&uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,	// 特に指定なし
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,	// CPUからの書き込み可能、GPUから見ると読み取りのみ
		nullptr,
		IID_PPV_ARGS(mesh->materialBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	return hr;
}
// コンスタントバッファービューの生成
HRESULT Mesh::CreateConstantBufferView(MESH* mesh)
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
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
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
	// デスクリプタを指定[CBV1つ + マテリアル数 * 1]
	heapDesc.NumDescriptors = 2;
	// 特に指定なし[シェーダー側から見える必要あり]
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// デスクリプタヒープを作成
	hr = DX12::GetInstance()->GetDevice()->CreateDescriptorHeap(&heapDesc
		, IID_PPV_ARGS(mesh->basicDescHeap.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// ディスクリプタの先頭ハンドルを取得しておく
	auto basicHeapHandle = mesh->basicDescHeap->GetCPUDescriptorHandleForHeapStart();


	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = mesh->constBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)mesh->constBuffer->GetDesc().Width;

	// 定数バッファービューの作成
	DX12::GetInstance()->GetDevice()->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	// 次の場所に移動
	basicHeapHandle.ptr +=
		DX12::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize
		(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// マテリアル用バッファーを作成
	auto materialBufferSize = sizeof(MetaData);
	materialBufferSize = (materialBufferSize + 0xff) & ~0xff;

	D3D12_CONSTANT_BUFFER_VIEW_DESC matCBVDesc = {};
	matCBVDesc.BufferLocation = mesh->materialBuffer->GetGPUVirtualAddress();
	matCBVDesc.SizeInBytes = (UINT)materialBufferSize;

	// マテリアリル(CBV)
	DX12::GetInstance()->GetDevice()->CreateConstantBufferView(&matCBVDesc
		, basicHeapHandle);

	return S_OK;
}
// 描画(箱)
void Mesh::DrawBox()
{
	if (_boxInstanceEntry == 0) {
		return;
	}
	for (int m = 0, size = (int)_boxMeshs.size(); m < size; ++m)
	{

		int EntryNum = _boxInstanceEntry - m * instanceMax;
		if (EntryNum >= instanceMax)
		{
			EntryNum = instanceMax;
		}

		MetaData* mapMetaData = nullptr;	// マップ先を示すポインター
		_boxMeshs[m]->materialBuffer->Map(0, nullptr, (void**)&mapMetaData);	// マップ
		*mapMetaData = _boxMetaData[m];
		_boxMeshs[m]->materialBuffer->Unmap(0, nullptr);		// アンマップ

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
		ConstantBuffer* constBuffer = nullptr;	// マップ先を示すポインター
		_boxMeshs[m]->constBuffer->Map(0, nullptr, (void**)&constBuffer);	// マップ
		constBuffer->view = viewMat;
		constBuffer->projection = projMat;
		constBuffer->eye = eye;
		constBuffer->lightVector = lightVector;
		constBuffer->lightColor = lightColor;
		_boxMeshs[m]->constBuffer->Unmap(0, nullptr);		// アンマップ

		// グラフィックスパイプラインステートの指定
		DX12::GetInstance()->CommandList()->SetPipelineState(_pipelineState.Get());
		// ルートシグネチャーの指定
		DX12::GetInstance()->CommandList()->SetGraphicsRootSignature(_rootSignature.Get());

		// ディスクリプターヒープの指定
		DX12::GetInstance()->CommandList()->SetDescriptorHeaps(1, _boxMeshs[m]->basicDescHeap.GetAddressOf());

		// ルートパラメーターとディスクリプターヒープの関連付け
		DX12::GetInstance()->CommandList()->SetGraphicsRootDescriptorTable(
			0,	// ルートパラメーターインデックス
			_boxMeshs[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart());    // ヒープアドレス

		// 頂点情報の指定[三角形]
		DX12::GetInstance()->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 描画(Draw関数)命令
		// 頂点バッファーの指定
		DX12::GetInstance()->CommandList()->IASetVertexBuffers(0, 1, &_boxMeshs[m]->vertexBufferView);
		// インデックスバッファーの指定
		DX12::GetInstance()->CommandList()->IASetIndexBuffer(&_boxMeshs[m]->indexBufferView);
		// ディスクリプタの先頭ハンドルを取得しておく
		auto basicHeapHandle = _boxMeshs[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart();
		// 次の場所に移動
		basicHeapHandle.ptr +=
			DX12::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize
			(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// 最初はオフセットなし
		unsigned int idxOffset = 0;

		// ルートパラメーターとディスクリプターヒープの関連付け
		DX12::GetInstance()->CommandList()->SetGraphicsRootDescriptorTable(
			1,			// ルートパラメーターインデックス
			basicHeapHandle); // ヒープアドレス

		// 描画命令
		DX12::GetInstance()->CommandList()->DrawIndexedInstanced(BoxNumIndices, EntryNum, idxOffset, 0, 0);
	}
}

// 描画(箱)
void Mesh::DrawCylinder()
{
	if (_cylinderInstanceEntry == 0) {
		return;
	}
	for (int m = 0, size = (int)_cylinderMeshs.size(); m < size; ++m)
	{
		int EntryNum = _cylinderInstanceEntry - m * instanceMax;
		if (EntryNum >= instanceMax)
		{
			EntryNum = instanceMax;
		}

		MetaData* mapMetaData = nullptr;	// マップ先を示すポインター
		_cylinderMeshs[m]->materialBuffer->Map(0, nullptr, (void**)&mapMetaData);	// マップ
		*mapMetaData = _cylinderMetaData[m];
		_cylinderMeshs[m]->materialBuffer->Unmap(0, nullptr);		// アンマップ

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
		ConstantBuffer* constBuffer = nullptr;	// マップ先を示すポインター
		_cylinderMeshs[m]->constBuffer->Map(0, nullptr, (void**)&constBuffer);	// マップ
		constBuffer->view = viewMat;
		constBuffer->projection = projMat;
		constBuffer->eye = eye;
		constBuffer->lightVector = lightVector;
		constBuffer->lightColor = lightColor;
		_cylinderMeshs[m]->constBuffer->Unmap(0, nullptr);		// アンマップ

		// グラフィックスパイプラインステートの指定
		DX12::GetInstance()->CommandList()->SetPipelineState(_pipelineState.Get());
		// ルートシグネチャーの指定
		DX12::GetInstance()->CommandList()->SetGraphicsRootSignature(_rootSignature.Get());

		// ディスクリプターヒープの指定
		DX12::GetInstance()->CommandList()->SetDescriptorHeaps(1, _cylinderMeshs[m]->basicDescHeap.GetAddressOf());

		// ルートパラメーターとディスクリプターヒープの関連付け
		DX12::GetInstance()->CommandList()->SetGraphicsRootDescriptorTable(
			0,	// ルートパラメーターインデックス
			_cylinderMeshs[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart());    // ヒープアドレス

		// 頂点情報の指定[三角形]
		DX12::GetInstance()->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 描画(Draw関数)命令
		// 頂点バッファーの指定
		DX12::GetInstance()->CommandList()->IASetVertexBuffers(0, 1, &_cylinderMeshs[m]->vertexBufferView);
		// インデックスバッファーの指定
		DX12::GetInstance()->CommandList()->IASetIndexBuffer(&_cylinderMeshs[m]->indexBufferView);
		// ディスクリプタの先頭ハンドルを取得しておく
		auto basicHeapHandle = _cylinderMeshs[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart();
		// 次の場所に移動
		basicHeapHandle.ptr +=
			DX12::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize
			(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// 最初はオフセットなし
		unsigned int idxOffset = 0;

		// ルートパラメーターとディスクリプターヒープの関連付け
		DX12::GetInstance()->CommandList()->SetGraphicsRootDescriptorTable(
			1,			// ルートパラメーターインデックス
			basicHeapHandle); // ヒープアドレス

		// 描画命令
		DX12::GetInstance()->CommandList()->DrawIndexedInstanced(CylinderNumIndices, EntryNum, idxOffset, 0, 0);
	}
}

// 描画(球)
void Mesh::DrawSphere()
{
	if (_sphereInstanceEntry == 0) {
		return;
	}
	for (int m = 0, size = (int)_sphereMeshs.size(); m < size; ++m)
	{
		int EntryNum = _sphereInstanceEntry - m * instanceMax;
		if (EntryNum >= instanceMax)
		{
			EntryNum = instanceMax;
		}

		MetaData* mapMetaData = nullptr;	// マップ先を示すポインター
		_sphereMeshs[m]->materialBuffer->Map(0, nullptr, (void**)&mapMetaData);	// マップ
		*mapMetaData = _sphereMetaData[m];
		_sphereMeshs[m]->materialBuffer->Unmap(0, nullptr);		// アンマップ

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
		ConstantBuffer* constBuffer = nullptr;	// マップ先を示すポインター
		_sphereMeshs[m]->constBuffer->Map(0, nullptr, (void**)&constBuffer);	// マップ
		constBuffer->view = viewMat;
		constBuffer->projection = projMat;
		constBuffer->eye = eye;
		constBuffer->lightVector = lightVector;
		constBuffer->lightColor = lightColor;
		_sphereMeshs[m]->constBuffer->Unmap(0, nullptr);		// アンマップ

		// グラフィックスパイプラインステートの指定
		DX12::GetInstance()->CommandList()->SetPipelineState(_pipelineState.Get());
		// ルートシグネチャーの指定
		DX12::GetInstance()->CommandList()->SetGraphicsRootSignature(_rootSignature.Get());

		// ディスクリプターヒープの指定
		DX12::GetInstance()->CommandList()->SetDescriptorHeaps(1, _sphereMeshs[m]->basicDescHeap.GetAddressOf());

		// ルートパラメーターとディスクリプターヒープの関連付け
		DX12::GetInstance()->CommandList()->SetGraphicsRootDescriptorTable(
			0,	// ルートパラメーターインデックス
			_sphereMeshs[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart());    // ヒープアドレス

		// 頂点情報の指定[三角形]
		DX12::GetInstance()->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 描画(Draw関数)命令
		// 頂点バッファーの指定
		DX12::GetInstance()->CommandList()->IASetVertexBuffers(0, 1, &_sphereMeshs[m]->vertexBufferView);
		// インデックスバッファーの指定
		DX12::GetInstance()->CommandList()->IASetIndexBuffer(&_sphereMeshs[m]->indexBufferView);
		// ディスクリプタの先頭ハンドルを取得しておく
		auto basicHeapHandle = _sphereMeshs[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart();
		// 次の場所に移動
		basicHeapHandle.ptr +=
			DX12::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize
			(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// 最初はオフセットなし
		unsigned int idxOffset = 0;

		// ルートパラメーターとディスクリプターヒープの関連付け
		DX12::GetInstance()->CommandList()->SetGraphicsRootDescriptorTable(
			1,			// ルートパラメーターインデックス
			basicHeapHandle); // ヒープアドレス

		// 描画命令
		DX12::GetInstance()->CommandList()->DrawIndexedInstanced(SphereNumIndices, EntryNum, idxOffset, 0, 0);
	}
}

// 描画(円)
void Mesh::DrawCircle()
{
	if (_circleInstanceEntry == 0) {
		return;
	}
	for (int m = 0, size = (int)_circleMeshs.size(); m < size; ++m)
	{
		int EntryNum = _circleInstanceEntry - m * instanceMax;
		if (EntryNum >= instanceMax)
		{
			EntryNum = instanceMax;
		}

		MetaData* mapMetaData = nullptr;	// マップ先を示すポインター
		_circleMeshs[m]->materialBuffer->Map(0, nullptr, (void**)&mapMetaData);	// マップ
		*mapMetaData = _circleMetaData[m];
		_circleMeshs[m]->materialBuffer->Unmap(0, nullptr);		// アンマップ

		// =====ビュー行列=====
		XMMATRIX viewMat = XMMatrixIdentity();
		// =====プロジェクション行列=====
		XMMATRIX projMat = XMMatrixIdentity();
		// ===== ライトの向き =====
		XMFLOAT4 lightVector = Camera::GetInstance()->GetLightVector();
		// ===== ライトの色 =====
		XMFLOAT4 lightColor = Camera::GetInstance()->GetLightColor();
		// ===== カメラ座標 =====
		XMFLOAT3 eye = Camera::GetInstance()->GetEyePosition();

		// 定数情報の更新
		ConstantBuffer* constBuffer = nullptr;	// マップ先を示すポインター
		_circleMeshs[m]->constBuffer->Map(0, nullptr, (void**)&constBuffer);	// マップ
		constBuffer->view = viewMat;
		constBuffer->projection = projMat;
		constBuffer->eye = eye;
		constBuffer->lightVector = lightVector;
		constBuffer->lightColor = lightColor;
		_circleMeshs[m]->constBuffer->Unmap(0, nullptr);		// アンマップ

		// グラフィックスパイプラインステートの指定
		DX12::GetInstance()->CommandList()->SetPipelineState(_pipelineState.Get());
		// ルートシグネチャーの指定
		DX12::GetInstance()->CommandList()->SetGraphicsRootSignature(_rootSignature.Get());

		// ディスクリプターヒープの指定
		DX12::GetInstance()->CommandList()->SetDescriptorHeaps(1, _circleMeshs[m]->basicDescHeap.GetAddressOf());

		// ルートパラメーターとディスクリプターヒープの関連付け
		DX12::GetInstance()->CommandList()->SetGraphicsRootDescriptorTable(
			0,	// ルートパラメーターインデックス
			_circleMeshs[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart());    // ヒープアドレス

		// 頂点情報の指定[三角形]
		DX12::GetInstance()->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 描画(Draw関数)命令
		// 頂点バッファーの指定
		DX12::GetInstance()->CommandList()->IASetVertexBuffers(0, 1, &_circleMeshs[m]->vertexBufferView);
		// インデックスバッファーの指定
		DX12::GetInstance()->CommandList()->IASetIndexBuffer(&_circleMeshs[m]->indexBufferView);
		// ディスクリプタの先頭ハンドルを取得しておく
		auto basicHeapHandle = _circleMeshs[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart();
		// 次の場所に移動
		basicHeapHandle.ptr +=
			DX12::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize
			(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// 最初はオフセットなし
		unsigned int idxOffset = 0;

		// ルートパラメーターとディスクリプターヒープの関連付け
		DX12::GetInstance()->CommandList()->SetGraphicsRootDescriptorTable(
			1,			// ルートパラメーターインデックス
			basicHeapHandle); // ヒープアドレス

		// 描画命令
		DX12::GetInstance()->CommandList()->DrawIndexedInstanced(_circleIndicesNum, EntryNum, idxOffset, 0, 0);
	}
}


// 箱
void Mesh::BoxDeleate(int& entryPos) {
	if (entryPos != -1)
	{
		int EntryPos = entryPos % instanceMax;
		int ArrayNum = entryPos / instanceMax;
		_boxMetaData[ArrayNum].world[EntryPos] = {};
		_boxDataCheck[ArrayNum]._dataCheck[EntryPos] = false;
		entryPos = -1;
	}
}

void Mesh::SetBoxMetaData(int& entryPos,
	VECTOR position, VECTOR angle, VECTOR scale, VECTOR color) {
	// 最初もしくは500番目まで使い切ったら次のバッファーを用意する
	int num = _boxInstanceEntry % instanceMax;

	if (num == 0&& entryPos == -1)
	{
		_boxMeshs.push_back(new MESH);
		_boxMeshs.back()->vertexBuffer = boxVertexBuffer;
		_boxMeshs.back()->vertexBufferView = boxVertexBufferView;
		_boxMeshs.back()->indexBuffer = boxIndexBuffer;
		_boxMeshs.back()->indexBufferView = boxIndexBufferView;
		CreateMaterialBuffer(_boxMeshs.back());	// マテリアル生成
		// コンスタントバッファービューの生成
		CreateConstantBufferView(_boxMeshs.back());

		_boxMetaData.resize(_boxMeshs.size());
		_boxDataCheck.resize(_boxMeshs.size());
	}
	// エントリー番号
	int EntryPos;
	// 使用するメタデータの番号
	int ArrayNum;
	// 仮のスペキュラー
	XMFLOAT4 specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 15.0f);
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
		for (int i = 0; i < _boxDataCheck.size(); i++)
		{
			for (int j = 0; j < instanceMax; j++)
			{
				if (_boxDataCheck[i]._dataCheck[j] == false)
				{
					entryPos = i * instanceMax + j;
					if ((UINT)entryPos >= _boxInstanceEntry)
					{
						_boxInstanceEntry = entryPos + 1;
					}
					_boxDataCheck[i]._dataCheck[j] = true;
					break;
				}
			}
		}

		EntryPos = entryPos % instanceMax;
		ArrayNum = entryPos / instanceMax;

		// 生成するライン情報代入
		_boxMetaData[ArrayNum].world[EntryPos] = world_matrix;
		_boxMetaData[ArrayNum].diffuse[EntryPos].x = color.x;
		_boxMetaData[ArrayNum].diffuse[EntryPos].y = color.y;
		_boxMetaData[ArrayNum].diffuse[EntryPos].z = color.z;
		_boxMetaData[ArrayNum].diffuse[EntryPos].w = 0;
		_boxMetaData[ArrayNum].ambient[EntryPos].x = color.x * 0.5f;
		_boxMetaData[ArrayNum].ambient[EntryPos].y = color.y * 0.5f;
		_boxMetaData[ArrayNum].ambient[EntryPos].z = color.z * 0.5f;
		_boxMetaData[ArrayNum].specular[EntryPos] = specular;
	}
	else
	{
		EntryPos = entryPos % instanceMax;
		ArrayNum = entryPos / instanceMax;
		// 生成するライン情報代入
		_boxMetaData[ArrayNum].world[EntryPos] = world_matrix;
		_boxMetaData[ArrayNum].diffuse[EntryPos].x = color.x;
		_boxMetaData[ArrayNum].diffuse[EntryPos].y = color.y;
		_boxMetaData[ArrayNum].diffuse[EntryPos].z = color.z;
		_boxMetaData[ArrayNum].diffuse[EntryPos].w = 0;
		_boxMetaData[ArrayNum].ambient[EntryPos].x = color.x * 0.5f;
		_boxMetaData[ArrayNum].ambient[EntryPos].y = color.y * 0.5f;
		_boxMetaData[ArrayNum].ambient[EntryPos].z = color.z * 0.5f;
		_boxMetaData[ArrayNum].specular[EntryPos] = specular;
	}
}

// 円柱
void Mesh::CylinderDeleate(int& entryPos) {
	if (entryPos != -1)
	{
		int EntryPos = entryPos % instanceMax;
		int ArrayNum = entryPos / instanceMax;
		_cylinderMetaData[ArrayNum].world[EntryPos] = {};
		_cylinderDataCheck[ArrayNum]._dataCheck[EntryPos] = false;
		entryPos = -1;
	}
}

void Mesh::SetCylinderMetaData(int& entryPos,
	VECTOR position, VECTOR angle, VECTOR scale, VECTOR color) {
	// 最初もしくは500番目まで使い切ったら次のバッファーを用意する
	int num = _cylinderInstanceEntry % instanceMax;

	if (num == 0 && entryPos == -1)
	{
		_cylinderMeshs.push_back(new MESH);
		_cylinderMeshs.back()->vertexBuffer = cylinderVertexBuffer;
		_cylinderMeshs.back()->vertexBufferView = cylinderVertexBufferView;
		_cylinderMeshs.back()->indexBuffer = cylinderIndexBuffer;
		_cylinderMeshs.back()->indexBufferView = cylinderIndexBufferView;
		CreateMaterialBuffer(_cylinderMeshs.back());	// マテリアル生成
		// コンスタントバッファービューの生成
		CreateConstantBufferView(_cylinderMeshs.back());

		_cylinderMetaData.resize(_cylinderMeshs.size());
		_cylinderDataCheck.resize(_cylinderMeshs.size());
	}
	// エントリー番号
	int EntryPos;
	// 使用するメタデータの番号
	int ArrayNum;
	// 仮のスペキュラー
	XMFLOAT4 specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 15.0f);
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
		for (int i = 0; i < _cylinderDataCheck.size(); i++)
		{
			for (int j = 0; j < instanceMax; j++)
			{
				if (_cylinderDataCheck[i]._dataCheck[j] == false)
				{
					entryPos = i * instanceMax + j;
					if ((UINT)entryPos >= _cylinderInstanceEntry)
					{
						_cylinderInstanceEntry = entryPos + 1;
					}
					_cylinderDataCheck[i]._dataCheck[j] = true;
					break;
				}
			}
		}

		EntryPos = entryPos % instanceMax;
		ArrayNum = entryPos / instanceMax;

		// 生成するライン情報代入
		_cylinderMetaData[ArrayNum].world[EntryPos] = world_matrix;
		_cylinderMetaData[ArrayNum].diffuse[EntryPos].x = color.x;
		_cylinderMetaData[ArrayNum].diffuse[EntryPos].y = color.y;
		_cylinderMetaData[ArrayNum].diffuse[EntryPos].z = color.z;
		_cylinderMetaData[ArrayNum].diffuse[EntryPos].w = 0;
		_cylinderMetaData[ArrayNum].ambient[EntryPos].x = color.x * 0.5f;
		_cylinderMetaData[ArrayNum].ambient[EntryPos].y = color.y * 0.5f;
		_cylinderMetaData[ArrayNum].ambient[EntryPos].z = color.z * 0.5f;
		_cylinderMetaData[ArrayNum].specular[EntryPos] = specular;
	}
	else
	{
		EntryPos = entryPos % instanceMax;
		ArrayNum = entryPos / instanceMax;
		// 生成するライン情報代入
		_cylinderMetaData[ArrayNum].world[EntryPos] = world_matrix;
		_cylinderMetaData[ArrayNum].diffuse[EntryPos].x = color.x;
		_cylinderMetaData[ArrayNum].diffuse[EntryPos].y = color.y;
		_cylinderMetaData[ArrayNum].diffuse[EntryPos].z = color.z;
		_cylinderMetaData[ArrayNum].diffuse[EntryPos].w = 0;
		_cylinderMetaData[ArrayNum].ambient[EntryPos].x = color.x * 0.5f;
		_cylinderMetaData[ArrayNum].ambient[EntryPos].y = color.y * 0.5f;
		_cylinderMetaData[ArrayNum].ambient[EntryPos].z = color.z * 0.5f;
		_cylinderMetaData[ArrayNum].specular[EntryPos] = specular;
	}
}

// 円柱
void Mesh::SphereDeleate(int& entryPos) {
	if (entryPos != -1)
	{
		int EntryPos = entryPos % instanceMax;
		int ArrayNum = entryPos / instanceMax;
		_sphereMetaData[ArrayNum].world[EntryPos] = {};
		_sphereDataCheck[ArrayNum]._dataCheck[EntryPos] = false;
		entryPos = -1;
	}
}

void Mesh::SetSphereMetaData(int& entryPos,
	VECTOR position, VECTOR angle, VECTOR scale, VECTOR color) {
	// 最初もしくは500番目まで使い切ったら次のバッファーを用意する
	int num = _sphereInstanceEntry % instanceMax;

	if (num == 0 && entryPos == -1)
	{
		_sphereMeshs.push_back(new MESH);
		_sphereMeshs.back()->vertexBuffer = sphereVertexBuffer;
		_sphereMeshs.back()->vertexBufferView = sphereVertexBufferView;
		_sphereMeshs.back()->indexBuffer = sphereIndexBuffer;
		_sphereMeshs.back()->indexBufferView = sphereIndexBufferView;
		CreateMaterialBuffer(_sphereMeshs.back());	// マテリアル生成
		// コンスタントバッファービューの生成
		CreateConstantBufferView(_sphereMeshs.back());

		_sphereMetaData.resize(_sphereMeshs.size());
		_sphereDataCheck.resize(_sphereMeshs.size());
	}
	// エントリー番号
	int EntryPos;
	// 使用するメタデータの番号
	int ArrayNum;
	// 仮のスペキュラー
	XMFLOAT4 specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 15.0f);
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
		for (int i = 0; i < _sphereDataCheck.size(); i++)
		{
			for (int j = 0; j < instanceMax; j++)
			{
				if (_sphereDataCheck[i]._dataCheck[j] == false)
				{
					entryPos = i * instanceMax + j;
					if ((UINT)entryPos >= _sphereInstanceEntry)
					{
						_sphereInstanceEntry = entryPos + 1;
					}
					_sphereDataCheck[i]._dataCheck[j] = true;
					break;
				}
			}
		}

		EntryPos = entryPos % instanceMax;
		ArrayNum = entryPos / instanceMax;

		// 生成するライン情報代入
		_sphereMetaData[ArrayNum].world[EntryPos] = world_matrix;
		_sphereMetaData[ArrayNum].diffuse[EntryPos].x = color.x;
		_sphereMetaData[ArrayNum].diffuse[EntryPos].y = color.y;
		_sphereMetaData[ArrayNum].diffuse[EntryPos].z = color.z;
		_sphereMetaData[ArrayNum].diffuse[EntryPos].w = 0;
		_sphereMetaData[ArrayNum].ambient[EntryPos].x = color.x * 0.5f;
		_sphereMetaData[ArrayNum].ambient[EntryPos].y = color.y * 0.5f;
		_sphereMetaData[ArrayNum].ambient[EntryPos].z = color.z * 0.5f;
		_sphereMetaData[ArrayNum].specular[EntryPos] = specular;
	}
	else
	{
		EntryPos = entryPos % instanceMax;
		ArrayNum = entryPos / instanceMax;
		// 生成するライン情報代入
		_sphereMetaData[ArrayNum].world[EntryPos] = world_matrix;
		_sphereMetaData[ArrayNum].diffuse[EntryPos].x = color.x;
		_sphereMetaData[ArrayNum].diffuse[EntryPos].y = color.y;
		_sphereMetaData[ArrayNum].diffuse[EntryPos].z = color.z;
		_sphereMetaData[ArrayNum].diffuse[EntryPos].w = 0;
		_sphereMetaData[ArrayNum].ambient[EntryPos].x = color.x * 0.5f;
		_sphereMetaData[ArrayNum].ambient[EntryPos].y = color.y * 0.5f;
		_sphereMetaData[ArrayNum].ambient[EntryPos].z = color.z * 0.5f;
		_sphereMetaData[ArrayNum].specular[EntryPos] = specular;
	}
}

// 円柱
void Mesh::CircleDeleate(int& entryPos) {
	if (entryPos != -1)
	{
		int EntryPos = entryPos % instanceMax;
		int ArrayNum = entryPos / instanceMax;
		_circleMetaData[ArrayNum].world[EntryPos] = {};
		_circleDataCheck[ArrayNum]._dataCheck[EntryPos] = false;
		entryPos = -1;
	}
}

void Mesh::SetCircleMetaData(int& entryPos,
	VECTOR position, VECTOR angle, VECTOR scale, VECTOR color) {
	// 最初もしくは500番目まで使い切ったら次のバッファーを用意する
	int num = _circleInstanceEntry % instanceMax;

	if (num == 0 && entryPos == -1)
	{
		_circleMeshs.push_back(new MESH);
		_circleMeshs.back()->vertexBuffer = circleVertexBuffer;
		_circleMeshs.back()->vertexBufferView = circleVertexBufferView;
		_circleMeshs.back()->indexBuffer = circleIndexBuffer;
		_circleMeshs.back()->indexBufferView = circleIndexBufferView;
		CreateMaterialBuffer(_circleMeshs.back());	// マテリアル生成
		// コンスタントバッファービューの生成
		CreateConstantBufferView(_circleMeshs.back());

		_circleMetaData.resize(_circleMeshs.size());
		_circleDataCheck.resize(_circleMeshs.size());
	}
	// エントリー番号
	int EntryPos;
	// 使用するメタデータの番号
	int ArrayNum;
	// 仮のスペキュラー
	XMFLOAT4 specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 15.0f);
	//// =====ワールド行列=====
	// 画像を左上寄せするための変数
	LONG x = g_window_width / 2;
	LONG y = g_window_height / 2;

	XMMATRIX world_matrix = XMMatrixIdentity();
	world_matrix.r[0].m128_f32[0] = scale.x / x;
	world_matrix.r[1].m128_f32[1] = -scale.y / y;
	world_matrix.r[3].m128_f32[0] = position.x / x - 1;		// 左上X座標
	world_matrix.r[3].m128_f32[1] = -(position.y / y - 1);	// 左上Y座標

	// 回転行列
	XMMATRIX rotationMat = XMMatrixRotationZ(XMConvertToRadians(angle.z));
	world_matrix = rotationMat * world_matrix;

	if (entryPos == -1)
	{
		// 空いてるエントリー番号を探す
		for (int i = 0; i < _circleDataCheck.size(); i++)
		{
			for (int j = 0; j < instanceMax; j++)
			{
				if (_circleDataCheck[i]._dataCheck[j] == false)
				{
					entryPos = i * instanceMax + j;
					if ((UINT)entryPos >= _circleInstanceEntry)
					{
						_circleInstanceEntry = entryPos + 1;
					}
					_circleDataCheck[i]._dataCheck[j] = true;
					break;
				}
			}
		}

		EntryPos = entryPos % instanceMax;
		ArrayNum = entryPos / instanceMax;

		// 生成するライン情報代入
		_circleMetaData[ArrayNum].world[EntryPos] = world_matrix;
		_circleMetaData[ArrayNum].diffuse[EntryPos].x = color.x;
		_circleMetaData[ArrayNum].diffuse[EntryPos].y = color.y;
		_circleMetaData[ArrayNum].diffuse[EntryPos].z = color.z;
		_circleMetaData[ArrayNum].diffuse[EntryPos].w = 0;
		_circleMetaData[ArrayNum].ambient[EntryPos].x = color.x * 0.5f;
		_circleMetaData[ArrayNum].ambient[EntryPos].y = color.y * 0.5f;
		_circleMetaData[ArrayNum].ambient[EntryPos].z = color.z * 0.5f;
		_circleMetaData[ArrayNum].specular[EntryPos] = specular;
	}
	else
	{
		EntryPos = entryPos % instanceMax;
		ArrayNum = entryPos / instanceMax;
		// 生成するライン情報代入
		_circleMetaData[ArrayNum].world[EntryPos] = world_matrix;
		_circleMetaData[ArrayNum].diffuse[EntryPos].x = color.x;
		_circleMetaData[ArrayNum].diffuse[EntryPos].y = color.y;
		_circleMetaData[ArrayNum].diffuse[EntryPos].z = color.z;
		_circleMetaData[ArrayNum].diffuse[EntryPos].w = 0;
		_circleMetaData[ArrayNum].ambient[EntryPos].x = color.x * 0.5f;
		_circleMetaData[ArrayNum].ambient[EntryPos].y = color.y * 0.5f;
		_circleMetaData[ArrayNum].ambient[EntryPos].z = color.z * 0.5f;
		_circleMetaData[ArrayNum].specular[EntryPos] = specular;
	}
}