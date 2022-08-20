// --------------------- Include File --------------------
#include "Mesh.h"
#include "GeometryData.h"
#include "../Camera/Camera.h"
#include "../DX12System.h"
#include "../VECTOR.h"
Mesh* Mesh::_instance = nullptr;			// �C���X�^���X�̎���
// �R���X�g���N�^�[
Mesh::Mesh()
{
	boxVertexBuffer = nullptr;			// ���_�o�b�t�@�[
	boxIndexBuffer = nullptr;				// �C���f�b�N�X�o�b�t�@�[

	boxVertexBufferView = {};		// ���_�o�b�t�@�[�r���[
	boxIndexBufferView = {};	    // �C���f�b�N�X�o�b�t�@�[�r���[

	cylinderVertexBuffer = nullptr;			// ���_�o�b�t�@�[
	cylinderIndexBuffer = nullptr;				// �C���f�b�N�X�o�b�t�@�[

	cylinderVertexBufferView = {};		// ���_�o�b�t�@�[�r���[
	cylinderIndexBufferView = {};	    // �C���f�b�N�X�o�b�t�@�[�r���[

	sphereVertexBuffer = nullptr;			// ���_�o�b�t�@�[
	sphereIndexBuffer = nullptr;				// �C���f�b�N�X�o�b�t�@�[

	sphereVertexBufferView = {};		// ���_�o�b�t�@�[�r���[
	sphereIndexBufferView = {};	    // �C���f�b�N�X�o�b�t�@�[�r���[


	circleVertexBuffer = nullptr;			// ���_�o�b�t�@�[
	circleIndexBuffer = nullptr;				// �C���f�b�N�X�o�b�t�@�[

	circleVertexBufferView = {};		// ���_�o�b�t�@�[�r���[
	circleIndexBufferView = {};	    // �C���f�b�N�X�o�b�t�@�[�r���[
}
// �f�X�g���N�^�[
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
// ����
void Mesh::Create()
{
	if (!_instance)
	{
		_instance = new Mesh();

		// ���[�g�V�O�l�`���[�̐���
		_instance->CreateRootSignature();

		// �`��p�C�v���C���̐���
		_instance->CreateGraphicsPipelineState();

		// ���_/�C���f�N�X�o�b�t�@�[�r���[�̐���
		_instance->CreateVertexIndexBufferView();

	}
}
// �j��
void Mesh::Destroy()
{
	delete _instance;
}
// ���[�g�V�O�l�`���[�̐���
HRESULT Mesh::CreateRootSignature()
{
	HRESULT hr = S_OK;

	// �f�B�X�N���v�^�[�����W�̐ݒ�
	D3D12_DESCRIPTOR_RANGE descTblRange[2] = {};
	// �f�B�X�N���v�^�[�̐�[�R���X�^���g�o�b�t�@�[]
	descTblRange[0].NumDescriptors = 1;
	// ���[�萔]
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// �擪���W�X�^�[�ԍ�[0�ԃX���b�g����]
	descTblRange[0].BaseShaderRegister = 0;
	// �A�������f�B�X�N���v�^�[�����W���O�̃f�B�X�N���v�^�[�����W�̒���ɗ���
	descTblRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// �f�B�X�N���v�^�[�̐�[�R���X�^���g�o�b�t�@�[]
	descTblRange[1].NumDescriptors = 1;
	// ���[�萔]
	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// �擪���W�X�^�[�ԍ�[1�ԃX���b�g����]
	descTblRange[1].BaseShaderRegister = 1;
	// �A�������f�B�X�N���v�^�[�����W���O�̃f�B�X�N���v�^�[�����W�̒���ɗ���
	descTblRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ���[�g�p�����[�^�[�̐ݒ�
	D3D12_ROOT_PARAMETER rootParam[2] = {};
	// ���[�g�p�����[�^�[�̎��[����̓f�B�X�N���v�^�[�e�[�u���Ƃ��ė��p]
	rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// �ǂ̃V�F�[�_�[���痘�p�\��[�S�V�F�[�_�[]
	rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// �f�B�X�N���v�^�[�����W�̃A�h���X
	rootParam[0].DescriptorTable.pDescriptorRanges = &descTblRange[0];
	// �f�B�X�N���v�^�[�����W�̐�
	rootParam[0].DescriptorTable.NumDescriptorRanges = 1;
	// ���[�g�p�����[�^�[�̎��[����̓f�B�X�N���v�^�[�e�[�u���Ƃ��ė��p]
	rootParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// �ǂ̃V�F�[�_�[���痘�p�\��[�S�V�F�[�_�[]
	rootParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// �f�B�X�N���v�^�[�����W�̃A�h���X
	rootParam[1].DescriptorTable.pDescriptorRanges = &descTblRange[1];
	// �f�B�X�N���v�^�[�����W�̐�
	rootParam[1].DescriptorTable.NumDescriptorRanges = 1;

	// �T���v���[�̐ݒ�
	D3D12_STATIC_SAMPLER_DESC samplerDesc[2] = {};
	// �������̌J��Ԃ�
	samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// �c�����̌J��Ԃ�
	samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// ���s���̌J��Ԃ�
	samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// �{�[�_�[�͍�
	samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	// ���`���
	samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	// �~�b�v�}�b�v�ő�l
	samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
	// �~�b�v�}�b�v�ŏ��l
	samplerDesc[0].MinLOD = 0.0f;
	// �s�N�Z���V�F�[�_�[���猩����
	samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	// �V�F�[�_�[�X���b�g�ԍ�
	samplerDesc[0].ShaderRegister = 0;
	// ���T���v�����O���Ȃ�
	samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	// �ύX�_�ȊO���R�s�[
	samplerDesc[1] = samplerDesc[0];
	// �������̌J��Ԃ�
	samplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	// �c�����̌J��Ԃ�
	samplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	// ���s���̌J��Ԃ�
	samplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	// �V�F�[�_�[�X���b�g�ԍ�
	samplerDesc[1].ShaderRegister = 1;

	// ���[�g�V�O�l�`���[�̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	// �t���O[���_���(���̓A�Z���u��)������]
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	// �쐬���郋�[�g�p�����[�^�[�z��̐擪�A�h���X
	rootSignatureDesc.pParameters = rootParam;
	// ���[�g�p�����[�^�[�̐�
	rootSignatureDesc.NumParameters = 2;
	// �쐬���郋�[�g�T���v���[�z��̐擪�A�h���X
	rootSignatureDesc.pStaticSamplers = samplerDesc;
	// ���[�g�T���v���[�̐�
	rootSignatureDesc.NumStaticSamplers = 2;

	ID3DBlob* rootSigBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// �o�C�i���R�[�h���쐬
	hr = D3D12SerializeRootSignature(
		&rootSignatureDesc,				// ���[�g�V�O�l�`���[�ݒ�
		D3D_ROOT_SIGNATURE_VERSION_1_0,	// ���[�g�V�O�l�`���[�o�[�W����
		&rootSigBlob,					// �󂯎�邽�߂̃|�C���^�[�̃A�h���X
		&errorBlob);					// �G���[�p�|�C���^�[�̃A�h���X[�G���[����errorBlob�Ƀ��b�Z�[�W������]
	if (FAILED(hr))
		return hr;

	// ���[�g�V�O�l�`���[�I�u�W�F�N�g�̍쐬
	hr = DX12::GetInstance()->GetDevice()->CreateRootSignature(
		0,	// nodemask�B�O�ł悢
		rootSigBlob->GetBufferPointer(),	// �A�h���X
		rootSigBlob->GetBufferSize(),		// �T�C�Y
		IID_PPV_ARGS(_rootSignature.ReleaseAndGetAddressOf()));		// �A�h���X��[��
	if (FAILED(hr))
		return hr;

	rootSigBlob->Release();		// �s�v�ɂȂ����̂ŉ��

	return S_OK;
}

// �`��p�C�v���C���̐���
HRESULT Mesh::CreateGraphicsPipelineState()
{
	HRESULT hr = S_OK;

	// �V�F�[�_�[�I�u�W�F�N�g�ǂݍ��ݗp
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// ���_�V�F�[�_�[�̓ǂݍ���
	hr = D3DCompileFromFile(
		L".\\Shader\\Mesh\\MeshVertexShader.hlsl",			// �V�F�[�_�[��
		nullptr,							// define�͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// �C���N���[�h�̓f�t�H���g
		"MeshVS", "vs_5_0",				// �֐��� BasicVS�A�ΏۃV�F�[�_�[�� vs_5_0
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
		L".\\Shader\\Mesh\\MeshPixelShader.hlsl",			// �V�F�[�_�[��
		nullptr,							// define�͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// �C���N���[�h�̓f�t�H���g
		"MeshPS", "ps_5_0",				// �֐��� BasicPS�A�ΏۃV�F�[�_�[�� ps_5_0
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
		{	// COLOR���
			"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
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

	// �[�x�o�b�t�@�[�̗L���̗L��[�L��]
	gpipelineDesc.DepthStencilState.DepthEnable = true;
	// �[�x�X�e���V���o�b�t�@�[�̏������ݕ��@[�������ݗL��]
	gpipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// �[�x�X�e���V���o�b�t�@�[�̐�����@[�������ق����̗p]
	gpipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
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
HRESULT Mesh::CreateVertexIndexBufferView()
{
	HRESULT hr = S_OK;

	// ====================================== �� =========================================

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
	resDesc.Width = sizeof(MESHVERTEX) * BoxNumIndices;

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
		IID_PPV_ARGS(boxVertexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;
	UINT polygonNum = 3;		// �ʌ`���̃|���S����
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

	// ���_���̃R�s�[
	MESHVERTEX* vertexMap = nullptr;
	// ���_�o�b�t�@�[�̃��b�N
	hr = boxVertexBuffer->Map(0, nullptr,
		(void**)&vertexMap);
	if (hr != S_OK)
		return hr;

	memcpy(vertexMap, boxVertices, sizeof(MESHVERTEX) * BoxNumVertices);

	// ���_�o�b�t�@�[�̉���
	boxVertexBuffer->Unmap(0, nullptr);

	// ���_�o�b�t�@�[�r���[���쐬
	// ���_�o�b�t�@�[�̃A�h���X[�o�b�t�@�[�̉��z�A�h���X]
	boxVertexBufferView.BufferLocation = boxVertexBuffer->GetGPUVirtualAddress();
	// ���o�C�g��[�S�o�C�g��]
	boxVertexBufferView.SizeInBytes = sizeof(MESHVERTEX) * BoxNumVertices;

	// 1���_������̃o�C�g��
	boxVertexBufferView.StrideInBytes = sizeof(MESHVERTEX);

	// �T�C�Y��[���őS���܂��Ȃ��̂�sizeof(�S�C���f�b�N�X)�Ƃ���]
	resDesc.Width = sizeof(BoxIndices);


	// ���\�[�X�I�u�W�F�N�g���쐬
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(boxIndexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �C���f�b�N�X���̃R�s�[
	unsigned short* indexMap = nullptr;
	// �C���f�b�N�X�o�b�t�@�[�̃��b�N
	hr = boxIndexBuffer->Map(0, nullptr,
		(void**)&indexMap);
	if (hr != S_OK)
		return hr;
	std::copy(std::begin(BoxIndices), std::end(BoxIndices), indexMap);

	// �C���f�b�N�X�o�b�t�@�[�̉���
	boxIndexBuffer->Unmap(0, nullptr);

	// �C���f�b�N�X�o�b�t�@�[�r���[���쐬
	// �C���f�b�N�X�o�b�t�@�[�̃A�h���X[�o�b�t�@�[�̉��z�A�h���X]
	boxIndexBufferView.BufferLocation = boxIndexBuffer->GetGPUVirtualAddress();
	// ���o�C�g��[�S�o�C�g��]
	boxIndexBufferView.SizeInBytes = sizeof(BoxIndices);
	// �t�H�[�}�b�g
	boxIndexBufferView.Format = DXGI_FORMAT_R16_UINT;

	// ====================================== �~�� =========================================

	// �T�C�Y��[���őS���܂��Ȃ��̂�sizeof(�S���_)�Ƃ���]
	resDesc.Width = sizeof(MESHVERTEX) * CylinderNumIndices;


	// ���\�[�X�I�u�W�F�N�g���쐬
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(cylinderVertexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// ���_�o�b�t�@����
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

	// ���_���̃R�s�[
	vertexMap = nullptr;
	// ���_�o�b�t�@�[�̃��b�N
	hr = cylinderVertexBuffer->Map(0, nullptr,
		(void**)&vertexMap);
	if (hr != S_OK)
		return hr;

	memcpy(vertexMap, cylinderVertices, sizeof(MESHVERTEX) * CylinderNumVertices);

	// ���_�o�b�t�@�[�̉���
	cylinderVertexBuffer->Unmap(0, nullptr);

	// ���_�o�b�t�@�[�r���[���쐬
	// ���_�o�b�t�@�[�̃A�h���X[�o�b�t�@�[�̉��z�A�h���X]
	cylinderVertexBufferView.BufferLocation = cylinderVertexBuffer->GetGPUVirtualAddress();
	// ���o�C�g��[�S�o�C�g��]
	cylinderVertexBufferView.SizeInBytes = sizeof(MESHVERTEX) * CylinderNumVertices;

	// 1���_������̃o�C�g��
	cylinderVertexBufferView.StrideInBytes = sizeof(MESHVERTEX);

	// �T�C�Y��[���őS���܂��Ȃ��̂�sizeof(�S�C���f�b�N�X)�Ƃ���]
	resDesc.Width = sizeof(CylinderIndices);



	// ���\�[�X�I�u�W�F�N�g���쐬
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(cylinderIndexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �C���f�b�N�X���̃R�s�[
	indexMap = nullptr;
	// �C���f�b�N�X�o�b�t�@�[�̃��b�N
	hr = cylinderIndexBuffer->Map(0, nullptr,
		(void**)&indexMap);
	if (hr != S_OK)
		return hr;

	std::copy(std::begin(CylinderIndices), std::end(CylinderIndices), indexMap);

	// �C���f�b�N�X�o�b�t�@�[�̉���
	cylinderIndexBuffer->Unmap(0, nullptr);

	// �C���f�b�N�X�o�b�t�@�[�r���[���쐬
	// �C���f�b�N�X�o�b�t�@�[�̃A�h���X[�o�b�t�@�[�̉��z�A�h���X]
	cylinderIndexBufferView.BufferLocation = cylinderIndexBuffer->GetGPUVirtualAddress();
	// ���o�C�g��[�S�o�C�g��]
	cylinderIndexBufferView.SizeInBytes = sizeof(CylinderIndices);
	// �t�H�[�}�b�g
	cylinderIndexBufferView.Format = DXGI_FORMAT_R16_UINT;


	// ====================================== �� =========================================
	
	// �T�C�Y��[���őS���܂��Ȃ��̂�sizeof(�S���_)�Ƃ���]
	resDesc.Width = sizeof(MESHVERTEX) * SphereNumVertices;

	// ���\�[�X�I�u�W�F�N�g���쐬
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(sphereVertexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// ���_�o�b�t�@����
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
	// ���_���̃R�s�[
	vertexMap = nullptr;
	// ���_�o�b�t�@�[�̃��b�N
	hr = sphereVertexBuffer->Map(0, nullptr,
		(void**)&vertexMap);
	if (hr != S_OK)
		return hr;

	memcpy(vertexMap, sphereVertices, sizeof(MESHVERTEX) * SphereNumVertices);

	// ���_�o�b�t�@�[�̉���
	sphereVertexBuffer->Unmap(0, nullptr);

	// ���_�o�b�t�@�[�r���[���쐬
	// ���_�o�b�t�@�[�̃A�h���X[�o�b�t�@�[�̉��z�A�h���X]
	sphereVertexBufferView.BufferLocation = sphereVertexBuffer->GetGPUVirtualAddress();
	// ���o�C�g��[�S�o�C�g��]
	sphereVertexBufferView.SizeInBytes = sizeof(MESHVERTEX) * SphereNumVertices;

	// 1���_������̃o�C�g��
	sphereVertexBufferView.StrideInBytes = sizeof(MESHVERTEX);

	// �T�C�Y��[���őS���܂��Ȃ��̂�sizeof(�S�C���f�b�N�X)�Ƃ���]
	resDesc.Width = sizeof(SphereIndices);



	// ���\�[�X�I�u�W�F�N�g���쐬
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(sphereIndexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �C���f�b�N�X���̃R�s�[
	indexMap = nullptr;
	// �C���f�b�N�X�o�b�t�@�[�̃��b�N
	hr = sphereIndexBuffer->Map(0, nullptr,
		(void**)&indexMap);
	if (hr != S_OK)
		return hr;

	std::copy(std::begin(SphereIndices), std::end(SphereIndices), indexMap);


	// �C���f�b�N�X�o�b�t�@�[�̉���
	sphereIndexBuffer->Unmap(0, nullptr);

	// �C���f�b�N�X�o�b�t�@�[�r���[���쐬
	// �C���f�b�N�X�o�b�t�@�[�̃A�h���X[�o�b�t�@�[�̉��z�A�h���X]
	sphereIndexBufferView.BufferLocation = sphereIndexBuffer->GetGPUVirtualAddress();
	// ���o�C�g��[�S�o�C�g��]
	sphereIndexBufferView.SizeInBytes = sizeof(SphereIndices);
	// �t�H�[�}�b�g
	sphereIndexBufferView.Format = DXGI_FORMAT_R16_UINT;

	// ====================================== �~ =========================================

	// ���_�o�b�t�@����
	const unsigned int  VerticesNum = 36 + 1;	// 1�͒��S�_
	MESHVERTEX circleVertices[VerticesNum] = {};
	for (int i = 0; i < VerticesNum; ++i)
	{
		if (i == (VerticesNum - 1))
		{
			// ���S�_
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

	// �T�C�Y��[���őS���܂��Ȃ��̂�sizeof(�S���_)�Ƃ���]
	resDesc.Width = sizeof(MESHVERTEX) * VerticesNum;

	// ���\�[�X�I�u�W�F�N�g���쐬
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(circleVertexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// ���_���̃R�s�[
	vertexMap = nullptr;
	// ���_�o�b�t�@�[�̃��b�N
	hr = circleVertexBuffer->Map(0, nullptr,
		(void**)&vertexMap);
	if (hr != S_OK)
		return hr;

	memcpy(vertexMap, circleVertices, sizeof(MESHVERTEX) * VerticesNum);

	// ���_�o�b�t�@�[�̉���
	circleVertexBuffer->Unmap(0, nullptr);

	// ���_�o�b�t�@�[�r���[���쐬
	// ���_�o�b�t�@�[�̃A�h���X[�o�b�t�@�[�̉��z�A�h���X]
	circleVertexBufferView.BufferLocation = circleVertexBuffer->GetGPUVirtualAddress();
	// ���o�C�g��[�S�o�C�g��]
	circleVertexBufferView.SizeInBytes = sizeof(MESHVERTEX) * VerticesNum;

	// 1���_������̃o�C�g��
	circleVertexBufferView.StrideInBytes = sizeof(MESHVERTEX);

	_circleIndicesNum = (VerticesNum - 1) * polygonNum;
	std::vector<unsigned short> circleIndices(_circleIndicesNum);
	int loopNum = 0;	// ���[�v��
	for (UINT i = 0; i < _circleIndicesNum; i += polygonNum, ++loopNum)
	{
		int num = i + 0;
		circleIndices[num] = loopNum + 0;
		num = i + 1;
		if ((loopNum) == VerticesNum - 2)
		{
			// �Ō�̃��[�v�ł͍ŏ��̈ʒu�ɖ߂�̂�0
			circleIndices[num] = 0;
		}
		else
		{
			circleIndices[num] = loopNum + 1;
		}
		num = i + 2;
		circleIndices[num] = VerticesNum - 1;
	}

	// �T�C�Y��[���őS���܂��Ȃ��̂�sizeof(�S�C���f�b�N�X)�Ƃ���]
	resDesc.Width = sizeof(unsigned short) * circleIndices.size();

	// ���\�[�X�I�u�W�F�N�g���쐬
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(circleIndexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �C���f�b�N�X���̃R�s�[
	indexMap = nullptr;
	// �C���f�b�N�X�o�b�t�@�[�̃��b�N
	hr = circleIndexBuffer->Map(0, nullptr,
		(void**)&indexMap);
	if (hr != S_OK)
		return hr;

	std::copy(std::begin(circleIndices), std::end(circleIndices), indexMap);


	// �C���f�b�N�X�o�b�t�@�[�̉���
	circleIndexBuffer->Unmap(0, nullptr);

	// �C���f�b�N�X�o�b�t�@�[�r���[���쐬
	// �C���f�b�N�X�o�b�t�@�[�̃A�h���X[�o�b�t�@�[�̉��z�A�h���X]
	circleIndexBufferView.BufferLocation = circleIndexBuffer->GetGPUVirtualAddress();
	// ���o�C�g��[�S�o�C�g��]
	circleIndexBufferView.SizeInBytes = sizeof(unsigned short) * (unsigned short)circleIndices.size();
	// �t�H�[�}�b�g
	circleIndexBufferView.Format = DXGI_FORMAT_R16_UINT;

	return S_OK;
}
// �}�e���A���o�b�t�@�[�̐���
HRESULT Mesh::CreateMaterialBuffer(MESH* mesh)
{
	HRESULT hr = S_OK;

	// �}�e���A���p�o�b�t�@�[���쐬
	auto materialBufferSize = sizeof(MetaData);
	materialBufferSize = (materialBufferSize + 0xff) & ~0xff;

	// ���ԃo�b�t�@�[�Ƃ��ẴA�b�v���[�h�q�[�v�̐ݒ�
	D3D12_HEAP_PROPERTIES uploadHeapProp = {};
	// �}�b�v�\�ɂ��邽�߁AUPLOAD�ɂ���
	uploadHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// �A�b�v���[�h�p�Ɏg�p���邱�ƑO��Ȃ̂�UNKNOWN�ł悢
	uploadHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	uploadHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	uploadHeapProp.CreationNodeMask = 0;	// �P��A�_�v�^�[�̂���0
	uploadHeapProp.VisibleNodeMask = 0;	// �P��A�_�v�^�[�̂���0

	// ���\�[�X��ݒ�
	D3D12_RESOURCE_DESC resDesc = {};
	// �t�H�[�}�b�g[�P�Ȃ�f�[�^�̉�Ȃ̂�UNKNOWN]
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	// ���p���@[�P�Ȃ�o�b�t�@�[�Ƃ��Ďw��]
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	// �T�C�Y��[�f�[�^�T�C�Y]
	resDesc.Width = materialBufferSize;
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
		&uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,	// ���Ɏw��Ȃ�
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,	// CPU����̏������݉\�AGPU���猩��Ɠǂݎ��̂�
		nullptr,
		IID_PPV_ARGS(mesh->materialBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	return hr;
}
// �R���X�^���g�o�b�t�@�[�r���[�̐���
HRESULT Mesh::CreateConstantBufferView(MESH* mesh)
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
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
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
	// �f�X�N���v�^���w��[CBV1�� + �}�e���A���� * 1]
	heapDesc.NumDescriptors = 2;
	// ���Ɏw��Ȃ�[�V�F�[�_�[�����猩����K�v����]
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// �f�X�N���v�^�q�[�v���쐬
	hr = DX12::GetInstance()->GetDevice()->CreateDescriptorHeap(&heapDesc
		, IID_PPV_ARGS(mesh->basicDescHeap.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �f�B�X�N���v�^�̐擪�n���h�����擾���Ă���
	auto basicHeapHandle = mesh->basicDescHeap->GetCPUDescriptorHandleForHeapStart();


	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = mesh->constBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)mesh->constBuffer->GetDesc().Width;

	// �萔�o�b�t�@�[�r���[�̍쐬
	DX12::GetInstance()->GetDevice()->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	// ���̏ꏊ�Ɉړ�
	basicHeapHandle.ptr +=
		DX12::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize
		(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// �}�e���A���p�o�b�t�@�[���쐬
	auto materialBufferSize = sizeof(MetaData);
	materialBufferSize = (materialBufferSize + 0xff) & ~0xff;

	D3D12_CONSTANT_BUFFER_VIEW_DESC matCBVDesc = {};
	matCBVDesc.BufferLocation = mesh->materialBuffer->GetGPUVirtualAddress();
	matCBVDesc.SizeInBytes = (UINT)materialBufferSize;

	// �}�e���A����(CBV)
	DX12::GetInstance()->GetDevice()->CreateConstantBufferView(&matCBVDesc
		, basicHeapHandle);

	return S_OK;
}
// �`��(��)
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

		MetaData* mapMetaData = nullptr;	// �}�b�v��������|�C���^�[
		_boxMeshs[m]->materialBuffer->Map(0, nullptr, (void**)&mapMetaData);	// �}�b�v
		*mapMetaData = _boxMetaData[m];
		_boxMeshs[m]->materialBuffer->Unmap(0, nullptr);		// �A���}�b�v

		// =====�r���[�s��=====
		XMMATRIX viewMat = Camera::GetInstance()->GetView();
		// =====�v���W�F�N�V�����s��=====
		XMMATRIX projMat = Camera::GetInstance()->GetProjection();
		// ===== ���C�g�̌��� =====
		XMFLOAT4 lightVector = Camera::GetInstance()->GetLightVector();
		// ===== ���C�g�̐F =====
		XMFLOAT4 lightColor = Camera::GetInstance()->GetLightColor();
		// ===== �J�������W =====
		XMFLOAT3 eye = Camera::GetInstance()->GetEyePosition();

		// �萔���̍X�V
		ConstantBuffer* constBuffer = nullptr;	// �}�b�v��������|�C���^�[
		_boxMeshs[m]->constBuffer->Map(0, nullptr, (void**)&constBuffer);	// �}�b�v
		constBuffer->view = viewMat;
		constBuffer->projection = projMat;
		constBuffer->eye = eye;
		constBuffer->lightVector = lightVector;
		constBuffer->lightColor = lightColor;
		_boxMeshs[m]->constBuffer->Unmap(0, nullptr);		// �A���}�b�v

		// �O���t�B�b�N�X�p�C�v���C���X�e�[�g�̎w��
		DX12::GetInstance()->CommandList()->SetPipelineState(_pipelineState.Get());
		// ���[�g�V�O�l�`���[�̎w��
		DX12::GetInstance()->CommandList()->SetGraphicsRootSignature(_rootSignature.Get());

		// �f�B�X�N���v�^�[�q�[�v�̎w��
		DX12::GetInstance()->CommandList()->SetDescriptorHeaps(1, _boxMeshs[m]->basicDescHeap.GetAddressOf());

		// ���[�g�p�����[�^�[�ƃf�B�X�N���v�^�[�q�[�v�̊֘A�t��
		DX12::GetInstance()->CommandList()->SetGraphicsRootDescriptorTable(
			0,	// ���[�g�p�����[�^�[�C���f�b�N�X
			_boxMeshs[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart());    // �q�[�v�A�h���X

		// ���_���̎w��[�O�p�`]
		DX12::GetInstance()->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// �`��(Draw�֐�)����
		// ���_�o�b�t�@�[�̎w��
		DX12::GetInstance()->CommandList()->IASetVertexBuffers(0, 1, &_boxMeshs[m]->vertexBufferView);
		// �C���f�b�N�X�o�b�t�@�[�̎w��
		DX12::GetInstance()->CommandList()->IASetIndexBuffer(&_boxMeshs[m]->indexBufferView);
		// �f�B�X�N���v�^�̐擪�n���h�����擾���Ă���
		auto basicHeapHandle = _boxMeshs[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart();
		// ���̏ꏊ�Ɉړ�
		basicHeapHandle.ptr +=
			DX12::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize
			(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// �ŏ��̓I�t�Z�b�g�Ȃ�
		unsigned int idxOffset = 0;

		// ���[�g�p�����[�^�[�ƃf�B�X�N���v�^�[�q�[�v�̊֘A�t��
		DX12::GetInstance()->CommandList()->SetGraphicsRootDescriptorTable(
			1,			// ���[�g�p�����[�^�[�C���f�b�N�X
			basicHeapHandle); // �q�[�v�A�h���X

		// �`�施��
		DX12::GetInstance()->CommandList()->DrawIndexedInstanced(BoxNumIndices, EntryNum, idxOffset, 0, 0);
	}
}

// �`��(��)
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

		MetaData* mapMetaData = nullptr;	// �}�b�v��������|�C���^�[
		_cylinderMeshs[m]->materialBuffer->Map(0, nullptr, (void**)&mapMetaData);	// �}�b�v
		*mapMetaData = _cylinderMetaData[m];
		_cylinderMeshs[m]->materialBuffer->Unmap(0, nullptr);		// �A���}�b�v

		// =====�r���[�s��=====
		XMMATRIX viewMat = Camera::GetInstance()->GetView();
		// =====�v���W�F�N�V�����s��=====
		XMMATRIX projMat = Camera::GetInstance()->GetProjection();
		// ===== ���C�g�̌��� =====
		XMFLOAT4 lightVector = Camera::GetInstance()->GetLightVector();
		// ===== ���C�g�̐F =====
		XMFLOAT4 lightColor = Camera::GetInstance()->GetLightColor();
		// ===== �J�������W =====
		XMFLOAT3 eye = Camera::GetInstance()->GetEyePosition();

		// �萔���̍X�V
		ConstantBuffer* constBuffer = nullptr;	// �}�b�v��������|�C���^�[
		_cylinderMeshs[m]->constBuffer->Map(0, nullptr, (void**)&constBuffer);	// �}�b�v
		constBuffer->view = viewMat;
		constBuffer->projection = projMat;
		constBuffer->eye = eye;
		constBuffer->lightVector = lightVector;
		constBuffer->lightColor = lightColor;
		_cylinderMeshs[m]->constBuffer->Unmap(0, nullptr);		// �A���}�b�v

		// �O���t�B�b�N�X�p�C�v���C���X�e�[�g�̎w��
		DX12::GetInstance()->CommandList()->SetPipelineState(_pipelineState.Get());
		// ���[�g�V�O�l�`���[�̎w��
		DX12::GetInstance()->CommandList()->SetGraphicsRootSignature(_rootSignature.Get());

		// �f�B�X�N���v�^�[�q�[�v�̎w��
		DX12::GetInstance()->CommandList()->SetDescriptorHeaps(1, _cylinderMeshs[m]->basicDescHeap.GetAddressOf());

		// ���[�g�p�����[�^�[�ƃf�B�X�N���v�^�[�q�[�v�̊֘A�t��
		DX12::GetInstance()->CommandList()->SetGraphicsRootDescriptorTable(
			0,	// ���[�g�p�����[�^�[�C���f�b�N�X
			_cylinderMeshs[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart());    // �q�[�v�A�h���X

		// ���_���̎w��[�O�p�`]
		DX12::GetInstance()->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// �`��(Draw�֐�)����
		// ���_�o�b�t�@�[�̎w��
		DX12::GetInstance()->CommandList()->IASetVertexBuffers(0, 1, &_cylinderMeshs[m]->vertexBufferView);
		// �C���f�b�N�X�o�b�t�@�[�̎w��
		DX12::GetInstance()->CommandList()->IASetIndexBuffer(&_cylinderMeshs[m]->indexBufferView);
		// �f�B�X�N���v�^�̐擪�n���h�����擾���Ă���
		auto basicHeapHandle = _cylinderMeshs[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart();
		// ���̏ꏊ�Ɉړ�
		basicHeapHandle.ptr +=
			DX12::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize
			(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// �ŏ��̓I�t�Z�b�g�Ȃ�
		unsigned int idxOffset = 0;

		// ���[�g�p�����[�^�[�ƃf�B�X�N���v�^�[�q�[�v�̊֘A�t��
		DX12::GetInstance()->CommandList()->SetGraphicsRootDescriptorTable(
			1,			// ���[�g�p�����[�^�[�C���f�b�N�X
			basicHeapHandle); // �q�[�v�A�h���X

		// �`�施��
		DX12::GetInstance()->CommandList()->DrawIndexedInstanced(CylinderNumIndices, EntryNum, idxOffset, 0, 0);
	}
}

// �`��(��)
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

		MetaData* mapMetaData = nullptr;	// �}�b�v��������|�C���^�[
		_sphereMeshs[m]->materialBuffer->Map(0, nullptr, (void**)&mapMetaData);	// �}�b�v
		*mapMetaData = _sphereMetaData[m];
		_sphereMeshs[m]->materialBuffer->Unmap(0, nullptr);		// �A���}�b�v

		// =====�r���[�s��=====
		XMMATRIX viewMat = Camera::GetInstance()->GetView();
		// =====�v���W�F�N�V�����s��=====
		XMMATRIX projMat = Camera::GetInstance()->GetProjection();
		// ===== ���C�g�̌��� =====
		XMFLOAT4 lightVector = Camera::GetInstance()->GetLightVector();
		// ===== ���C�g�̐F =====
		XMFLOAT4 lightColor = Camera::GetInstance()->GetLightColor();
		// ===== �J�������W =====
		XMFLOAT3 eye = Camera::GetInstance()->GetEyePosition();

		// �萔���̍X�V
		ConstantBuffer* constBuffer = nullptr;	// �}�b�v��������|�C���^�[
		_sphereMeshs[m]->constBuffer->Map(0, nullptr, (void**)&constBuffer);	// �}�b�v
		constBuffer->view = viewMat;
		constBuffer->projection = projMat;
		constBuffer->eye = eye;
		constBuffer->lightVector = lightVector;
		constBuffer->lightColor = lightColor;
		_sphereMeshs[m]->constBuffer->Unmap(0, nullptr);		// �A���}�b�v

		// �O���t�B�b�N�X�p�C�v���C���X�e�[�g�̎w��
		DX12::GetInstance()->CommandList()->SetPipelineState(_pipelineState.Get());
		// ���[�g�V�O�l�`���[�̎w��
		DX12::GetInstance()->CommandList()->SetGraphicsRootSignature(_rootSignature.Get());

		// �f�B�X�N���v�^�[�q�[�v�̎w��
		DX12::GetInstance()->CommandList()->SetDescriptorHeaps(1, _sphereMeshs[m]->basicDescHeap.GetAddressOf());

		// ���[�g�p�����[�^�[�ƃf�B�X�N���v�^�[�q�[�v�̊֘A�t��
		DX12::GetInstance()->CommandList()->SetGraphicsRootDescriptorTable(
			0,	// ���[�g�p�����[�^�[�C���f�b�N�X
			_sphereMeshs[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart());    // �q�[�v�A�h���X

		// ���_���̎w��[�O�p�`]
		DX12::GetInstance()->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// �`��(Draw�֐�)����
		// ���_�o�b�t�@�[�̎w��
		DX12::GetInstance()->CommandList()->IASetVertexBuffers(0, 1, &_sphereMeshs[m]->vertexBufferView);
		// �C���f�b�N�X�o�b�t�@�[�̎w��
		DX12::GetInstance()->CommandList()->IASetIndexBuffer(&_sphereMeshs[m]->indexBufferView);
		// �f�B�X�N���v�^�̐擪�n���h�����擾���Ă���
		auto basicHeapHandle = _sphereMeshs[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart();
		// ���̏ꏊ�Ɉړ�
		basicHeapHandle.ptr +=
			DX12::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize
			(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// �ŏ��̓I�t�Z�b�g�Ȃ�
		unsigned int idxOffset = 0;

		// ���[�g�p�����[�^�[�ƃf�B�X�N���v�^�[�q�[�v�̊֘A�t��
		DX12::GetInstance()->CommandList()->SetGraphicsRootDescriptorTable(
			1,			// ���[�g�p�����[�^�[�C���f�b�N�X
			basicHeapHandle); // �q�[�v�A�h���X

		// �`�施��
		DX12::GetInstance()->CommandList()->DrawIndexedInstanced(SphereNumIndices, EntryNum, idxOffset, 0, 0);
	}
}

// �`��(�~)
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

		MetaData* mapMetaData = nullptr;	// �}�b�v��������|�C���^�[
		_circleMeshs[m]->materialBuffer->Map(0, nullptr, (void**)&mapMetaData);	// �}�b�v
		*mapMetaData = _circleMetaData[m];
		_circleMeshs[m]->materialBuffer->Unmap(0, nullptr);		// �A���}�b�v

		// =====�r���[�s��=====
		XMMATRIX viewMat = XMMatrixIdentity();
		// =====�v���W�F�N�V�����s��=====
		XMMATRIX projMat = XMMatrixIdentity();
		// ===== ���C�g�̌��� =====
		XMFLOAT4 lightVector = Camera::GetInstance()->GetLightVector();
		// ===== ���C�g�̐F =====
		XMFLOAT4 lightColor = Camera::GetInstance()->GetLightColor();
		// ===== �J�������W =====
		XMFLOAT3 eye = Camera::GetInstance()->GetEyePosition();

		// �萔���̍X�V
		ConstantBuffer* constBuffer = nullptr;	// �}�b�v��������|�C���^�[
		_circleMeshs[m]->constBuffer->Map(0, nullptr, (void**)&constBuffer);	// �}�b�v
		constBuffer->view = viewMat;
		constBuffer->projection = projMat;
		constBuffer->eye = eye;
		constBuffer->lightVector = lightVector;
		constBuffer->lightColor = lightColor;
		_circleMeshs[m]->constBuffer->Unmap(0, nullptr);		// �A���}�b�v

		// �O���t�B�b�N�X�p�C�v���C���X�e�[�g�̎w��
		DX12::GetInstance()->CommandList()->SetPipelineState(_pipelineState.Get());
		// ���[�g�V�O�l�`���[�̎w��
		DX12::GetInstance()->CommandList()->SetGraphicsRootSignature(_rootSignature.Get());

		// �f�B�X�N���v�^�[�q�[�v�̎w��
		DX12::GetInstance()->CommandList()->SetDescriptorHeaps(1, _circleMeshs[m]->basicDescHeap.GetAddressOf());

		// ���[�g�p�����[�^�[�ƃf�B�X�N���v�^�[�q�[�v�̊֘A�t��
		DX12::GetInstance()->CommandList()->SetGraphicsRootDescriptorTable(
			0,	// ���[�g�p�����[�^�[�C���f�b�N�X
			_circleMeshs[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart());    // �q�[�v�A�h���X

		// ���_���̎w��[�O�p�`]
		DX12::GetInstance()->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// �`��(Draw�֐�)����
		// ���_�o�b�t�@�[�̎w��
		DX12::GetInstance()->CommandList()->IASetVertexBuffers(0, 1, &_circleMeshs[m]->vertexBufferView);
		// �C���f�b�N�X�o�b�t�@�[�̎w��
		DX12::GetInstance()->CommandList()->IASetIndexBuffer(&_circleMeshs[m]->indexBufferView);
		// �f�B�X�N���v�^�̐擪�n���h�����擾���Ă���
		auto basicHeapHandle = _circleMeshs[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart();
		// ���̏ꏊ�Ɉړ�
		basicHeapHandle.ptr +=
			DX12::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize
			(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// �ŏ��̓I�t�Z�b�g�Ȃ�
		unsigned int idxOffset = 0;

		// ���[�g�p�����[�^�[�ƃf�B�X�N���v�^�[�q�[�v�̊֘A�t��
		DX12::GetInstance()->CommandList()->SetGraphicsRootDescriptorTable(
			1,			// ���[�g�p�����[�^�[�C���f�b�N�X
			basicHeapHandle); // �q�[�v�A�h���X

		// �`�施��
		DX12::GetInstance()->CommandList()->DrawIndexedInstanced(_circleIndicesNum, EntryNum, idxOffset, 0, 0);
	}
}


// ��
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
	// �ŏ���������500�Ԗڂ܂Ŏg���؂����玟�̃o�b�t�@�[��p�ӂ���
	int num = _boxInstanceEntry % instanceMax;

	if (num == 0&& entryPos == -1)
	{
		_boxMeshs.push_back(new MESH);
		_boxMeshs.back()->vertexBuffer = boxVertexBuffer;
		_boxMeshs.back()->vertexBufferView = boxVertexBufferView;
		_boxMeshs.back()->indexBuffer = boxIndexBuffer;
		_boxMeshs.back()->indexBufferView = boxIndexBufferView;
		CreateMaterialBuffer(_boxMeshs.back());	// �}�e���A������
		// �R���X�^���g�o�b�t�@�[�r���[�̐���
		CreateConstantBufferView(_boxMeshs.back());

		_boxMetaData.resize(_boxMeshs.size());
		_boxDataCheck.resize(_boxMeshs.size());
	}
	// �G���g���[�ԍ�
	int EntryPos;
	// �g�p���郁�^�f�[�^�̔ԍ�
	int ArrayNum;
	// ���̃X�y�L�����[
	XMFLOAT4 specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 15.0f);
	//// =====���[���h�s��=====
	XMMATRIX world_matrix;
	XMMATRIX translate = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX rotate_x = XMMatrixRotationX(XMConvertToRadians(angle.x));
	XMMATRIX rotate_y = XMMatrixRotationY(XMConvertToRadians(angle.y));
	XMMATRIX rotate_z = XMMatrixRotationZ(XMConvertToRadians(angle.z));
	XMMATRIX scale_mat = XMMatrixScaling(scale.x, scale.y, scale.z);
	world_matrix = scale_mat * rotate_z * rotate_x * rotate_y * translate;

	if (entryPos == -1)
	{
		// �󂢂Ă�G���g���[�ԍ���T��
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

		// �������郉�C�������
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
		// �������郉�C�������
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

// �~��
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
	// �ŏ���������500�Ԗڂ܂Ŏg���؂����玟�̃o�b�t�@�[��p�ӂ���
	int num = _cylinderInstanceEntry % instanceMax;

	if (num == 0 && entryPos == -1)
	{
		_cylinderMeshs.push_back(new MESH);
		_cylinderMeshs.back()->vertexBuffer = cylinderVertexBuffer;
		_cylinderMeshs.back()->vertexBufferView = cylinderVertexBufferView;
		_cylinderMeshs.back()->indexBuffer = cylinderIndexBuffer;
		_cylinderMeshs.back()->indexBufferView = cylinderIndexBufferView;
		CreateMaterialBuffer(_cylinderMeshs.back());	// �}�e���A������
		// �R���X�^���g�o�b�t�@�[�r���[�̐���
		CreateConstantBufferView(_cylinderMeshs.back());

		_cylinderMetaData.resize(_cylinderMeshs.size());
		_cylinderDataCheck.resize(_cylinderMeshs.size());
	}
	// �G���g���[�ԍ�
	int EntryPos;
	// �g�p���郁�^�f�[�^�̔ԍ�
	int ArrayNum;
	// ���̃X�y�L�����[
	XMFLOAT4 specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 15.0f);
	//// =====���[���h�s��=====
	XMMATRIX world_matrix;
	XMMATRIX translate = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX rotate_x = XMMatrixRotationX(XMConvertToRadians(angle.x));
	XMMATRIX rotate_y = XMMatrixRotationY(XMConvertToRadians(angle.y));
	XMMATRIX rotate_z = XMMatrixRotationZ(XMConvertToRadians(angle.z));
	XMMATRIX scale_mat = XMMatrixScaling(scale.x, scale.y, scale.z);
	world_matrix = scale_mat * rotate_z * rotate_x * rotate_y * translate;

	if (entryPos == -1)
	{
		// �󂢂Ă�G���g���[�ԍ���T��
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

		// �������郉�C�������
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
		// �������郉�C�������
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

// �~��
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
	// �ŏ���������500�Ԗڂ܂Ŏg���؂����玟�̃o�b�t�@�[��p�ӂ���
	int num = _sphereInstanceEntry % instanceMax;

	if (num == 0 && entryPos == -1)
	{
		_sphereMeshs.push_back(new MESH);
		_sphereMeshs.back()->vertexBuffer = sphereVertexBuffer;
		_sphereMeshs.back()->vertexBufferView = sphereVertexBufferView;
		_sphereMeshs.back()->indexBuffer = sphereIndexBuffer;
		_sphereMeshs.back()->indexBufferView = sphereIndexBufferView;
		CreateMaterialBuffer(_sphereMeshs.back());	// �}�e���A������
		// �R���X�^���g�o�b�t�@�[�r���[�̐���
		CreateConstantBufferView(_sphereMeshs.back());

		_sphereMetaData.resize(_sphereMeshs.size());
		_sphereDataCheck.resize(_sphereMeshs.size());
	}
	// �G���g���[�ԍ�
	int EntryPos;
	// �g�p���郁�^�f�[�^�̔ԍ�
	int ArrayNum;
	// ���̃X�y�L�����[
	XMFLOAT4 specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 15.0f);
	//// =====���[���h�s��=====
	XMMATRIX world_matrix;
	XMMATRIX translate = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX rotate_x = XMMatrixRotationX(XMConvertToRadians(angle.x));
	XMMATRIX rotate_y = XMMatrixRotationY(XMConvertToRadians(angle.y));
	XMMATRIX rotate_z = XMMatrixRotationZ(XMConvertToRadians(angle.z));
	XMMATRIX scale_mat = XMMatrixScaling(scale.x, scale.y, scale.z);
	world_matrix = scale_mat * rotate_z * rotate_x * rotate_y * translate;

	if (entryPos == -1)
	{
		// �󂢂Ă�G���g���[�ԍ���T��
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

		// �������郉�C�������
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
		// �������郉�C�������
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

// �~��
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
	// �ŏ���������500�Ԗڂ܂Ŏg���؂����玟�̃o�b�t�@�[��p�ӂ���
	int num = _circleInstanceEntry % instanceMax;

	if (num == 0 && entryPos == -1)
	{
		_circleMeshs.push_back(new MESH);
		_circleMeshs.back()->vertexBuffer = circleVertexBuffer;
		_circleMeshs.back()->vertexBufferView = circleVertexBufferView;
		_circleMeshs.back()->indexBuffer = circleIndexBuffer;
		_circleMeshs.back()->indexBufferView = circleIndexBufferView;
		CreateMaterialBuffer(_circleMeshs.back());	// �}�e���A������
		// �R���X�^���g�o�b�t�@�[�r���[�̐���
		CreateConstantBufferView(_circleMeshs.back());

		_circleMetaData.resize(_circleMeshs.size());
		_circleDataCheck.resize(_circleMeshs.size());
	}
	// �G���g���[�ԍ�
	int EntryPos;
	// �g�p���郁�^�f�[�^�̔ԍ�
	int ArrayNum;
	// ���̃X�y�L�����[
	XMFLOAT4 specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 15.0f);
	//// =====���[���h�s��=====
	// �摜������񂹂��邽�߂̕ϐ�
	LONG x = g_window_width / 2;
	LONG y = g_window_height / 2;

	XMMATRIX world_matrix = XMMatrixIdentity();
	world_matrix.r[0].m128_f32[0] = scale.x / x;
	world_matrix.r[1].m128_f32[1] = -scale.y / y;
	world_matrix.r[3].m128_f32[0] = position.x / x - 1;		// ����X���W
	world_matrix.r[3].m128_f32[1] = -(position.y / y - 1);	// ����Y���W

	// ��]�s��
	XMMATRIX rotationMat = XMMatrixRotationZ(XMConvertToRadians(angle.z));
	world_matrix = rotationMat * world_matrix;

	if (entryPos == -1)
	{
		// �󂢂Ă�G���g���[�ԍ���T��
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

		// �������郉�C�������
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
		// �������郉�C�������
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