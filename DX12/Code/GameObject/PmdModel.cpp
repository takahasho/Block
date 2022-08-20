// --------------------- Include File --------------------
#include "PmdModel.h"
#include "../Camera/Camera.h"
#include "../DX12System.h"
PmdModel* PmdModel::_instance = nullptr;			// �C���X�^���X�̎���
// �R���X�g���N�^�[
PmdModel::PmdModel()
{

}
// �f�X�g���N�^�[
PmdModel::~PmdModel()
{

}
// ����
void PmdModel::Create()
{
	if (!_instance)
	{
		_instance = new PmdModel();

		// ���[�g�V�O�l�`���[�̐���
		_instance->CreateRootSignature();

		// �`��p�C�v���C���̐���
		_instance->CreateGraphicsPipelineState();

		// ���F�e�N�X�`���[�̐���
		_instance->CreateWhiteTexture();

		// ���F�e�N�X�`���[�̐���
		_instance->CreateBlackTexture();
	}
}
// �j��
void PmdModel::Destroy()
{
	delete _instance;
}
// ���[�g�V�O�l�`���[�̐���
HRESULT PmdModel::CreateRootSignature()
{
	HRESULT hr = S_OK;

	// �f�B�X�N���v�^�[�����W���쐬
	D3D12_DESCRIPTOR_RANGE descTblRange[3] = {};

	// �萔�o�b�t�@�[(���W)
	// �f�B�X�N���v�^�[�̐�[�P��]
	descTblRange[0].NumDescriptors = 1;
	// �����W�̎��[CBV:�R���X�^���g�o�b�t�@�[�r���[]
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// �擪���W�X�^�[�ԍ�[0��I��]
	descTblRange[0].BaseShaderRegister = 0;
	// �I�t�Z�b�g���w��[�A�������f�B�X�N���v�^�[�����W���O�̃f�B�X�N���v�^�����W�̒���ɂ���]
	descTblRange[0].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// �萔�o�b�t�@�[(�}�e���A��)
	// �f�B�X�N���v�^�[�̐�[�P��]
	descTblRange[1].NumDescriptors = 1;
	// �����W�̎��[CBV:�R���X�^���g�o�b�t�@�[�r���[]
	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// �擪���W�X�^�[�ԍ�[1��I��]
	descTblRange[1].BaseShaderRegister = 1;
	// �I�t�Z�b�g���w��[�A�������f�B�X�N���v�^�[�����W���O�̃f�B�X�N���v�^�����W�̒���ɂ���]
	descTblRange[1].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// �e�N�X�`���[�o�b�t�@�[�r���[
	// �f�B�X�N���v�^�[�̐�[3��]
	descTblRange[2].NumDescriptors = 3;
	// �����W�̎��[SRV:�V�F�[�_�[���\�[�X�r���[]
	descTblRange[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	// �擪���W�X�^�[�ԍ�[0��1��I��]
	descTblRange[2].BaseShaderRegister = 0;
	// �I�t�Z�b�g���w��[�A�������f�B�X�N���v�^�[�����W���O�̃f�B�X�N���v�^�����W�̒���ɂ���]
	descTblRange[2].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	// ���[�g�p�����[�^�[���쐬
	D3D12_ROOT_PARAMETER rootParam[2] = {};
	// ��ʂ̑I��[�f�B�X�N���v�^�[�e�[�u����I��]
	rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// �ǂ̃V�F�[�_�[���痘�p�\��[�S�V�F�[�_�[]
	rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// �f�B�X�N���v�^�[�����W�̃A�h���X
	rootParam[0].DescriptorTable.pDescriptorRanges = &descTblRange[0];
	// �f�B�X�N���v�^�[�����W�̐�
	rootParam[0].DescriptorTable.NumDescriptorRanges = 1;
	// ��ʂ̑I��[�f�B�X�N���v�^�[�e�[�u����I��]
	rootParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// �ǂ̃V�F�[�_�[���痘�p�\��[�S�V�F�[�_�[]
	rootParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// �f�B�X�N���v�^�[�����W�̃A�h���X
	rootParam[1].DescriptorTable.pDescriptorRanges = &descTblRange[1];
	// �f�B�X�N���v�^�[�����W�̐�
	rootParam[1].DescriptorTable.NumDescriptorRanges = 2;

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
	rootSignatureDesc.pParameters = rootParam;
	// �p�����[�^�[�̐�
	rootSignatureDesc.NumParameters = 2;
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
	hr = DX12::GetInstance()->GetDevice()->CreateRootSignature(
		0,	// nodemask�B�O�ł悢
		rootSigBlob->GetBufferPointer(),	// �A�h���X
		rootSigBlob->GetBufferSize(),		// �T�C�Y
		IID_PPV_ARGS(_rootSignature.GetAddressOf()));	// �A�h���X��[��
	if (hr != S_OK)
		return E_FAIL;

	return S_OK;
}

// �`��p�C�v���C���̐���
HRESULT PmdModel::CreateGraphicsPipelineState()
{
	HRESULT hr = S_OK;

	// �V�F�[�_�[�I�u�W�F�N�g�ǂݍ��ݗp
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// ���_�V�F�[�_�[�̓ǂݍ���
	hr = D3DCompileFromFile(
		L".\\Shader\\Pmd\\PmdVertexShader.hlsl",			// �V�F�[�_�[��
		nullptr,							// define�͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// �C���N���[�h�̓f�t�H���g
		"PmdVS", "vs_5_0",				// �֐��� PmdVS�A�ΏۃV�F�[�_�[�� vs_5_0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�N�l����эœK���Ȃ�	
		0,									// �G�t�F�N�g�R���p�C���I�v�V����[0�𐄏�]
		&vsBlob,							// �󂯎�邽�߂̃|�C���^�[�̃A�h���X
		&errorBlob);						// �G���[�p�|�C���^�[�̃A�h���X[�G���[����errorBlob�Ƀ��b�Z�[�W������]
	if (FAILED(hr))
	{
		// �G���[���b�Z�[�W
		BlobErrorMessange(hr, errorBlob);
		return E_FAIL;
	}

	// �s�N�Z���V�F�[�_�[�̓ǂݍ���
	hr = D3DCompileFromFile(
		L".\\Shader\\Pmd\\PmdPixelShader.hlsl",			// �V�F�[�_�[��
		nullptr,							// define�͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// �C���N���[�h�̓f�t�H���g
		"PmdPS", "ps_5_0",				// �֐��� PmdPS�A�ΏۃV�F�[�_�[�� ps_5_0
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
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{	// normal���
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{	// uv���
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{	// �{�[���ԍ����
			"BONE_NO", 0, DXGI_FORMAT_R16G16_UINT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{	// �{�[���E�F�C�g���
			"WEIGHT", 0, DXGI_FORMAT_R8_UINT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{	// �G�b�W���
			"EDGE_FLG", 0, DXGI_FORMAT_R8_UINT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	// �O���t�B�b�N�X�p�C�v���C���X�e�[�g(PSO:pipeline state object)���쐬
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipelineDesc = {};
	// ���[�g�V�O�l�`���[
	gpipelineDesc.pRootSignature = _rootSignature.Get();
	// ���_�V�F�[�_�[�̃A�h���X�ƃT�C�Y
	gpipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	gpipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
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

	// �v���~�e�B�u�̃^�C�v[�O�p�`]
	gpipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

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

	// �[�x�X�e���V���o�b�t�@�[�̗L���̗L��[�L��]
	gpipelineDesc.DepthStencilState.DepthEnable = true;
	// �[�x�X�e���V���o�b�t�@�[�̏������ݕ��@[�������ݗL��]
	gpipelineDesc.DepthStencilState.DepthWriteMask =
		D3D12_DEPTH_WRITE_MASK_ALL;
	// �[�x�X�e���V���o�b�t�@�[�̐�����@[�����������̗p]
	gpipelineDesc.DepthStencilState.DepthFunc =
		D3D12_COMPARISON_FUNC_LESS;
	// �t�H�[�}�b�g���w��
	gpipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// �O���t�B�b�N�p�C�v���C���X�e�[�g�I�u�W�F�N�g�𐶐�
	hr = DX12::GetInstance()->GetDevice()->CreateGraphicsPipelineState(
		&gpipelineDesc, IID_PPV_ARGS(_pipelineState.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	return S_OK;
}
// ���_/�C���f�b�N�X�o�b�t�@�[�̐���
HRESULT PmdModel::CreateVertexIndexBufferView(PMDMODEL& model)
{
	HRESULT hr = S_OK;
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
	resDesc.Width = model.mesh.vertices.size();
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
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(model.mesh.vertexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// ���_���̃R�s�[
	unsigned char* vertexMap = nullptr;
	// ���_�o�b�t�@�[�̃��b�N
	hr = model.mesh.vertexBuffer->Map(0, nullptr,
		(void**)&vertexMap);
	if (hr != S_OK)
		return hr;
	std::copy(std::begin(model.mesh.vertices), std::end(model.mesh.vertices),
		vertexMap);
	// ���_�o�b�t�@�[�̉���
	model.mesh.vertexBuffer->Unmap(0, nullptr);

	// ���_�o�b�t�@�[�r���[���쐬
	// ���_�o�b�t�@�[�̃A�h���X[�o�b�t�@�[�̉��z�A�h���X]
	model.mesh.vertexBufferView.BufferLocation = model.mesh.vertexBuffer->GetGPUVirtualAddress();
	// ���o�C�g��[�S�o�C�g��]
	model.mesh.vertexBufferView.SizeInBytes = (UINT)model.mesh.vertices.size();
	// 1���_������̃o�C�g��
	model.mesh.vertexBufferView.StrideInBytes = _pmdVertex_size;

	// �T�C�Y��[���őS���܂��Ȃ��̂�sizeof(�S�C���f�b�N�X)�Ƃ���]
	resDesc.Width = model.mesh.indices.size() * sizeof(model.mesh.indices[0]);

	// ���\�[�X�I�u�W�F�N�g���쐬
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(model.mesh.indexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �C���f�b�N�X���̃R�s�[
	unsigned short* indexMap = nullptr;
	// �C���f�b�N�X�o�b�t�@�[�̃��b�N
	hr = model.mesh.indexBuffer->Map(0, nullptr,
		(void**)&indexMap);
	if (hr != S_OK)
		return hr;
	std::copy(std::begin(model.mesh.indices), std::end(model.mesh.indices),
		indexMap);
	// �C���f�b�N�X�o�b�t�@�[�̉���
	model.mesh.indexBuffer->Unmap(0, nullptr);

	// �C���f�b�N�X�o�b�t�@�[�r���[���쐬
	// �C���f�b�N�X�o�b�t�@�[�̃A�h���X[�o�b�t�@�[�̉��z�A�h���X]
	model.mesh.indexBufferView.BufferLocation = model.mesh.indexBuffer->GetGPUVirtualAddress();
	// ���o�C�g��[�S�o�C�g��]
	model.mesh.indexBufferView.SizeInBytes = (UINT)model.mesh.indices.size() * sizeof(model.mesh.indices[0]);
	// �t�H�[�}�b�g
	model.mesh.indexBufferView.Format = DXGI_FORMAT_R16_UINT;

	return S_OK;
}
// �V�F�[�_�[���\�[�X�̐���
HRESULT PmdModel::CreateShaderResource(
	ComPtr<ID3D12Resource>* texBuffer,
	const std::string fileName)
{
	HRESULT hr = S_OK;

	auto it = _resourceTable.find(fileName);
	if (it != _resourceTable.end())
	{
		// �e�[�u�����ɂ������烍�[�h����̂ł͂Ȃ�
		// �}�b�v���̃��\�[�X��Ԃ�
		texBuffer = it->second;
		return S_OK;
	}
	// �摜�T�C�Y�̎擾�ƃC���[�W�f�[�^�[�m��
	BYTE* pImageData = nullptr;
	SIZE size = {};

	// �摜���̓ǂݍ���
	if (FAILED(LoadTextureFromFile(fileName.c_str(),
		&pImageData, &size)))
		return E_FAIL;

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
	resDesc.Width = (UINT64)(
		AlignmentSize(size.cx, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * AlignmentSize(size.cy, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * bytePerPixel);
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
	resDesc.Width = (UINT64)AlignmentSize(size.cx, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
	// �T�C�Y����
	resDesc.Height = (UINT)AlignmentSize(size.cy, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
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
		IID_PPV_ARGS(texBuffer->ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �A�b�v���[�h���\�[�X�ւ̃}�b�v
	uint8_t* mapForImg = nullptr;	// image->pixels�Ɠ����^�ɂ���
	hr = _uploadBuffer->Map(0, nullptr, (void**)&mapForImg);	// �}�b�v
	if (FAILED(hr))
		return hr;
	std::copy_n(pImageData, AlignmentSize(size.cx, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * AlignmentSize(size.cy, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * bytePerPixel, mapForImg);	// �R�s�[
	_uploadBuffer->Unmap(0, nullptr);	// �A���}�b�v

	// �C���[�W���̉��
	delete[] pImageData;

	// �R�s�[�e�N�X�`���[���M�I���̐ݒ�
	// �R�s�[��(�A�b�v���[�h��)�ݒ�
	D3D12_TEXTURE_COPY_LOCATION src = {};
	src.pResource = _uploadBuffer.Get();	// ���ԃo�b�t�@�[
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;	// �v�b�g�v�����g�w��[�o�b�t�@�[���\�[�X]
	src.PlacedFootprint.Offset = 0;							// �I�t�Z�b�g
	src.PlacedFootprint.Footprint.Width = (UINT)AlignmentSize(size.cx, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);		// ��
	src.PlacedFootprint.Footprint.Height = (UINT)AlignmentSize(size.cy, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);	// ����
	src.PlacedFootprint.Footprint.Depth = 1;				// �[��
	src.PlacedFootprint.Footprint.RowPitch =
		(UINT)AlignmentSize(size.cx, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * bytePerPixel;// 1�s������̃o�C�g��
	src.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// �t�H�[�}�b�g

	// �R�s�[��ݒ�
	D3D12_TEXTURE_COPY_LOCATION dst = {};
	dst.pResource = texBuffer->Get();
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
	BarrierDesc.Transition.pResource = texBuffer->Get();				// �e�N�X�`���[���\�[�X
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

	_resourceTable[fileName] = texBuffer;

	return S_OK;
}

// �R���X�^���g�o�b�t�@�[�r���[�̐���
HRESULT PmdModel::CreateConstantBufferView(PMDMODEL& model)
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
	resDesc.Width = (sizeof(SceneMatrix) + 0xff) & ~0xff;
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
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(model.mesh.constBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �f�X�N���v�^�q�[�v���쐬
	// �q�[�v�̋L�q��p��
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	// ��ނ̑I��[CBV�𗘗p]
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	// GPU���P�����g�p����z��
	heapDesc.NodeMask = 0;
	// �f�X�N���v�^���w��[CBV�P�� + �}�e���A���̐�(����+�e�N�X�`���[+�X�t�B�A�}�b�v) + �X�t�B�A�}�b�v(���Z)]
	heapDesc.NumDescriptors = 1 + model.mesh.materialNum * 4;
	// ���Ɏw��Ȃ�[�V�F�[�_�[�����猩����K�v����]
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// �f�X�N���v�^�q�[�v���쐬
	hr = DX12::GetInstance()->GetDevice()->CreateDescriptorHeap(&heapDesc
		, IID_PPV_ARGS(model.mesh.basicDescHeap.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	// �f�B�X�N���v�^�̐擪�n���h�����擾���Ă���
	auto basicHeapHandle = model.mesh.basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = model.mesh.constBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)model.mesh.constBuffer->GetDesc().Width;

	// �萔�o�b�t�@�[�r���[�̍쐬(���W�Ɠ������e�ϊ�)
	DX12::GetInstance()->GetDevice()->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	// ���̏ꏊ�Ɉړ�
	basicHeapHandle.ptr +=
		DX12::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize
		(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	auto inc = DX12::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize
	(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// �萔�o�b�t�@�[�r���[�̍쐬(�}�e���A�����)
	cbvDesc.BufferLocation = model.mesh.materialBuffer->GetGPUVirtualAddress();
	auto materialBufferSize = sizeof(MaterialForHlsl);
	materialBufferSize = (materialBufferSize + 0xff) & ~0xff;
	cbvDesc.SizeInBytes = (UINT)materialBufferSize;// �}�e���A����256�A���C�����g�T�C�Y
	for (int i = 0; i < (int)model.mesh.materialNum; ++i)
	{
		// �}�e���A��
		DX12::GetInstance()->GetDevice()->CreateConstantBufferView(
			&cbvDesc, basicHeapHandle);
		cbvDesc.BufferLocation += materialBufferSize;
		// ���̏ꏊ�Ɉړ�
		basicHeapHandle.ptr += inc;

		// �V�F�[�_�[���\�[�X�r���[�쐬
		// �e�N�X�`���[
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

		// ���̏ꏊ�Ɉړ�
		basicHeapHandle.ptr += inc;

		// �X�t�B�A�}�b�v( ��Z)
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

		// ���̏ꏊ�Ɉړ�
		basicHeapHandle.ptr += inc;

		// �X�t�B�A�}�b�v( ��Z)
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

		// ���̏ꏊ�Ɉړ�
		basicHeapHandle.ptr += inc;
	}
	return S_OK;
}
// �`��
void PmdModel::Draw(PMDMODEL model)
{
	XMMATRIX world_matrix;
	XMMATRIX translate = XMMatrixTranslation(model.position.x, model.position.y, model.position.z);
	XMMATRIX rotate_x = XMMatrixRotationX(DirectX::XMConvertToRadians(model.angle.x));
	XMMATRIX rotate_y = XMMatrixRotationY(DirectX::XMConvertToRadians(model.angle.y));
	XMMATRIX rotate_z = XMMatrixRotationZ(DirectX::XMConvertToRadians(model.angle.z));
	XMMATRIX scale_mat = XMMatrixScaling(model.size.x, model.size.y, model.size.z);
	world_matrix = scale_mat * rotate_x * rotate_y * rotate_z * translate;

	// =====�r���[�s��=====
	XMMATRIX viewMat = Camera::GetInstance()->GetView();
	XMFLOAT3 eyePos = Camera::GetInstance()->GetEyePosition();
	// =====�v���W�F�N�V�����s��=====
	XMMATRIX projMat = Camera::GetInstance()->GetProjection();

	// �萔���̍X�V
	SceneMatrix* mapMatrix = nullptr;	// �}�b�v��������|�C���^�[
	model.mesh.constBuffer->Map(0, nullptr, (void**)&mapMatrix);	// �}�b�v
	mapMatrix->world = world_matrix;
	mapMatrix->view = viewMat;
	mapMatrix->Projection = projMat;
	mapMatrix->eye = eyePos;
	mapMatrix->LightVector = Camera::GetInstance()->GetLightVector();
	model.mesh.constBuffer->Unmap(0, nullptr);		// �A���}�b�v

	// �O���t�B�b�N�X�p�C�v���C���X�e�[�g�̎w��
	DX12::GetInstance()->CommandList()->SetPipelineState(_pipelineState.Get());
	// ���[�g�V�O�l�`���[�̎w��
	DX12::GetInstance()->CommandList()->SetGraphicsRootSignature(_rootSignature.Get());

	// �f�B�X�N���v�^�[�q�[�v�̎w��
	DX12::GetInstance()->CommandList()->SetDescriptorHeaps(1, model.mesh.basicDescHeap.GetAddressOf());

	// ���[�g�p�����[�^�[�ƃf�B�X�N���v�^�[�q�[�v�̊֘A�t��
	DX12::GetInstance()->CommandList()->SetGraphicsRootDescriptorTable(
		0,	// ���[�g�p�����[�^�[�C���f�b�N�X
		model.mesh.basicDescHeap->GetGPUDescriptorHandleForHeapStart());

	// ���_���̎w��[�O�p�`]
	DX12::GetInstance()->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���_�o�b�t�@�[�̎w��
	DX12::GetInstance()->CommandList()->IASetVertexBuffers(0, 1, &model.mesh.vertexBufferView);

	// �C���f�b�N�X�o�b�t�@�[�̎w��
	DX12::GetInstance()->CommandList()->IASetIndexBuffer(&model.mesh.indexBufferView);

	// �f�B�X�N���v�^�̐擪�n���h�����擾���Ă���
	auto basicHeapHandle = model.mesh.basicDescHeap->GetGPUDescriptorHandleForHeapStart();
	// ���̏ꏊ�Ɉړ�
	basicHeapHandle.ptr +=
		DX12::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize
		(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// �ŏ��̓I�t�Z�b�g�Ȃ�
	unsigned int idxOffset = 0;

	auto cbvsrvIncSize = DX12::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize
	(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 4;


	for (auto& m : model.mesh.materials)
	{
		// ���[�g�p�����[�^�[�ƃf�B�X�N���v�^�[�q�[�v�̊֘A�t��
		DX12::GetInstance()->CommandList()->SetGraphicsRootDescriptorTable(
			1,					// ���[�g�p�����[�^�[�C���f�b�N�X
			basicHeapHandle);	// �q�[�v�A�h���X

		// �`�施��
		DX12::GetInstance()->CommandList()->DrawIndexedInstanced(m.indicesNum, 1, idxOffset, 0, 0);

		// �q�[�v�|�C���^�[�ƃC���f�b�N�X�����ɐi�߂�
		basicHeapHandle.ptr += cbvsrvIncSize;
		idxOffset += m.indicesNum;
	}
}

HRESULT PmdModel::CreatePMD(PMDMODEL& model, const char* file_name)
{
	HRESULT hr = S_OK;

	// PMF�t�@�C���ǂݍ���
	if (FAILED(LoadPMD(model, file_name)))
		return E_FAIL;

	// ���_�C���f�b�N�X�o�b�t�@�[�r���[�̐���
	if (FAILED(CreateVertexIndexBufferView(model)))
		return E_FAIL;

	// �}�e���A���o�b�t�@�[�̐���
	if (FAILED(CreateMaterialBuffer(model, file_name)))
		return E_FAIL;

	// �e�N�X�`���̐���
	if (FAILED(CreateTexture(model)))
		return E_FAIL;

	// �R���X�^���g�o�b�t�@�[�r���[�̐���
	if (FAILED(CreateConstantBufferView(model)))
		return E_FAIL;

	return S_OK;
}
// pmd�t�@�C���̓ǂݍ���
HRESULT PmdModel::LoadPMD(PMDMODEL& model, const char* file_name)
{
	HRESULT hr = S_OK;

	FILE* fp = nullptr;	// �t�@�C���|�C���^�[
	fopen_s(&fp, file_name, "rb");
	if (fp == nullptr)
	{
		return E_FAIL;
	}

	// �擪��3�������V�O�l�`���["Pmd"������̂ŁA�ǂݔ�΂�
	char signature[3] = {};	// �V�O�l�`���[
	fread(signature, sizeof(signature), 1, fp);

	// �w�b�_�[�̓ǂݍ���
	PMDHeader pmdHeader = {};
	fread(&pmdHeader, sizeof(pmdHeader), 1, fp);

	// ���_���̓ǂݍ���
	fread(&model.mesh.verticesNum, sizeof(model.mesh.verticesNum), 1, fp);

	// ���_���̓ǂݍ���
	// �o�b�t�@�[�̊m��
	model.mesh.vertices.resize(model.mesh.verticesNum * _pmdVertex_size);
	fread(model.mesh.vertices.data(), model.mesh.vertices.size(), 1, fp);

	// �C���f�b�N�X���̓ǂݍ���
	fread(&model.mesh.indicesNum, sizeof(model.mesh.indicesNum), 1, fp);

	// �C���f�b�N�X���̓ǂݍ���
	// �o�b�t�@�[�̊m��
	model.mesh.indices.resize(model.mesh.indicesNum);
	fread(model.mesh.indices.data(), model.mesh.indices.size() * sizeof(model.mesh.indices[0]), 1, fp);

	// �}�e���A�����̓ǂݍ���
	fread(&model.mesh.materialNum, sizeof(model.mesh.materialNum), 1, fp);

	// �}�e���A�����̓ǂݍ���
	// �o�b�t�@�[�̊m��
	model.mesh.pmdMaterials.resize(model.mesh.materialNum);
	fread(model.mesh.pmdMaterials.data(),
		model.mesh.pmdMaterials.size() * sizeof(PMDMaterial), 1, fp);

	fclose(fp);

	return S_OK;
}
// �}�e���A���o�b�t�@�[�̐���
HRESULT PmdModel::CreateMaterialBuffer(PMDMODEL& model, const char* file_name)
{
	HRESULT hr = S_OK;

	model.mesh.toonTexBuffer.resize(model.mesh.pmdMaterials.size());
	for (int i = 0; i < model.mesh.pmdMaterials.size(); ++i)
	{
		// �g�D�[�����\�[�X�̓ǂݍ���
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
	// �o�b�t�@�[�̊m��
	model.mesh.materials.resize(model.mesh.pmdMaterials.size());
	// �R�s�[
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

	// �}�e���A���p�o�b�t�@�[���쐬
	auto materialBufferSize = sizeof(MaterialForHlsl);
	materialBufferSize = (materialBufferSize + 0xff) & ~0xff;

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
	// �T�C�Y��[���őS���܂��Ȃ��̂Ń}�e���A�����T�C�Y*���Ƃ���]
	resDesc.Width = materialBufferSize * model.mesh.materialNum;
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
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(model.mesh.materialBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �}�e���A�����̃R�s�[
	char* mapMaterial = nullptr;
	// �}�e���A���o�b�t�@�[�̃��b�N
	hr = model.mesh.materialBuffer->Map(0, nullptr,
		(void**)&mapMaterial);
	if (hr != S_OK)
		return hr;
	for (auto& m : model.mesh.materials)
	{
		*((MaterialForHlsl*)mapMaterial) = m.material;// �f�[�^�R�s�[
		mapMaterial += materialBufferSize;// ���̃A���C�����g�ʒu�܂Ői�߂�(256�̔{��)
	}
	// �}�e���A���o�b�t�@�[�̉���
	model.mesh.materialBuffer->Unmap(0, nullptr);

	return S_OK;
}
// �e�N�X�`���̐���
HRESULT PmdModel::CreateTexture(PMDMODEL& model)
{
	HRESULT hr = S_OK;

	// �e�N�X�`���[�̐���
	// �e�N�X�`���[���̊m��
	model.mesh.texBuffer.resize(model.mesh.materialNum);
	model.mesh.sphTexBuffer.resize(model.mesh.materialNum);
	model.mesh.spaTexBuffer.resize(model.mesh.materialNum);
	for (UINT i = 0; i < model.mesh.materialNum; ++i)
	{
		std::string texFileName = model.mesh.materials[i].additional.texPath;
		if (std::count(texFileName.begin(), texFileName.end(), '*') > 0)
		{
			// �X�v���b�^������
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
