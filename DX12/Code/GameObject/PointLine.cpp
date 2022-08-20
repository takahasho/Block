// -------------------- IncludeFile -----------------
#include "PointLine.h"
#include "../Camera/Camera.h"
#include "../DX12System.h"
PointLine* PointLine::_instance = nullptr;			// �C���X�^���X�̎���
// �R���X�g���N�^
PointLine::PointLine() :
	_pointVertexBufferView(), _pointIndexBufferView(), _pointInstanceEntry(0),
	_lineVertexBufferView(), _lineIndexBufferView(), _lineInstanceEntry(0),
	_pointMetaData(), _lineMetaData()
{

}
// �f�X�g���N�^
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
// �쐬
void PointLine::Create()
{
	if (!_instance)
	{
		_instance = new PointLine();

		_instance->CreatePointLine();
	}
}
// �j��
void PointLine::Destroy()
{
	delete _instance;
}
// ����
HRESULT PointLine::CreatePointLine()
{
	HRESULT hr = S_OK;

	PointLine::GetInstance()->_dev = DX12::GetInstance()->GetDevice();
	PointLine::GetInstance()->_cmdList = DX12::GetInstance()->CommandList();

	// ���[�g�V�O�l�`���[�̐���
	if (FAILED(CreateRootSignature()))
		return E_FAIL;

	// �`��p�C�v���C���X�e�[�g�̐���
	if (FAILED(CreatePipelineState()))
		return E_FAIL;

	// ���_/�C���f�b�N�X�o�b�t�@�[�̐���(�_)
	if (FAILED(CreatePointVertexIndexBufferView()))
		return E_FAIL;

	// ���_/�C���f�b�N�X�o�b�t�@�[�̐���(�_)
	if (FAILED(CreateLineVertexIndexBufferView()))
		return E_FAIL;


	return S_OK;
}
// ���[�g�V�O�l�`���[�̐���
HRESULT PointLine::CreateRootSignature()
{
	HRESULT hr = S_OK;

	// �f�B�X�N���v�^�[�����W���쐬
	D3D12_DESCRIPTOR_RANGE descTblRange[1] = {};
	// �f�B�X�N���v�^�[�̐�[�P��]
	descTblRange[0].NumDescriptors = 1;
	// �����W�̎��[CBV:�R���X�^���g�o�b�t�@�[�r���[]
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// �擪���W�X�^�[�ԍ�[0��I��]
	descTblRange[0].BaseShaderRegister = 0;
	// �I�t�Z�b�g���w��[�A�������f�B�X�N���v�^�[�����W���O�̃f�B�X�N���v�^�����W�̒���ɂ���]
	descTblRange[0].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ���[�g�p�����[�^�[���쐬
	D3D12_ROOT_PARAMETER rootParam = {};
	// ��ʂ̑I��[�f�B�X�N���v�^�[�e�[�u����I��]
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// �ǂ̃V�F�[�_�[���痘�p�\��[�S�ẴV�F�[�_�[]
	rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// �f�B�X�N���v�^�[�����W�̃A�h���X
	rootParam.DescriptorTable.pDescriptorRanges =
		&descTblRange[0];
	// �f�B�X�N���v�^�[�����W�̐�
	rootParam.DescriptorTable.NumDescriptorRanges = 1;

	// �T���v���[���쐬
	D3D12_STATIC_SAMPLER_DESC samplerDecs = {};
	// �������̌J��Ԃ�
	samplerDecs.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// �c�����̌J��Ԃ�
	samplerDecs.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// ���s�����̌J��Ԃ�
	samplerDecs.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// �{�[�_�[�F[���F]
	samplerDecs.BorderColor =
		D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	// ���`���[���̐F�̕��ϒl]
	samplerDecs.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	// �~�b�v�}�b�v�̍ő�l
	samplerDecs.MaxLOD = D3D12_FLOAT32_MAX;
	// �~�b�v�}�b�v�̍ŏ��l
	samplerDecs.MinLOD = 0.0f;
	// �ǂ̃V�F�[�_�[���痘�p�\��[�s�N�Z���V�F�[�_�[]
	samplerDecs.ShaderVisibility =
		D3D12_SHADER_VISIBILITY_PIXEL;
	// ��r�֐�[���T���v�����O���Ȃ�]
	samplerDecs.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

	// ���[�g�V�O�l�`���[�̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	// �t���O[���_���(���̓A�Z���u��)������]
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	// �p�����[�^�[�̐擪�A�h���X
	rootSignatureDesc.pParameters = &rootParam;
	// �p�����[�^�[�̐�
	rootSignatureDesc.NumParameters = 1;
	// �T���v���[�̐擪�A�h���X
	rootSignatureDesc.pStaticSamplers = &samplerDecs;
	// �T���v���[�̐�
	rootSignatureDesc.NumStaticSamplers = 1;

	// �o�C�i���R�[�h���쐬
	ID3DBlob* rootSigBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	hr = D3D12SerializeRootSignature(
		&rootSignatureDesc,				// ���[�g�V�O�l�`���[�ݒ�
		D3D_ROOT_SIGNATURE_VERSION_1_0,	// ���[�g�V�O�l�`���[�o�[�W����
		&rootSigBlob,					// �󂯎�邽�߂̃|�C���^�[�̃A�h���X
		&errorBlob);					// �G���[�p�|�C���^�[�̃A�h���X[�G���[����errorBlob�Ƀ��b�Z�[�W������]
	if (FAILED(hr))
	{
		// �G���[���b�Z�[�W
		BlobErrorMessange(hr, errorBlob);
		return E_FAIL;
	}

	// ���[�g�V�O�l�`���[�I�u�W�F�N�g�̍쐬
	hr = _dev->CreateRootSignature(
		0,	// nodemask�B�O�ł悢
		rootSigBlob->GetBufferPointer(),	// �A�h���X
		rootSigBlob->GetBufferSize(),		// �T�C�Y
		IID_PPV_ARGS(_rootSignature.GetAddressOf()));	// �A�h���X��[��
	if (hr != S_OK)
		return E_FAIL;

	return S_OK;
}
// �`��p�C�v���C���X�e�[�g�̐���
HRESULT PointLine::CreatePipelineState()
{
	HRESULT hr = S_OK;

	// �V�F�[�_�[�I�u�W�F�N�g�ǂݍ��ݗp
	ID3DBlob* vsPointBlob = nullptr;
	ID3DBlob* vsLineBlob = nullptr;
	ID3DBlob* gsLineBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// ���_�V�F�[�_�[�̓ǂݍ���
	hr = D3DCompileFromFile(
		L".\\Shader\\PointLine\\PointVertexShader.hlsl",	// �V�F�[�_�[��
		nullptr,							// define�͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// �C���N���[�h�̓f�t�H���g
		"PointVS", "vs_5_0",				// �֐��� BasicVS�A�ΏۃV�F�[�_�[�� vs_5_0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�N�l����эœK���Ȃ�	
		0,									// �G�t�F�N�g�R���p�C���I�v�V����[0�𐄏�]
		&vsPointBlob,							// �󂯎�邽�߂̃|�C���^�[�̃A�h���X
		&errorBlob);						// �G���[�p�|�C���^�[�̃A�h���X[�G���[����errorBlob�Ƀ��b�Z�[�W������]
	if (FAILED(hr))
	{
		// �G���[���b�Z�[�W
		BlobErrorMessange(hr, errorBlob);
		return E_FAIL;
	}
	hr = D3DCompileFromFile(
		L".\\Shader\\PointLine\\LineVertexShader.hlsl",	// �V�F�[�_�[��
		nullptr,							// define�͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// �C���N���[�h�̓f�t�H���g
		"LineVS", "vs_5_0",				// �֐��� BasicVS�A�ΏۃV�F�[�_�[�� vs_5_0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�N�l����эœK���Ȃ�	
		0,									// �G�t�F�N�g�R���p�C���I�v�V����[0�𐄏�]
		&vsLineBlob,							// �󂯎�邽�߂̃|�C���^�[�̃A�h���X
		&errorBlob);						// �G���[�p�|�C���^�[�̃A�h���X[�G���[����errorBlob�Ƀ��b�Z�[�W������]
	if (FAILED(hr))
	{
		// �G���[���b�Z�[�W
		BlobErrorMessange(hr, errorBlob);
		return E_FAIL;
	}

	// �W�I���g���[�V�F�[�_�[�̓ǂݍ���
	hr = D3DCompileFromFile(
		L".\\Shader\\PointLine\\LineGeometryShader.hlsl",	// �V�F�[�_�[��
		nullptr,							// define�͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// �C���N���[�h�̓f�t�H���g
		"LineGS", "gs_5_0",				// �֐��� BasicVS�A�ΏۃV�F�[�_�[�� vs_5_0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�N�l����эœK���Ȃ�	
		0,									// �G�t�F�N�g�R���p�C���I�v�V����[0�𐄏�]
		&gsLineBlob,							// �󂯎�邽�߂̃|�C���^�[�̃A�h���X
		&errorBlob);						// �G���[�p�|�C���^�[�̃A�h���X[�G���[����errorBlob�Ƀ��b�Z�[�W������]
	if (FAILED(hr))
	{
		// �G���[���b�Z�[�W
		BlobErrorMessange(hr, errorBlob);
		return E_FAIL;
	}

	// �s�N�Z���V�F�[�_�[�̓ǂݍ���
	hr = D3DCompileFromFile(
		L".\\Shader\\PointLine\\PointLinePixelShader.hlsl",	// �V�F�[�_�[��
		nullptr,							// define�͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// �C���N���[�h�̓f�t�H���g
		"PointLinePS", "ps_5_0",			// �֐��� BasicPS�A�ΏۃV�F�[�_�[�� ps_5_0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�N�l����эœK���Ȃ�	
		0,									// �G�t�F�N�g�R���p�C���I�v�V����[0�𐄏�]
		&psBlob,							// �󂯎�邽�߂̃|�C���^�[�̃A�h���X
		&errorBlob);						// �G���[�p�|�C���^�[�̃A�h���X[�G���[����errorBlob�Ƀ��b�Z�[�W������]
	if (FAILED(hr))
	{
		// �G���[���b�Z�[�W
		BlobErrorMessange(hr, errorBlob);
		return E_FAIL;
	}

	// ���_���̓��C�A�E�g���쐬
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{	// ���W���
			"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	// �O���t�B�b�N�X�p�C�v���C���X�e�[�g(PSO:pipeline state object)���쐬
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipelineDesc = {};
	// ���[�g�V�O�l�`���[
	gpipelineDesc.pRootSignature = _rootSignature.Get();
	// ���_�V�F�[�_�[�̃A�h���X�ƃT�C�Y
	gpipelineDesc.VS.pShaderBytecode = vsPointBlob->GetBufferPointer();
	gpipelineDesc.VS.BytecodeLength = vsPointBlob->GetBufferSize();
	// �s�N�Z���V�F�[�_�[�̃A�h���X�ƃT�C�Y
	gpipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	gpipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// �T���v���}�X�N�ƃ��X�^���C�U�[�X�e�[�g�̐ݒ�
	// �f�t�H���g�̃T���v���}�X�N��\���萔(0xffffffff)
	gpipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// �܂��A���`�G�C���A�X�͎g��Ȃ�����false
	gpipelineDesc.RasterizerState.MultisampleEnable = FALSE;
	// �J�����O���Ȃ�(�\���`��)
	gpipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	// ���g��h��ׂ�
	gpipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	// �[�x�����̃N���b�s���O�͗L����
	gpipelineDesc.RasterizerState.DepthClipEnable = TRUE;

	// �u�����h�X�e�[�g�̐ݒ�
	// �A���t�@�[�ԗ����̗L��[�Ȃ�]
	gpipelineDesc.BlendState.AlphaToCoverageEnable = FALSE;
	// �����_�[�^�[�Q�b�g�Ƀu�����h�X�e�[�g�̊��蓖�Ă̗L��[�Ȃ�]
	gpipelineDesc.BlendState.IndependentBlendEnable = FALSE;

	// �����_�[�^�[�Q�b�g�u�����h�̐ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	// �u�����h�̗L��[�Ȃ�]
	renderTargetBlendDesc.BlendEnable = FALSE;
	// �_�����Z�̗L��[�Ȃ�]
	renderTargetBlendDesc.LogicOpEnable = FALSE;
	// RGBA���ꂼ��̒l���������ނ��w��[�S��]
	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	// �����_�[�^�[�Q�b�g�u�����h��PSO�ɕR�Â���
	gpipelineDesc.BlendState.RenderTarget[0] = renderTargetBlendDesc;

	// ���̓��C�A�E�g�̐ݒ�
	// ���_���̓��C�A�E�g��PSO�ɕR�Â���
	gpipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	// ���̓��C�A�E�g�̃T�C�Y���w��[���C�A�E�g�z��̗v�f��]
	gpipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	// �C���f�b�N�X�o�b�t�@�[���p���A�X�g���b�v�J�b�g�l�ݒ�[�J�b�g�Ȃ�]
	gpipelineDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

	// �v���~�e�B�u�̃^�C�v[�_]
	gpipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

	// �����_�[�^�[�Q�b�g�̐ݒ�
	// �����_�[�^�[�Q�b�g��[�P�ł悢]
	gpipelineDesc.NumRenderTargets = 1;
	// �t�H�[�}�b�g[0�Ԃ̂�R8G8B8A8_UNORM�Őݒ�]
	gpipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	// �A���`�G�C���A�V���O�̂߂̃T���v�����̐ݒ�
	// �}���`�T���v�����O��[�T���v�����O�͂P�s�N�Z���ɂ��P]
	gpipelineDesc.SampleDesc.Count = 1;
	// �}���`�T���v�����O�̃N�I���e�B[�N�I���e�B�͍Œ�]
	gpipelineDesc.SampleDesc.Quality = 0;

	// �[�x�o�b�t�@�[�̗L���̗L��[�L��]
	gpipelineDesc.DepthStencilState.DepthEnable = true;
	// �[�x�X�e���V���o�b�t�@�[�̏������ݕ��@[�������ݗL��]
	gpipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// �[�x�X�e���V���o�b�t�@�[�̐�����@[�������ق����̗p]
	gpipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	// �t�H�[�}�b�g���w��
	gpipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// �O���t�B�b�N�p�C�v���C���X�e�[�g�I�u�W�F�N�g�𐶐�
	hr = _dev->CreateGraphicsPipelineState(
		&gpipelineDesc, IID_PPV_ARGS(_pointPipelineState.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// ���_�V�F�[�_�[�̃A�h���X�ƃT�C�Y
	gpipelineDesc.VS.pShaderBytecode = vsLineBlob->GetBufferPointer();
	gpipelineDesc.VS.BytecodeLength = vsLineBlob->GetBufferSize();
	// �W�I���g���[�V�F�[�_�[�̃A�h���X�ƃT�C�Y
	gpipelineDesc.GS.pShaderBytecode = gsLineBlob->GetBufferPointer();
	gpipelineDesc.GS.BytecodeLength = gsLineBlob->GetBufferSize();

	// �v���~�e�B�u�̃^�C�v[��]
	gpipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	// �O���t�B�b�N�p�C�v���C���X�e�[�g�I�u�W�F�N�g�𐶐�
	hr = _dev->CreateGraphicsPipelineState(
		&gpipelineDesc, IID_PPV_ARGS(_linePipelineState.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	return S_OK;
}
// ���_/�C���f�b�N�X�o�b�t�@�[�̐���(�_)
HRESULT PointLine::CreatePointVertexIndexBufferView()
{
	HRESULT hr = S_OK;

	// ���_�f�[�^�\����
	struct Vertex
	{
		XMFLOAT4 pos;	// xyz:���W
	};

	Vertex vertices[] =
	{
		{{ 0.0f, 0.0f, 0.0f, 0.0f}},
	};

	// �q�[�v�v���p�e�B�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp = {};
	// �q�[�v�̎��[CPU����A�N�Z�X�ł���(�}�b�v�ł���)]
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// CPU�̃y�[�W���O�ݒ�[�l���Ȃ��Ă��悢]
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	// �������[�v�[�����ǂ���������[CUSTOM�ȊO�̂Ƃ��͂���ł悢]
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	// ���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC resDesc = {};
	// ���p���@[�o�b�t�@�[�Ɏg���̂�BUFFER���w��]
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	// �T�C�Y��[���őS���܂��Ȃ��̂�sizeof(�S���_)�Ƃ���]
	resDesc.Width = sizeof(vertices);
	// �T�C�Y����[���ŕ\�����Ă���̂łP�Ƃ���]
	resDesc.Height = 1;
	// �[�x��z��T�C�Y[�P�ł悢]
	resDesc.DepthOrArraySize = 1;
	// �~�b�v�}�b�v���x��[�P�ł悢]
	resDesc.MipLevels = 1;
	// �t�H�[�}�b�g[�摜�ł͂Ȃ��̂�UNKNOWN�ł悢]
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	// �A���`�G�C���A�V���O�̃p�����[�^�[[�A���`�G�C���A�V���O���s��Ȃ�����SampleDesc.Count = 1]
	resDesc.SampleDesc.Count = 1;
	// �t���O[NONE�ł悢]
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	// ���C�A�E�g[����̓e�N�X�`���[�̃��C�A�E�g�ł͂Ȃ��̂ŁA�������[���ŏ�����Ō㖘�A�����Ă��邱�Ƃ�����]
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// ���\�[�X�I�u�W�F�N�g���쐬
	hr = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_pointVertexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// ���_���̃R�s�[
	Vertex* vertexMap = nullptr;
	// ���_�o�b�t�@�[�̃��b�N
	hr = _pointVertexBuffer->Map(0, nullptr,
		(void**)&vertexMap);
	if (hr != S_OK)
		return hr;
	std::copy(std::begin(vertices), std::end(vertices),
		vertexMap);
	// ���_�o�b�t�@�[�̉���
	_pointVertexBuffer->Unmap(0, nullptr);

	// ���_�o�b�t�@�[�r���[���쐬
	// ���_�o�b�t�@�[�̃A�h���X[�o�b�t�@�[�̉��z�A�h���X]
	_pointVertexBufferView.BufferLocation = _pointVertexBuffer->GetGPUVirtualAddress();
	// ���o�C�g��[�S�o�C�g��]
	_pointVertexBufferView.SizeInBytes = sizeof(vertices);
	// 1���_������̃o�C�g��
	_pointVertexBufferView.StrideInBytes = sizeof(vertices[0]);

	// �C���f�b�N�X�����쐬
	unsigned short indices[] =
	{
		0,
	};

	// �T�C�Y��[���őS���܂��Ȃ��̂�sizeof(�S�C���f�b�N�X)�Ƃ���]
	resDesc.Width = sizeof(indices);

	// ���\�[�X�I�u�W�F�N�g���쐬
	hr = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_pointIndexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �C���f�b�N�X���̃R�s�[
	unsigned short* indexMap = nullptr;
	// �C���f�b�N�X�o�b�t�@�[�̃��b�N
	hr = _pointIndexBuffer->Map(0, nullptr,
		(void**)&indexMap);
	if (hr != S_OK)
		return hr;
	std::copy(std::begin(indices), std::end(indices),
		indexMap);
	// �C���f�b�N�X�o�b�t�@�[�̉���
	_pointIndexBuffer->Unmap(0, nullptr);

	// �C���f�b�N�X�o�b�t�@�[�r���[���쐬
	// �C���f�b�N�X�o�b�t�@�[�̃A�h���X[�o�b�t�@�[�̉��z�A�h���X]
	_pointIndexBufferView.BufferLocation = _pointIndexBuffer->GetGPUVirtualAddress();
	// ���o�C�g��[�S�o�C�g��]
	_pointIndexBufferView.SizeInBytes = sizeof(indices);
	// �t�H�[�}�b�g
	_pointIndexBufferView.Format = DXGI_FORMAT_R16_UINT;

	return S_OK;
}
// ���_/�C���f�b�N�X�o�b�t�@�[�̐���(��)
HRESULT PointLine::CreateLineVertexIndexBufferView()
{
	HRESULT hr = S_OK;

	// ���_�f�[�^�\����
	struct Vertex
	{
		XMFLOAT4 pos;	// xyz:���W
	};

	Vertex vertices[] =
	{
		{{ 0.0f, 0.0f, 0.0f, 0.0f}},
		{{ 0.0f, 0.0f, 0.0f, 0.0f}},
	};

	// �q�[�v�v���p�e�B�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp = {};
	// �q�[�v�̎��[CPU����A�N�Z�X�ł���(�}�b�v�ł���)]
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// CPU�̃y�[�W���O�ݒ�[�l���Ȃ��Ă��悢]
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	// �������[�v�[�����ǂ���������[CUSTOM�ȊO�̂Ƃ��͂���ł悢]
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	// ���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC resDesc = {};
	// ���p���@[�o�b�t�@�[�Ɏg���̂�BUFFER���w��]
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	// �T�C�Y��[���őS���܂��Ȃ��̂�sizeof(�S���_)�Ƃ���]
	resDesc.Width = sizeof(vertices);
	// �T�C�Y����[���ŕ\�����Ă���̂łP�Ƃ���]
	resDesc.Height = 1;
	// �[�x��z��T�C�Y[�P�ł悢]
	resDesc.DepthOrArraySize = 1;
	// �~�b�v�}�b�v���x��[�P�ł悢]
	resDesc.MipLevels = 1;
	// �t�H�[�}�b�g[�摜�ł͂Ȃ��̂�UNKNOWN�ł悢]
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	// �A���`�G�C���A�V���O�̃p�����[�^�[[�A���`�G�C���A�V���O���s��Ȃ�����SampleDesc.Count = 1]
	resDesc.SampleDesc.Count = 1;
	// �t���O[NONE�ł悢]
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	// ���C�A�E�g[����̓e�N�X�`���[�̃��C�A�E�g�ł͂Ȃ��̂ŁA�������[���ŏ�����Ō㖘�A�����Ă��邱�Ƃ�����]
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// ���\�[�X�I�u�W�F�N�g���쐬
	hr = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_lineVertexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// ���_���̃R�s�[
	Vertex* vertexMap = nullptr;
	// ���_�o�b�t�@�[�̃��b�N
	hr = _lineVertexBuffer->Map(0, nullptr,
		(void**)&vertexMap);
	if (hr != S_OK)
		return hr;
	std::copy(std::begin(vertices), std::end(vertices),
		vertexMap);
	// ���_�o�b�t�@�[�̉���
	_lineVertexBuffer->Unmap(0, nullptr);

	// ���_�o�b�t�@�[�r���[���쐬
	// ���_�o�b�t�@�[�̃A�h���X[�o�b�t�@�[�̉��z�A�h���X]
	_lineVertexBufferView.BufferLocation = _lineVertexBuffer->GetGPUVirtualAddress();
	// ���o�C�g��[�S�o�C�g��]
	_lineVertexBufferView.SizeInBytes = sizeof(vertices);
	// 1���_������̃o�C�g��
	_lineVertexBufferView.StrideInBytes = sizeof(vertices[0]);

	// �C���f�b�N�X�����쐬
	unsigned short indices[] =
	{
		0, 1,
	};

	// �T�C�Y��[���őS���܂��Ȃ��̂�sizeof(�S�C���f�b�N�X)�Ƃ���]
	resDesc.Width = sizeof(indices);

	// ���\�[�X�I�u�W�F�N�g���쐬
	hr = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_lineIndexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �C���f�b�N�X���̃R�s�[
	unsigned short* indexMap = nullptr;
	// �C���f�b�N�X�o�b�t�@�[�̃��b�N
	hr = _lineIndexBuffer->Map(0, nullptr,
		(void**)&indexMap);
	if (hr != S_OK)
		return hr;
	std::copy(std::begin(indices), std::end(indices),
		indexMap);
	// �C���f�b�N�X�o�b�t�@�[�̉���
	_lineIndexBuffer->Unmap(0, nullptr);

	// �C���f�b�N�X�o�b�t�@�[�r���[���쐬
	// �C���f�b�N�X�o�b�t�@�[�̃A�h���X[�o�b�t�@�[�̉��z�A�h���X]
	_lineIndexBufferView.BufferLocation = _lineIndexBuffer->GetGPUVirtualAddress();
	// ���o�C�g��[�S�o�C�g��]
	_lineIndexBufferView.SizeInBytes = sizeof(indices);
	// �t�H�[�}�b�g
	_lineIndexBufferView.Format = DXGI_FORMAT_R16_UINT;

	return S_OK;
}

// �R���X�^���g�o�b�t�@�[�r���[�̐���(�_)
HRESULT PointLine::CreatePointConstantBufferView(MESH* mesh)
{
	HRESULT hr = S_OK;

	// �萔�o�b�t�@�[�̍쐬
	// �q�[�v�v���p�e�B�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp = {};
	// �q�[�v�̎��[CPU����A�N�Z�X�ł���(�}�b�v�ł���)]
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// CPU�̃y�[�W���O�ݒ�[�l���Ȃ��Ă��悢]
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	// �������[�v�[�����ǂ���������[CUSTOM�ȊO�̂Ƃ��͂���ł悢]
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	// ���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC resDesc = {};
	// ���p���@[�o�b�t�@�[�Ɏg���̂�BUFFER���w��]
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	// �T�C�Y��[���őS���܂��Ȃ��̂�sizeof(�S���):256�̔{���Ƃ���]
	resDesc.Width = (sizeof(ConstantBuffer) + 0xff) & ~0xff;
	// �T�C�Y����[���ŕ\�����Ă���̂łP�Ƃ���]
	resDesc.Height = 1;
	// �[�x��z��T�C�Y[�P�ł悢]
	resDesc.DepthOrArraySize = 1;
	// �~�b�v�}�b�v���x��[�P�ł悢]
	resDesc.MipLevels = 1;
	// �t�H�[�}�b�g[�摜�ł͂Ȃ��̂�UNKNOWN�ł悢]
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	// �A���`�G�C���A�V���O�̃p�����[�^�[[�A���`�G�C���A�V���O���s��Ȃ�����SampleDesc.Count = 1]
	resDesc.SampleDesc.Count = 1;
	// �t���O[NONE�ł悢]
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	// ���C�A�E�g[����̓e�N�X�`���[�̃��C�A�E�g�ł͂Ȃ��̂ŁA�������[���ŏ�����Ō㖘�A�����Ă��邱�Ƃ�����]
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// ���\�[�X�I�u�W�F�N�g���쐬
	hr = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(mesh->constBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �f�X�N���v�^�q�[�v���쐬
	// �q�[�v�̋L�q��p��
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	// ��ނ̑I��[CBV �� SRV�𗘗p]
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	// GPU���P�����g�p����z��
	heapDesc.NodeMask = 0;
	// �f�X�N���v�^���w��[CBV:�P��]
	heapDesc.NumDescriptors = 1;
	// ���Ɏw��Ȃ�[�V�F�[�_�[�����猩����K�v����]
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// �f�X�N���v�^�q�[�v���쐬
	hr = _dev->CreateDescriptorHeap(&heapDesc
		, IID_PPV_ARGS(mesh->basicDescHeap.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �f�B�X�N���v�^�̐擪�n���h�����擾���Ă���
	auto basicHeapHandle = mesh->basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = mesh->constBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)mesh->constBuffer->GetDesc().Width;

	// �萔�o�b�t�@�[�r���[�̍쐬
	_dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	return S_OK;
}
// �R���X�^���g�o�b�t�@�[�r���[�̐���(��)
HRESULT PointLine::CreateLineConstantBufferView(MESH* mesh)
{
	HRESULT hr = S_OK;

	// �萔�o�b�t�@�[�̍쐬
	// �q�[�v�v���p�e�B�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp = {};
	// �q�[�v�̎��[CPU����A�N�Z�X�ł���(�}�b�v�ł���)]
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// CPU�̃y�[�W���O�ݒ�[�l���Ȃ��Ă��悢]
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	// �������[�v�[�����ǂ���������[CUSTOM�ȊO�̂Ƃ��͂���ł悢]
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	// ���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC resDesc = {};
	// ���p���@[�o�b�t�@�[�Ɏg���̂�BUFFER���w��]
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	// �T�C�Y��[���őS���܂��Ȃ��̂�sizeof(�S���):256�̔{���Ƃ���]
	resDesc.Width = (sizeof(ConstantBuffer) + 0xff) & ~0xff;
	// �T�C�Y����[���ŕ\�����Ă���̂łP�Ƃ���]
	resDesc.Height = 1;
	// �[�x��z��T�C�Y[�P�ł悢]
	resDesc.DepthOrArraySize = 1;
	// �~�b�v�}�b�v���x��[�P�ł悢]
	resDesc.MipLevels = 1;
	// �t�H�[�}�b�g[�摜�ł͂Ȃ��̂�UNKNOWN�ł悢]
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	// �A���`�G�C���A�V���O�̃p�����[�^�[[�A���`�G�C���A�V���O���s��Ȃ�����SampleDesc.Count = 1]
	resDesc.SampleDesc.Count = 1;
	// �t���O[NONE�ł悢]
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	// ���C�A�E�g[����̓e�N�X�`���[�̃��C�A�E�g�ł͂Ȃ��̂ŁA�������[���ŏ�����Ō㖘�A�����Ă��邱�Ƃ�����]
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// ���\�[�X�I�u�W�F�N�g���쐬
	hr = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(mesh->constBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �f�X�N���v�^�q�[�v���쐬
	// �q�[�v�̋L�q��p��
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	// ��ނ̑I��[CBV �� SRV�𗘗p]
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	// GPU���P�����g�p����z��
	heapDesc.NodeMask = 0;
	// �f�X�N���v�^���w��[CBV:�P��]
	heapDesc.NumDescriptors = 1;
	// ���Ɏw��Ȃ�[�V�F�[�_�[�����猩����K�v����]
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// �f�X�N���v�^�q�[�v���쐬
	hr = _dev->CreateDescriptorHeap(&heapDesc
		, IID_PPV_ARGS(mesh->basicDescHeap.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �f�B�X�N���v�^�̐擪�n���h�����擾���Ă���
	auto basicHeapHandle = mesh->basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = mesh->constBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)mesh->constBuffer->GetDesc().Width;

	// �萔�o�b�t�@�[�r���[�̍쐬
	_dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	return S_OK;
}
// �`��(�_)
void PointLine::DrawPoint()
{
	if (_pointInstanceEntry == 0) {
		return;
	}
	for (int m = 0, size = (int)_pointMeshs.size(); m < size; ++m)
	{
		// �r���[�s��
		XMMATRIX viewMat = Camera::GetInstance()->GetView();

		// �v���W�F�N�V�����s��
		XMMATRIX projectionMat = Camera::GetInstance()->GetProjection();

		// �萔���̍X�V

		int EntryNum = _pointInstanceEntry - m * 1000;
		if (EntryNum >= instanceMax)
		{
			EntryNum = instanceMax;
		}

		ConstantBuffer* mapMetaData = nullptr;	// �}�b�v��������|�C���^�[
		_pointMeshs[m]->constBuffer->Map(0, nullptr, (void**)&mapMetaData);	// �}�b�v
		XMStoreFloat4x4(&mapMetaData->viewMat, viewMat);
		XMStoreFloat4x4(&mapMetaData->projectionMat, projectionMat);
		for (int i = 0; i < EntryNum; ++i)
		{
			mapMetaData->startPos[i] = _pointMetaData[m].startPos[i];
			mapMetaData->endPos[i] = _pointMetaData[m].endPos[i];
			mapMetaData->color[i] = _pointMetaData[m].color[i];
		}
		_pointMeshs[m]->constBuffer->Unmap(0, nullptr);		// �A���}�b�v

		// �O���t�B�b�N�X�p�C�v���C���X�e�[�g�̎w��
		_cmdList->SetPipelineState(_pointPipelineState.Get());
		// ���[�g�V�O�l�`���[�̎w��
		_cmdList->SetGraphicsRootSignature(_rootSignature.Get());

		// �f�B�X�N���v�^�[�q�[�v�̎w��
		_cmdList->SetDescriptorHeaps(1, _pointMeshs[m]->basicDescHeap.GetAddressOf());

		// ���[�g�p�����[�^�[�ƃf�B�X�N���v�^�[�q�[�v�̊֘A�t��
		_cmdList->SetGraphicsRootDescriptorTable(
			0,	// ���[�g�p�����[�^�[�C���f�b�N�X
			_pointMeshs[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart());

		// ���_���̎w��[��]
		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		// ���_�o�b�t�@�[�̎w��
		_cmdList->IASetVertexBuffers(0, 1, &_pointMeshs[m]->vertexBufferView);

		// �C���f�b�N�X�o�b�t�@�[�̎w��
		_cmdList->IASetIndexBuffer(&_pointMeshs[m]->indexBufferView);

		// �`�施��
		_cmdList->DrawIndexedInstanced(2, EntryNum, 0, 0, 0);
	}
}
// �`��(��)
void PointLine::DrawLine()
{
	if (_lineInstanceEntry == 0) {
		return;
	}
	for (int m = 0, size = (int)_lineMeshs.size(); m < size; ++m)
	{
		// �r���[�s��
		XMMATRIX viewMat = Camera::GetInstance()->GetView();

		// �v���W�F�N�V�����s��
		XMMATRIX projectionMat = Camera::GetInstance()->GetProjection();

		// �萔���̍X�V
		int EntryNum = _lineInstanceEntry - m * 1000;
		if (EntryNum >= instanceMax)
		{
			EntryNum = instanceMax;
		}

		ConstantBuffer* mapMetaData = nullptr;	// �}�b�v��������|�C���^�[
		_lineMeshs[m]->constBuffer->Map(0, nullptr, (void**)&mapMetaData);	// �}�b�v
		XMStoreFloat4x4(&mapMetaData->viewMat, viewMat);
		XMStoreFloat4x4(&mapMetaData->projectionMat, projectionMat);
		for (int i = 0; i < EntryNum; ++i)
		{
			mapMetaData->startPos[i] = _lineMetaData[m].startPos[i];
			mapMetaData->endPos[i] = _lineMetaData[m].endPos[i];
			mapMetaData->color[i] = _lineMetaData[m].color[i];
		}
		_lineMeshs[m]->constBuffer->Unmap(0, nullptr);		// �A���}�b�v

		// �O���t�B�b�N�X�p�C�v���C���X�e�[�g�̎w��
		_cmdList->SetPipelineState(_linePipelineState.Get());
		// ���[�g�V�O�l�`���[�̎w��
		_cmdList->SetGraphicsRootSignature(_rootSignature.Get());

		// �f�B�X�N���v�^�[�q�[�v�̎w��
		_cmdList->SetDescriptorHeaps(1, _lineMeshs[m]->basicDescHeap.GetAddressOf());

		// ���[�g�p�����[�^�[�ƃf�B�X�N���v�^�[�q�[�v�̊֘A�t��
		_cmdList->SetGraphicsRootDescriptorTable(
			0,	// ���[�g�p�����[�^�[�C���f�b�N�X
			_lineMeshs[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart());

		// ���_���̎w��[��]
		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		// ���_�o�b�t�@�[�̎w��
		_cmdList->IASetVertexBuffers(0, 1, &_lineMeshs[m]->vertexBufferView);

		// �C���f�b�N�X�o�b�t�@�[�̎w��
		_cmdList->IASetIndexBuffer(&_lineMeshs[m]->indexBufferView);

		// �`�施��
		_cmdList->DrawIndexedInstanced(2, EntryNum, 0, 0, 0);
	}
}

// �_
// �w�肵���ԍ����폜
void PointLine::PointDeleate(int& entryPos) {
	if (entryPos != -1)
	{
		int EntryPos = entryPos % instanceMax;
		int ArrayNum = entryPos / instanceMax;
		_pointDataCheck[ArrayNum]._dataCheck[EntryPos] = false;
		// �g���ĂȂ�EntryPos�͈ꎞ�I�ɔ�΂�
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
	// �ŏ���������1000�Ԗڂ܂Ŏg���؂����玟�̃o�b�t�@�[��p�ӂ���
	int num = _pointInstanceEntry % instanceMax;
	if (num == 0 && entryPos == -1)
	{
		_pointMeshs.push_back(new MESH);

		_pointMeshs.back()->vertexBuffer = _pointVertexBuffer;
		_pointMeshs.back()->vertexBufferView = _pointVertexBufferView;
		_pointMeshs.back()->indexBuffer = _pointIndexBuffer;
		_pointMeshs.back()->indexBufferView = _pointIndexBufferView;
		// �R���X�^���g�o�b�t�@�[�r���[�̐���
		CreateLineConstantBufferView(_pointMeshs.back());
		_pointMetaData.resize(_pointMeshs.size());
		_pointDataCheck.resize(_pointMeshs.size());
	}
	// �G���g���[�ԍ�
	int EntryPos;
	// �g�p���郁�b�V���̔ԍ�
	int ArrayNum;
	if (entryPos == -1)
	{
		// �󂢂Ă�G���g���[�ԍ���T��
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

		// �������郉�C�������
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
		// �������郉�C�������
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

// ��
// �w�肵���ԍ����폜
void PointLine::LineDeleate(int& entryPos) {
	if (entryPos != -1)
	{
		int EntryPos = entryPos % instanceMax;
		int ArrayNum = entryPos / instanceMax;
		_lineDataCheck[ArrayNum]._dataCheck[EntryPos] = false;
		// �g���ĂȂ�EntryPos�͈ꎞ�I�ɔ�΂�
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
	// �ŏ���������1000�Ԗڂ܂Ŏg���؂����玟�̃o�b�t�@�[��p�ӂ���
	int num = _lineInstanceEntry % instanceMax;
	if (num == 0 && entryPos == -1)
	{
		_lineMeshs.push_back(new MESH);

		_lineMeshs.back()->vertexBuffer = _lineVertexBuffer;
		_lineMeshs.back()->vertexBufferView = _lineVertexBufferView;
		_lineMeshs.back()->indexBuffer = _lineIndexBuffer;
		_lineMeshs.back()->indexBufferView = _lineIndexBufferView;
		// �R���X�^���g�o�b�t�@�[�r���[�̐���
		CreateLineConstantBufferView(_lineMeshs.back());
		_lineMetaData.resize(_lineMeshs.size());
		_lineDataCheck.resize(_lineMeshs.size());
	}
	// �G���g���[�ԍ�
	int EntryPos;
	// �g�p���郁�b�V���̔ԍ�
	int ArrayNum;
	if (entryPos == -1)
	{
		// �󂢂Ă�G���g���[�ԍ���T��
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

		// �������郉�C�������
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
		// �������郉�C�������
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
