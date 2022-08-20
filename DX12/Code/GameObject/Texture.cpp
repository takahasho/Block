// --------------------- Include File --------------------
#include "Texture.h"
#include "../Camera/Camera.h"
#include "../DX12System.h"
Texture* Texture::_instance = nullptr;			// �C���X�^���X�̎���
// �R���X�g���N�^�[
Texture::Texture()
{

}
// �f�X�g���N�^�[
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
// ����
void Texture::Create()
{
	if (!_instance)
	{
		_instance = new Texture();

		// �e�N�X�`���[�̐���
		_instance->CreateTexture();
	}
}
// �j��
void Texture::Destroy()
{
	delete _instance;
}
// ����
HRESULT Texture::CreateTexture()
{
	HRESULT hr = S_OK;

	_dev = DX12::GetInstance()->GetDevice();
	_cmdList = DX12::GetInstance()->CommandList();
	_cmdAllocator = DX12::GetInstance()->CommandAllocator();

	// ���[�g�V�O�l�`���[�̐���
	if (FAILED(CreateRootSignature()))
		return E_FAIL;

	// �`��p�C�v���C���X�e�[�g�̐���
	if (FAILED(CreateGraphicsPipelineState()))
		return E_FAIL;

	return S_OK;
}
// ���[�g�V�O�l�`���[�̐���
HRESULT Texture::CreateRootSignature()
{
	HRESULT hr = S_OK;

	// �f�B�X�N���v�^�[�����W���쐬
	D3D12_DESCRIPTOR_RANGE descTblRange[2] = {};

	// �e�N�X�`���[�p
	// �f�B�X�N���v�^�[�̐�[�P��]
	descTblRange[0].NumDescriptors = 1;
	// �����W�̎��[SRV:�V�F�[�_�[���\�[�X�r���[]
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	// �擪���W�X�^�[�ԍ�[0��I��]
	descTblRange[0].BaseShaderRegister = 0;
	// �I�t�Z�b�g���w��[�A�������f�B�X�N���v�^�[�����W���O�̃f�B�X�N���v�^�����W�̒���ɂ���]
	descTblRange[0].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// �萔�o�b�t�@�[�p
	// �f�B�X�N���v�^�[�̐�[�P��]
	descTblRange[1].NumDescriptors = 1;
	// �����W�̎��[CBV:�R���X�^���g�o�b�t�@�[�r���[]
	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// �擪���W�X�^�[�ԍ�[0��I��]
	descTblRange[1].BaseShaderRegister = 0;
	// �I�t�Z�b�g���w��[�A�������f�B�X�N���v�^�[�����W���O�̃f�B�X�N���v�^�����W�̒���ɂ���]
	descTblRange[1].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ���[�g�p�����[�^�[���쐬
	D3D12_ROOT_PARAMETER rootParam = {};
	// ��ʂ̑I��[�f�B�X�N���v�^�[�e�[�u����I��]
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// �ǂ̃V�F�[�_�[���痘�p�\��[�S�V�F�[�_�[]
	rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// �f�B�X�N���v�^�[�����W�̃A�h���X
	rootParam.DescriptorTable.pDescriptorRanges =
		descTblRange;
	// �f�B�X�N���v�^�[�����W�̐�
	rootParam.DescriptorTable.NumDescriptorRanges = 2;

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

	rootSigBlob->Release();		// �s�v�ɂȂ����̂ŉ��

	return hr;
}
// �`��p�C�v���C���̐���
HRESULT Texture::CreateGraphicsPipelineState()
{
	HRESULT hr = S_OK;

	// �V�F�[�_�[�I�u�W�F�N�g�ǂݍ��ݗp
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// ���_�V�F�[�_�[�̓ǂݍ���
	hr = D3DCompileFromFile(
		L".\\Shader\\Texture\\TextureVertexShader.hlsl",			// �V�F�[�_�[��
		nullptr,							// define�͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// �C���N���[�h�̓f�t�H���g
		"TextureVS", "vs_5_0",				// �֐��� BasicVS�A�ΏۃV�F�[�_�[�� vs_5_0
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
		L".\\Shader\\Texture\\TexturePixelShader.hlsl",			// �V�F�[�_�[��
		nullptr,							// define�͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// �C���N���[�h�̓f�t�H���g
		"TexturePS", "ps_5_0",				// �֐��� BasicPS�A�ΏۃV�F�[�_�[�� ps_5_0
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
		{	// UV���
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
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

	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	for (UINT i = 0; i < _countof(gpipelineDesc.BlendState.RenderTarget); ++i)
	{
		// �u�����h�̗L��[����]
		renderTargetBlendDesc.BlendEnable = TRUE;
		// �_�����Z�̗L��[�Ȃ�]
		renderTargetBlendDesc.LogicOpEnable = FALSE;

		// �`�挳�W��[1��Z:�u�����f�B���O�W����(As, As, As, As)]
		renderTargetBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		// �`���W��[0��Z:�u�����f�B���O�W����(1-As, 1-As, 1-As, 1-As)]
		renderTargetBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		// �ǂ��u�����h����̂�[���Z]
		renderTargetBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;

		// �`�挳���W��[1��Z:�f�ނ̌��̐F]
		renderTargetBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		// �`��惿�W��[0��Z:���ł��[��]
		renderTargetBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		// �����ǂ��u�����h����̂�[���Z]
		renderTargetBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;

		// �ǂ��_�����Z����̂�[�`�挳���̂܂܂̒l]
		renderTargetBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;

		// RGBA���ꂼ��̒l���������ނ��w��[�S��]
		renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		// �����_�[�^�[�Q�b�g�u�����h��PSO�ɕR�Â���
		gpipelineDesc.BlendState.RenderTarget[i] = renderTargetBlendDesc;
	}
	

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

	// �O���t�B�b�N�p�C�v���C���X�e�[�g�I�u�W�F�N�g�𐶐�
	hr = _dev->CreateGraphicsPipelineState(
		&gpipelineDesc, IID_PPV_ARGS(_pipelineState.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	return S_OK;
}
// PLANE���_/�C���f�b�N�X�o�b�t�@�[�̐���
HRESULT Texture::CreateVertexIndexBufferView(MESH* mesh, float width, float height)
{
	HRESULT hr = S_OK;
	LONG x = g_window_width;
	LONG y = g_window_height;
	TextureVertex vertices[4] = {};
	vertices[0] = { {  -width / x ,   height / y , 0.0f}, {0.0f, 1.0f} };	// ����
	vertices[1] = { {  -width / x ,  -height / y , 0.0f}, {0.0f, 0.0f} };	// ����
	vertices[2] = { {   width / x ,   height / y , 0.0f}, {1.0f, 1.0f} };	// �E��
	vertices[3] = { {   width / x ,  -height / y , 0.0f}, {1.0f, 0.0f} };	// �E��		

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
	resDesc.Width = sizeof(TextureVertex) * PlaneNumVertices;
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
		IID_PPV_ARGS(mesh->vertexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// ���_���̃R�s�[
	TextureVertex* vertexMap = nullptr;
	// ���_�o�b�t�@�[�̃��b�N
	hr = mesh->vertexBuffer->Map(0, nullptr,
		(void**)&vertexMap);
	if (hr != S_OK)
		return hr;
	memcpy(vertexMap, vertices, sizeof(TextureVertex) * PlaneNumVertices);
	// ���_�o�b�t�@�[�̉���
	mesh->vertexBuffer->Unmap(0, nullptr);

	// ���_�o�b�t�@�[�r���[���쐬
	// ���_�o�b�t�@�[�̃A�h���X[�o�b�t�@�[�̉��z�A�h���X]
	mesh->vertexBufferView.BufferLocation = mesh->vertexBuffer->GetGPUVirtualAddress();
	// ���o�C�g��[�S�o�C�g��]
	mesh->vertexBufferView.SizeInBytes = sizeof(TextureVertex) * PlaneNumVertices;
	// 1���_������̃o�C�g��
	mesh->vertexBufferView.StrideInBytes = sizeof(TextureVertex);

	// �T�C�Y��[���őS���܂��Ȃ��̂�sizeof(�S�C���f�b�N�X)�Ƃ���]
	resDesc.Width = sizeof(PlaneIndices);

	// ���\�[�X�I�u�W�F�N�g���쐬
	hr = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(mesh->indexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �C���f�b�N�X���̃R�s�[
	unsigned short* indexMap = nullptr;
	// �C���f�b�N�X�o�b�t�@�[�̃��b�N
	hr = mesh->indexBuffer->Map(0, nullptr,
		(void**)&indexMap);
	if (hr != S_OK)
		return hr;
	std::copy(std::begin(PlaneIndices), std::end(PlaneIndices),
		indexMap);
	// �C���f�b�N�X�o�b�t�@�[�̉���
	mesh->indexBuffer->Unmap(0, nullptr);

	// �C���f�b�N�X�o�b�t�@�[�r���[���쐬
	// �C���f�b�N�X�o�b�t�@�[�̃A�h���X[�o�b�t�@�[�̉��z�A�h���X]
	mesh->indexBufferView.BufferLocation = mesh->indexBuffer->GetGPUVirtualAddress();
	// ���o�C�g��[�S�o�C�g��]
	mesh->indexBufferView.SizeInBytes = sizeof(PlaneIndices);
	// �t�H�[�}�b�g
	mesh->indexBufferView.Format = DXGI_FORMAT_R16_UINT;

	return S_OK;
}
// �R���X�^���g�o�b�t�@�[�r���[�̐���
HRESULT Texture::CreateConstantBufferView(MESH* mesh)
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
	resDesc.Width = (sizeof(MetaData) + 0xff) & ~0xff;
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
	// �f�X�N���v�^���w��[SRV�P�� + CBV�P��]
	heapDesc.NumDescriptors = 2;
	// ���Ɏw��Ȃ�[�V�F�[�_�[�����猩����K�v����]
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// �f�X�N���v�^�q�[�v���쐬
	hr = _dev->CreateDescriptorHeap(&heapDesc
		, IID_PPV_ARGS(mesh->basicDescHeap.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �f�B�X�N���v�^�̐擪�n���h�����擾���Ă���
	auto basicHeapHandle = mesh->basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	// �V�F�[�_�[���\�[�X�r���[���쐬
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	// �t�H�[�}�b�g���w��[R8G8B8_UINT:RGBA(0.0f�`1.0f�ɐ��K��)]
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// �R���|�[�l���g�}�b�s���O���w��[�f�B�t�H���g��I��]
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	// ���p�̂̂ǂ̃����o�[�Ƃ��Ĉ�����[�e�N�X�`���[2D��I��]
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	// �~�b�v�}�b�v�̐����w��[�g�p���Ȃ��̂�1]
	srvDesc.Texture2D.MipLevels = 1;
	_dev->CreateShaderResourceView(
		mesh->textureBuffer[0].Get(),	// �r���[�Ɗ֘A�t����o�b�t�@�[���w��
		&srvDesc,				// �e�N�X�`���[�I������w��
		basicHeapHandle);		// �q�[�v�̂ǂ��Ɋ��蓖�Ă邩

	// ���̏ꏊ�Ɉړ�
	basicHeapHandle.ptr +=
		_dev->GetDescriptorHandleIncrementSize
		(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = mesh->constBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)mesh->constBuffer->GetDesc().Width;

	// �萔�o�b�t�@�[�r���[�̍쐬
	_dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	return S_OK;
}
// �V�F�[�_�[���\�[�X�̐���
HRESULT Texture::CreateShaderResource(const char* pFileName, ComPtr<ID3D12Resource>& textureBuffer, SIZE& size)
{

	HRESULT hr = S_OK;
	// �摜�T�C�Y�̎擾�ƃC���[�W�f�[�^�[�m��
	BYTE* pImageData = nullptr;
	size = {};

	// �摜���̓ǂݍ���
	if (FAILED(LoadTextureFromFile(pFileName,
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

	// ���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC resDesc = {};
	// ���p���@[2D�e�N�X�`���[]
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	// �T�C�Y��
	resDesc.Width = (UINT64)size.cx;
	// �T�C�Y����
	resDesc.Height = (UINT64)size.cy;
	// �[�x��z��T�C�Y[2D�Ŕz��ł��Ȃ��̂�1]
	resDesc.DepthOrArraySize = 1;
	// �~�b�v�}�b�v���x��[�~�b�v�}�b�v���Ȃ��̂Ń~�b�v����1��]
	resDesc.MipLevels = 1;
	// �t�H�[�}�b�g[RGBA�t�H�[�}�b�g]
	resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// �A���`�G�C���A�V���O�̃p�����[�^�[[�A���`�G�C���A�V���O���s��Ȃ�����SampleDesc.Count = 1]
	resDesc.SampleDesc.Count = 1;
	// �t���O[NONE�ł悢]
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	// ���C�A�E�g[���C�A�E�g�͌��肵�Ȃ�]
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	// �i��[�N�I���e�B�[�͍Œ�]
	resDesc.SampleDesc.Quality = 1;

	// ���\�[�X�I�u�W�F�N�g���쐬
	hr = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,	// �e�N�X�`���[�p�w��
		nullptr,
		IID_PPV_ARGS(textureBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// 1�s�N�Z��������̃o�C�g��
	UINT bytePerPixel = 4;

	// �f�[�^�]��
	hr = textureBuffer->WriteToSubresource(
		0,			// �T�u���\�[�X�C���f�b�N�X[0���w��]
		nullptr,	// �������ݗ̈���w��[nullptr�͑S��]
		pImageData,	// �������݂����f�[�^�̃A�h���X
		(UINT)(bytePerPixel * size.cx),// �P���C���T�C�Y
		(UINT)(size.cy));// �P���C�������̃T�C�Y
	if (FAILED(hr))
		return hr;

	// �C���[�W���̉��
	delete[] pImageData;
	return S_OK;
}
// �`��
void Texture::Draw()
{
	// �O���t�B�b�N�X�p�C�v���C���X�e�[�g�̎w��
	_cmdList->SetPipelineState(_pipelineState.Get());
	// ���[�g�V�O�l�`���[�̎w��
	_cmdList->SetGraphicsRootSignature(_rootSignature.Get());

	// ���_���̎w��[�O�p�`]
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
						MetaData* mapMatrix = nullptr;	// �}�b�v��������|�C���^�[
						_object[j].mesh->constBuffer->Map(0, nullptr, (void**)&mapMatrix);	// �}�b�v
						XMMATRIX matrix = XMMatrixIdentity();

						// =====���[���h�s��=====
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

						// =====�r���[�s��=====
						XMMATRIX viewMat = Camera::GetInstance()->GetView();

						// =====�v���W�F�N�V�����s��=====
						XMMATRIX projMat = Camera::GetInstance()->GetProjection();

						matrix = world_matrix * viewMat * projMat;

						mapMatrix->world = matrix;
						XMFLOAT4 color(_object[j].info.color.x, _object[j].info.color.y, _object[j].info.color.z, _object[j].info.alpha);
						mapMatrix->color = color;

						_object[j].mesh->constBuffer->Unmap(0, nullptr);		// �A���}�b�v

						// �f�B�X�N���v�^�[�q�[�v�̎w��
						_cmdList->SetDescriptorHeaps(1, _object[j].mesh->basicDescHeap.GetAddressOf());

						// ���[�g�p�����[�^�[�ƃf�B�X�N���v�^�[�q�[�v�̊֘A�t��
						_cmdList->SetGraphicsRootDescriptorTable(
							0,	// ���[�g�p�����[�^�[�C���f�b�N�X
							_object[j].mesh->basicDescHeap->GetGPUDescriptorHandleForHeapStart());


						// ���_�o�b�t�@�[�̎w��
						_cmdList->IASetVertexBuffers(0, 1, &_object[j].mesh->vertexBufferView);

						// �C���f�b�N�X�o�b�t�@�[�̎w��
						_cmdList->IASetIndexBuffer(&_object[j].mesh->indexBufferView);

						// �`�施��
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
						MetaData* mapMatrix = nullptr;	// �}�b�v��������|�C���^�[
						_object[j].mesh->constBuffer->Map(0, nullptr, (void**)&mapMatrix);	// �}�b�v
						XMMATRIX matrix = XMMatrixIdentity();

						// �摜������񂹂��邽�߂̕ϐ�
						LONG x = g_window_width / 2;
						LONG y = g_window_height / 2;
						FLOAT ratioX = (FLOAT)g_window_width / (FLOAT)g_window_height;
						FLOAT ratioY = (FLOAT)g_window_height / (FLOAT)g_window_width;		
						XMMATRIX translate = XMMatrixTranslation(_object[j].info.position.x / x - 1, -(_object[j].info.position.y / y - 1), 0);
						XMMATRIX rotationMat = XMMatrixRotationZ(XMConvertToRadians(_object[j].info.angle.z));
						// �����L�΂���Ă���̂Ŕ䗦���|����
						rotationMat.r[0].m128_f32[1] *= ratioX;
						rotationMat.r[1].m128_f32[0] *= ratioY;
						XMMATRIX scale_mat = XMMatrixScaling(_object[j].info.scale.x, -_object[j].info.scale.y, 1);

						matrix = scale_mat * rotationMat * translate;

						mapMatrix->world = matrix;
						XMFLOAT4 color(_object[j].info.color.x, _object[j].info.color.y, _object[j].info.color.z, _object[j].info.alpha);
						mapMatrix->color = color;

						_object[j].mesh->constBuffer->Unmap(0, nullptr);		// �A���}�b�v

						// �f�B�X�N���v�^�[�q�[�v�̎w��
						_cmdList->SetDescriptorHeaps(1, _object[j].mesh->basicDescHeap.GetAddressOf());

						// ���[�g�p�����[�^�[�ƃf�B�X�N���v�^�[�q�[�v�̊֘A�t��
						_cmdList->SetGraphicsRootDescriptorTable(
							0,	// ���[�g�p�����[�^�[�C���f�b�N�X
							_object[j].mesh->basicDescHeap->GetGPUDescriptorHandleForHeapStart());


						// ���_�o�b�t�@�[�̎w��
						_cmdList->IASetVertexBuffers(0, 1, &_object[j].mesh->vertexBufferView);

						// �C���f�b�N�X�o�b�t�@�[�̎w��
						_cmdList->IASetIndexBuffer(&_object[j].mesh->indexBufferView);

						// �`�施��
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
	// ���߂ăG���g���[�����Ȃ�Mesh���������łȂ����Info���������̂�
	if (entryPos == -1)
	{
		if (!_mesh[filename])
		{
			SIZE size = {};
			_mesh[filename] = new MESH;
			// �V�F�[�_�[���\�[�X�̐���
			CreateShaderResource(filename, _mesh[filename]->textureBuffer[0], size);

			float width = (float)size.cx;
			float height = (float)size.cy;

			_size[filename].width = width;
			_size[filename].height = height;

			// �C���f�b�N�X�o�[�e�b�N�X�o�b�t�@�[�r���[�̐���
			CreateVertexIndexBufferView(_mesh[filename], width, height);
		}

		bool flag = false;
		// �󂢂Ă�G���g���[�ԍ���T��		
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

			// �R���X�^���g�o�b�t�@�[�r���[�̐���
			CreateConstantBufferView(_object.back().mesh);

			// ������������
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

			// �R���X�^���g�o�b�t�@�[�r���[�̐���
			CreateConstantBufferView(_object[entryPos].mesh);

			_drawflag[entryPos] = true;
			// ������������
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
		// ������������
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
