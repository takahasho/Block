// --------------------- Include File --------------------
#include "ObjModel.h"
#include "GeometryData.h"
#include "../Camera/Camera.h"
#include "../DX12System.h"
#include "../VECTOR.h"
ObjModel* ObjModel::_instance = nullptr;			// �C���X�^���X�̎���
// �R���X�g���N�^�[
ObjModel::ObjModel() :
	_path(nullptr), _attributeEntryFlag(FALSE), _materialEntryFlag(FALSE)
{
}
// �f�X�g���N�^�[
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
// ����
void ObjModel::Create()
{
	if (!_instance)
	{
		_instance = new ObjModel();

		_instance->CreateObjModel();
	}
}
// �j��
void ObjModel::Destroy()
{
	delete _instance;
}


// ����
HRESULT ObjModel::CreateObjModel()
{
	HRESULT hr = S_OK;

	_dev = DX12::GetInstance()->GetDevice();
	_cmdList = DX12::GetInstance()->CommandList();

	// ���[�g�V�O�l�`���[�̐���
	if (FAILED(CreateRootSignature()))
		return E_FAIL;

	// �`��p�C�v���C���X�e�[�g�̐���
	if (FAILED(CreatePipelineState()))
		return E_FAIL;

	return S_OK;
}

// ���[�g�V�O�l�`���[�̐���
HRESULT ObjModel::CreateRootSignature()
{
	HRESULT hr = S_OK;

	// �f�B�X�N���v�^�[�����W���쐬
	D3D12_DESCRIPTOR_RANGE descTblRange[3] = {};
	// �f�B�X�N���v�^�[�̐�[�P��]
	descTblRange[0].NumDescriptors = 1;
	// �����W�̎��[CBV:�R���X�^���g�o�b�t�@�[�r���[]
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// �擪���W�X�^�[�ԍ�[0��I��]
	descTblRange[0].BaseShaderRegister = 0;
	// �I�t�Z�b�g���w��[�A�������f�B�X�N���v�^�[�����W���O�̃f�B�X�N���v�^�����W�̒���ɂ���]
	descTblRange[0].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// �f�B�X�N���v�^�[�̐�[�P��]
	descTblRange[1].NumDescriptors = 1;
	// �����W�̎��[CBV:�R���X�^���g�o�b�t�@�[�r���[]
	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// �擪���W�X�^�[�ԍ�[1��I��]
	descTblRange[1].BaseShaderRegister = 1;
	// �I�t�Z�b�g���w��[�A�������f�B�X�N���v�^�[�����W���O�̃f�B�X�N���v�^�����W�̒���ɂ���]
	descTblRange[1].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// �f�B�X�N���v�^�[�̐�[�P��]
	descTblRange[2].NumDescriptors = 1;
	// �����W�̎��[SRV:�V�F�[�_�[���\�[�X�r���[]
	descTblRange[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	// �擪���W�X�^�[�ԍ�[0��I��]
	descTblRange[2].BaseShaderRegister = 0;
	// �I�t�Z�b�g���w��[�A�������f�B�X�N���v�^�[�����W���O�̃f�B�X�N���v�^�����W�̒���ɂ���]
	descTblRange[2].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	// ���[�g�p�����[�^�[���쐬
	D3D12_ROOT_PARAMETER rootParam[2] = {};
	// ��ʂ̑I��[�f�B�X�N���v�^�[�e�[�u����I��]
	rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// �ǂ̃V�F�[�_�[���痘�p�\��[�S�ẴV�F�[�_�[]
	rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// �f�B�X�N���v�^�[�����W�̃A�h���X
	rootParam[0].DescriptorTable.pDescriptorRanges =
		&descTblRange[0];
	// �f�B�X�N���v�^�[�����W�̐�
	rootParam[0].DescriptorTable.NumDescriptorRanges = 1;
	// ��ʂ̑I��[�f�B�X�N���v�^�[�e�[�u����I��]
	rootParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// �ǂ̃V�F�[�_�[���痘�p�\��[�S�ẴV�F�[�_�[]
	rootParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// �f�B�X�N���v�^�[�����W�̃A�h���X
	rootParam[1].DescriptorTable.pDescriptorRanges =
		&descTblRange[1];
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
	rootSignatureDesc.pParameters = &rootParam[0];
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
HRESULT ObjModel::CreatePipelineState()
{
	HRESULT hr = S_OK;

	// �V�F�[�_�[�I�u�W�F�N�g�ǂݍ��ݗp
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// ���_�V�F�[�_�[�̓ǂݍ���
	hr = D3DCompileFromFile(
		L".\\Shader\\Obj\\ObjVertexShader.hlsl",// �V�F�[�_�[��
		nullptr,							// define�͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// �C���N���[�h�̓f�t�H���g
		"ObjVS", "vs_5_0",			// �֐��� BasicVS�A�ΏۃV�F�[�_�[�� vs_5_0
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
		L".\\Shader\\Obj\\ObjPixelShader.hlsl",	// �V�F�[�_�[��
		nullptr,							// define�͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// �C���N���[�h�̓f�t�H���g
		"ObjPS", "ps_5_0",			// �֐��� BasicPS�A�ΏۃV�F�[�_�[�� ps_5_0
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
		{	// �@�����
			"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
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
	// �J�����O�̐ݒ�(�\�`��)
	gpipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
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
	hr = _dev->CreateGraphicsPipelineState(
		&gpipelineDesc, IID_PPV_ARGS(_pipelineState.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	return S_OK;
}

// �}�e���A���o�b�t�@�[�̐���
HRESULT ObjModel::CreateMaterialBuffer(MESH* mesh)
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
	resDesc.Width = materialBufferSize * (UINT)_material.size();
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
	hr = _dev->CreateCommittedResource(
		&uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,	// ���Ɏw��Ȃ�
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,	// CPU����̏������݉\�AGPU���猩��Ɠǂݎ��̂�
		nullptr,
		IID_PPV_ARGS(mesh->materialBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	return S_OK;
}
// �V�F�[�_�[���\�[�X�̐���
HRESULT ObjModel::CreateShaderResource(
	const TCHAR* pFileName,
	ComPtr<ID3D12Resource>& textureBuffer)
{
	HRESULT hr = S_OK;

	// �摜�T�C�Y�̎擾�ƃC���[�W�f�[�^�[�m��
	BYTE* pImageData = nullptr;
	SIZE size = {};
	if (lstrlenW(pFileName) > 0)
	{
		// �摜���̓ǂݍ���
		if (FAILED(LoadTextureFromFile(pFileName,
			&pImageData, &size)))
			return E_FAIL;
	}
	else
	{
		// �_�~�[�摜(���F)�̐���
		size.cx = 4;
		size.cy = 4;
		// 1�s�N�Z��������̃o�C�g��
		UINT bytePerPixel = 4;
		// �f�[�^�T�C�Y�̌���(RGBA)
		UINT imageSize = size.cx * size.cy * bytePerPixel;
		// �ۑ��̈�쐬
		pImageData = new BYTE[imageSize];
		::FillMemory(pImageData, imageSize, 0xFF);
	}

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

// �R���X�^���g�o�b�t�@�[�r���[�̐���
HRESULT ObjModel::CreateConstantBufferView(MESH* mesh)
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

	ConstantBuffer constantBuffer = {};
	// ���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC resDesc = {};
	// ���p���@[�o�b�t�@�[�Ɏg���̂�BUFFER���w��]
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	// �T�C�Y��[���őS���܂��Ȃ��̂�sizeof(�S���):256�̔{���Ƃ���]
	resDesc.Width = (sizeof(constantBuffer) + 0xff) & ~0xff;
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
	// �f�X�N���v�^���w��[CBV:1�� (CBV:1�� + SRV:1��) * ��]
	heapDesc.NumDescriptors = 1 + 2 * (UINT)_material.size();
	// ���Ɏw��Ȃ�[�V�F�[�_�[�����猩����K�v����]
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// �f�X�N���v�^�q�[�v���쐬
	hr = _dev->CreateDescriptorHeap(&heapDesc
		, IID_PPV_ARGS(mesh->basicDescHeap.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

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

	// �f�B�X�N���v�^�̐擪�n���h�����擾���Ă���
	auto basicHeapHandle = mesh->basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = mesh->constBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)mesh->constBuffer->GetDesc().Width;

	// �萔�o�b�t�@�[�r���[�̍쐬
	_dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	// ���̏ꏊ�Ɉړ�
	basicHeapHandle.ptr +=
		_dev->GetDescriptorHandleIncrementSize
		(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// �}�e���A���p�o�b�t�@�[���쐬
	auto materialBufferSize = sizeof(MetaData);
	materialBufferSize = (materialBufferSize + 0xff) & ~0xff;
	cbvDesc.BufferLocation = mesh->materialBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)materialBufferSize;

	auto incSize = _dev->GetDescriptorHandleIncrementSize
	(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (UINT i = 0; i < _material.size(); ++i)
	{
		// �萔�o�b�t�@�[�r���[�̍쐬
		_dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
		// �}�e���A���̃��P�[�V�������ړ�
		cbvDesc.BufferLocation += materialBufferSize;

		// �o�^��̃A�h���X���ړ�
		basicHeapHandle.ptr += incSize;

		// �V�F�[�_�[���\�[�X�r���[�̐���
		_dev->CreateShaderResourceView(
			mesh->textureBuffer[i].Get(),	// �r���[�Ɗ֘A�t����o�b�t�@�[���w��
			&srvDesc,				// �e�N�X�`���[�I������w��
			basicHeapHandle);		// �q�[�v�̂ǂ��Ɋ��蓖�Ă邩

		// �o�^��̃A�h���X���ړ�
		basicHeapHandle.ptr += incSize;
	}

	return S_OK;
}

// �`��
void ObjModel::Draw()
{
	for (auto mesh : _objMeshs)
	{
		if (_objInstanceEntry[mesh.first] == 0) {
			continue;
		}
		for (int m = 0, size = (int)mesh.second.size(); m < size; ++m)
		{

			// �}�e���A���p�o�b�t�@�[���쐬
			auto materialBufferSize = sizeof(MetaData);
			materialBufferSize = (materialBufferSize + 0xff) & ~0xff;
			// �}�b�v�}�e���A���ɃR�s�[
			BYTE* mapMaterial = nullptr;
			mesh.second[m]->materialBuffer->Map(0, nullptr, (void**)&mapMaterial);	// �}�b�v

			int EntryNum = _objInstanceEntry[mesh.first] - m * instanceMax;
			if (EntryNum >= instanceMax)
			{
				EntryNum = instanceMax;
			}
	
			for (std::vector<MATERIAL>::size_type k = 0, size = (int)_saveMaterial[mesh.first].size();
				k < size; ++k)
			{			
				*((MetaData*)mapMaterial) = _objMetaData[mesh.first][m][k];	// �f�[�^�[�R�s�[
				mapMaterial += materialBufferSize;	// ���̃A���C�����g�ʒu�܂Ői�߂�(256�̔{��)
			}
			mesh.second[m]->materialBuffer->Unmap(0, nullptr);	// �A���}�b�v

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
			ConstantBuffer* mapMatrix = nullptr;	// �}�b�v��������|�C���^�[
			mesh.second[m]->constBuffer->Map(0, nullptr, (void**)&mapMatrix);	// �}�b�v
			// �s��̓��e���R�s�[
			mapMatrix->view = viewMat;
			mapMatrix->projection = projMat;
			mapMatrix->lightVector = lightVector;
			mapMatrix->lightColor = lightColor;
			mapMatrix->eye = eye;
			mesh.second[m]->constBuffer->Unmap(0, nullptr);		// �A���}�b�v

			// �O���t�B�b�N�X�p�C�v���C���X�e�[�g�̎w��
			_cmdList->SetPipelineState(_pipelineState.Get());
			// ���[�g�V�O�l�`���[�̎w��
			_cmdList->SetGraphicsRootSignature(_rootSignature.Get());

			// �f�B�X�N���v�^�[�q�[�v�̎w��
			_cmdList->SetDescriptorHeaps(1, mesh.second[m]->basicDescHeap.GetAddressOf());

			// ���[�g�p�����[�^�[�ƃf�B�X�N���v�^�[�q�[�v�̊֘A�t��
			_cmdList->SetGraphicsRootDescriptorTable(
				0,	// ���[�g�p�����[�^�[�C���f�b�N�X
				mesh.second[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart());

			// ���_���̎w��[�O�p�`]
			_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// ���_�o�b�t�@�[�̎w��
			_cmdList->IASetVertexBuffers(0, 1, &mesh.second[m]->vertexBufferView);

			// �C���f�b�N�X�o�b�t�@�[�̎w��
			_cmdList->IASetIndexBuffer(&mesh.second[m]->indexBufferView);

			// �f�B�X�N���v�^�̐擪�n���h�����擾���Ă���
			auto basicHeapHandle = mesh.second[m]->basicDescHeap->GetGPUDescriptorHandleForHeapStart();

			// ���̏ꏊ�Ɉړ�
			basicHeapHandle.ptr +=
				_dev->GetDescriptorHandleIncrementSize
				(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			// CBV + SRV ��2��
			auto incSize = _dev->GetDescriptorHandleIncrementSize
			(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 2;

			// �ŏ��̓I�t�Z�b�g�Ȃ�
			UINT idxOffset = 0;

			for (std::vector<ATTRIBUTE>::size_type i = 0,size= _saveAttribute[mesh.first].size();
				i < size; ++i)
			{
				// ���[�g�p�����[�^�[�ƃf�B�X�N���v�^�[�q�[�v�̊֘A�t��
				_cmdList->SetGraphicsRootDescriptorTable(
					1,	// ���[�g�p�����[�^�[�C���f�b�N�X
					basicHeapHandle);

				UINT verticesNum = _saveAttribute[mesh.first][i].verticesNum;

				// �`�施��
				_cmdList->DrawIndexedInstanced(verticesNum, _objInstanceEntry[mesh.first], idxOffset, 0, 0);

				// �Ăяo����̃A�h���X���ړ�
				basicHeapHandle.ptr += incSize;

				// �I�t�Z�b�g��i�܂���
				idxOffset += verticesNum;
			}
		}
	}
}
// OBJ�t�@�C���̓ǂݍ���
BOOL ObjModel::LoadObjFromFile(const char* filename)
{

	INT size = (INT)strlen(filename) + 1;

	FILE* fp = nullptr;	// �t�@�C���|�C���^�[

	// �t�@�C�����J����
	fopen_s(&fp, filename, "rt");
	if (fp == nullptr)
	{
		return FALSE;
	}

	TCHAR line[MAX_PATH] = _T("");
	TCHAR keyWord[MAX_PATH] = _T("");

	ATTRIBUTE attribute = {};

	BOOL faceTriangleStartFlag = TRUE;	// �ʏ������߂Ď擾�������̗L��
	BOOL faceQuadrangleStartFlag = TRUE;// �ʏ������߂Ď擾�������̗L��
	UINT triangleStartIndex = 0;		// �ʏ��̊J�n�C���f�b�N�X
	UINT quadrangleStartIndex = 0;		// �ʏ��̊J�n�C���f�b�N�X

	BOOL entryTriangleIndex = FALSE;	// Triangle�o�^�̗L��
	BOOL entryQuadrangleIndex = FALSE;	// Quad�o�^�̗L��

	while (!feof(fp))
	{
		::ZeroMemory(line, sizeof(line));
		::ZeroMemory(keyWord, sizeof(keyWord));
		fgetws(line, MAX_PATH, fp);

		// �]���ȋ󔒂�ǂݔ�΂�
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

		// #:�R�����g�A\n:���s�R�[�h
		if (keyWord[0] == '#' ||
			keyWord[0] == '\n')
			;
		// "mtllib":�}�e���A�����̃t�@�C����
		else if (keyWord[0] == 'm' && keyWord[1] == 't' && keyWord[2] == 'l'
			&& keyWord[3] == 'l' && keyWord[4] == 'i' && keyWord[5] == 'b')
		{
			// �t�@�C�����擾
			TCHAR mtlFilename[MAX_PATH] = _T("");
			// 6�����ǂݍ��݈ʒu���ړ�������
			for (INT i = 6; i < size; ++i)
			{
				if (keyWord[i] != ' ')
				{
					UINT copySize = static_cast<UINT>(size - i);
					memcpy_s(mtlFilename, MAX_PATH, &keyWord[i], sizeof(TCHAR) * copySize);
					break;
				}
			}

			// �t�H���_�[�����݂���ꍇ�́A�t�@�C�����ɕt��
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

				// �t�H���_�[��char->wchar�ɕϊ�
				WCHAR* wFolder = new WCHAR[(strlen(folder) + 1) * 2];
				// ���P�[���w��
				setlocale(LC_ALL, "japanese");
				size_t len = 0;
				// CHARSTR -> LPCWSTR
				mbstowcs_s(&len, wFolder, strlen(folder) + 1, folder, _TRUNCATE);
				lstrcpyW(pathFilename, wFolder);
				// �p�X��ۑ�
				_path = new TCHAR[strlen(folder) + 1];
				StrCpyNW(_path, pathFilename,
					static_cast<INT>(strlen(folder) + 1));
				delete[] wFolder;
				delete[] folder;
			}

			lstrcatW(pathFilename, mtlFilename);

			// �}�e���A�����̓ǂݍ���
			if (!LoadMaterialFromFile(pathFilename))
			{
				// �t�@�C�������
				fclose(fp);
				return FALSE;
			}
		}

		// usemtl:�g�p����}�e���A����
		if (keyWord[0] == 'u' && keyWord[1] == 's' && keyWord[2] == 'e' &&
			keyWord[3] == 'm' && keyWord[4] == 't' && keyWord[5] == 'l')
		{
			// �}�e���A�������擾
			TCHAR name[MAX_PATH] = _T("");
			// 6�����ǂݍ��݈ʒu���ړ�������
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
				// �O�񖘂̑�������ۑ�
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
			// ���O�̓o�^
			StrCpyNW(attribute.materialName
				, name, lstrlenW(name) + 1);
			_attribute.push_back(attribute);
		}
		// "v ":���_���
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
		// "vn":�@�����
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
		// "vt":�e�N�X�`���[UV���
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
		// "f ":��
		else if (keyWord[0] == 'f' && keyWord[1] == ' ')
		{
			UINT loadPos = 0;

			UINT triangleIndexSize = (UINT)_triangleIndex.size();
			UINT quadrangleIndexSize = (UINT)_quadrangleIndex.size();

			// keyWord���琔�l���o�����閘�i�܂���
			for (UINT i = 0; keyWord[i] != '\0'; ++i)
			{
				if ('0' <= keyWord[i] && keyWord[i] <= '9')
				{
					loadPos = i;
					break;
				}
			}

			// '/'�̏o���������߂�
			UINT count = 0;
			for (UINT i = loadPos; keyWord[i] != '\0'; ++i)
			{
				if (keyWord[i] == '/')
				{
					count++;
				}
			}

			// [��� * ���_�̐�]
			UINT indices[3 * 4] = { 0, };
			// �J�E���g��0�̏ꍇ:���_�̂�
			// �J�E���g��0�ȊO:���_/UV�l/�@��

			// �J�E���g��0�ȊO�̏ꍇ�́AUV�ɒl�����邩
			BOOL textureUV = FALSE;
			if (count != 0)
			{
				// ����1���_��ǂݍ���ŁA[1]�̒l���Ȃ��ꍇ�́A�e�N�X�`���[UV�l�Ȃ�
				swscanf_s(keyWord, _T("%*s%d/%d/%d")
					, &indices[0], &indices[1], &indices[2]);
				if (indices[1] > 0)
				{
					textureUV = TRUE;
				}
			}

			if (count == 0)
			{
				// �e�N�X�`���[UV�l�Ɩ@�����Ȃ��̂ŁA�_�~�[��0����
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
				// �e�N�X�`���[UV�l����
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

	// �O�񖘂̑�������ۑ�
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

	// �t�@�C�������
	fclose(fp);

	
	return TRUE;
}
// �}�e���A���̓ǂݍ���
BOOL ObjModel::LoadMaterialFromFile(const TCHAR* filename)
{
	// LPCWSTR -> CHARSTR�ύX
	size_t len = 0;
	INT size = lstrlenW(filename) + 1;
	char* newFilename = new char[size];
	wcstombs_s(&len, newFilename, size
		, filename, _TRUNCATE);

	FILE* fp = nullptr;	// �t�@�C���|�C���^�[

	// �t�@�C�����J����
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

		// �]���ȋ󔒂�ǂݔ�΂�
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

		// keyWord�T�C�Y���擾
		INT keyWordSize = lstrlenW(keyWord);

		// #:�R�����g�A\n:���s�R�[�h
		if (keyWord[0] == '#' ||
			keyWord[0] == '\n')
			;
		// newmtl:�g�p����}�e���A����
		else if (keyWord[0] == 'n' && keyWord[1] == 'e' && keyWord[2] == 'w' &&
			keyWord[3] == 'm' && keyWord[4] == 't' && keyWord[5] == 'l')
		{
			// �}�e���A�������擾
			TCHAR name[MAX_PATH] = _T("");
			UINT copySize = 0;
			// 6�����ǂݍ��݈ʒu���ړ�������
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
			// ���O�̓o�^
			memcpy_s(material.materialName, MAX_PATH, name, sizeof(TCHAR) * copySize);
		}
		// "Ns":�X�y�L�����w��
		else if (keyWord[0] == 'N' && keyWord[1] == 's')
		{
			swscanf_s(keyWord, _T("%*s%f"), &material.Ns);
		}
		// "d":�f�B�]���u��
		else if (keyWord[0] == 'd')
		{
			swscanf_s(keyWord, _T("%*s%f"), &material.dissolve);
		}
		// Tr:�����F
		else if (keyWord[0] == 'T' && keyWord[1] == 'r')
		{
			swscanf_s(keyWord, _T("%*s%f"), &material.Tr);
		}
		// "illum":�Ɩ����f��
		else if (keyWord[0] == 'i' && keyWord[1] == 'l' &&
			keyWord[2] == 'l' && keyWord[3] == 'u' && keyWord[4] == 'm')
		{
			swscanf_s(keyWord, _T("%*s%d"), &material.illum);
		}
		// "Ka":�A���r�G���g
		else if (keyWord[0] == 'K' && keyWord[1] == 'a')
		{
			swscanf_s(keyWord, _T("%*s%f%f%f"), &material.Ka.x, &material.Ka.y, &material.Ka.z);
		}
		// "Kd":�f�B�t���[�Y
		else if (keyWord[0] == 'K' && keyWord[1] == 'd')
		{
			swscanf_s(keyWord, _T("%*s%f%f%f"), &material.Kd.x, &material.Kd.y, &material.Kd.z);
		}
		// "Ks":�X�y�L�����[
		else if (keyWord[0] == 'K' && keyWord[1] == 's')
		{
			swscanf_s(keyWord, _T("%*s%f%f%f"), &material.Ks.x, &material.Ks.y, &material.Ks.z);
		}
		// �e�N�X�`���[���
		else if (keyWordSize > 6)
		{
			// map_Ka:�e�N�X�`���[�A���r�G���g��
			if (keyWord[0] == 'm' && keyWord[1] == 'a' && keyWord[2] == 'p' &&
				keyWord[3] == '_' && keyWord[4] == 'K' && keyWord[5] == 'a')
			{
				// �t�@�C����
				TCHAR name[MAX_PATH] = _T("");
				UINT copySize = 0;
				// 6�����ǂݍ��݈ʒu���ړ�������
				for (INT i = 6; i < keyWordSize; i++)
				{
					if (keyWord[i] != ' ')
					{
						copySize = static_cast<UINT>(keyWordSize - i);
						memcpy_s(name, MAX_PATH, &keyWord[i], sizeof(TCHAR) * copySize);
						break;
					}
				}
				// �t�@�C�����̓o�^
				TCHAR value = '\0';
				memcpy_s(&name[copySize], MAX_PATH, &value, sizeof(TCHAR));
				memcpy_s(material.textureNameKa, MAX_PATH, name, sizeof(TCHAR) * copySize);
			}
			// map_Kd:�e�N�X�`���[�f�B�t���[�Y��
			if (keyWord[0] == 'm' && keyWord[1] == 'a' && keyWord[2] == 'p' &&
				keyWord[3] == '_' && keyWord[4] == 'K' && keyWord[5] == 'd')
			{
				// �t�@�C����
				TCHAR name[MAX_PATH] = _T("");
				UINT copySize = 0;
				// 6�����ǂݍ��݈ʒu���ړ�������
				for (INT i = 6; i < keyWordSize; i++)
				{
					if (keyWord[i] != ' ')
					{
						copySize = static_cast<UINT>(keyWordSize - i);
						memcpy_s(name, MAX_PATH, &keyWord[i], sizeof(TCHAR) * copySize);
						break;
					}
				}
				// �t�@�C�����̓o�^
				TCHAR value = '\0';
				memcpy_s(&name[copySize], MAX_PATH, &value, sizeof(TCHAR));
				memcpy_s(material.textureNameKd, MAX_PATH, name, sizeof(TCHAR) * copySize);
			}
			// map_Ks:�e�N�X�`���[�X�y�L�����[��
			if (keyWord[0] == 'm' && keyWord[1] == 'a' && keyWord[2] == 'p' &&
				keyWord[3] == '_' && keyWord[4] == 'K' && keyWord[5] == 's')
			{
				// �t�@�C����
				TCHAR name[MAX_PATH] = _T("");
				UINT copySize = 0;
				// 6�����ǂݍ��݈ʒu���ړ�������
				for (INT i = 6; i < keyWordSize; i++)
				{
					if (keyWord[i] != ' ')
					{
						copySize = static_cast<UINT>(keyWordSize - i);
						memcpy_s(name, MAX_PATH, &keyWord[i], sizeof(TCHAR) * copySize);
						break;
					}
				}
				// �t�@�C�����̓o�^
				TCHAR value = '\0';
				memcpy_s(&name[copySize], MAX_PATH, &value, sizeof(TCHAR));
				memcpy_s(material.textureNameKs, MAX_PATH, name, sizeof(TCHAR) * copySize);
			}
			// map_Ns:�e�N�X�`���[�n�C���C�g������
			if (keyWord[0] == 'm' && keyWord[1] == 'a' && keyWord[2] == 'p' &&
				keyWord[3] == '_' && keyWord[4] == 'N' && keyWord[5] == 's')
			{
				// �t�@�C����
				TCHAR name[MAX_PATH] = _T("");
				UINT copySize = 0;
				// 6�����ǂݍ��݈ʒu���ړ�������
				for (INT i = 6; i < keyWordSize; i++)
				{
					if (keyWord[i] != ' ')
					{
						copySize = static_cast<UINT>(keyWordSize - i);
						memcpy_s(name, MAX_PATH, &keyWord[i], sizeof(TCHAR) * copySize);
						break;
					}
				}
				// �t�@�C�����̓o�^
				TCHAR value = '\0';
				memcpy_s(&name[copySize], MAX_PATH, &value, sizeof(TCHAR));
				memcpy_s(material.textureNameNs, MAX_PATH, name, sizeof(TCHAR) * copySize);
			}
		}
	}

	if (_materialEntryFlag)
		_material.push_back(material);

	delete[] newFilename;

	// �t�@�C�������
	fclose(fp);

	return TRUE;
}
// �ϊ�
HRESULT ObjModel::Convert(MESH* mesh, std::vector<VECTOR>& saveVertex)
{
	HRESULT hr = S_OK;

	UINT quadrangleIndexNum = (UINT)_quadrangleIndex.size();
	UINT triangleIndexNum = (UINT)_triangleIndex.size();
	UINT entryPos = 0;

	// �������̍X�V
	UINT polygonSize = 3;
	UINT verticesNum = 0;
	UINT attributeNum = (UINT)_attribute.size();
	UINT materialNum = (UINT)_material.size();

	for (UINT i = 0; i < attributeNum; ++i)
	{
		// �}�e���A���ԍ��̌���
		for (UINT j = 0; j < materialNum; ++j)
		{
			if (lstrcmpW(_material[j].materialName,
				_attribute[i].materialName) == 0)
			{
				_attribute[i].materialNumber = j;
				break;
			}
		}
		// ���_�`�搔�̌���
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

	// Quadrangle -> Triangle�̏��Ԃœo�^
	VERTICES* vertices = new VERTICES[verticesNum];
	::ZeroMemory(vertices, sizeof(vertices[0]) * verticesNum);
	saveVertex.resize(verticesNum);
	for (UINT j = 0; j < attributeNum; ++j)
	{
		if (quadrangleIndexNum > 0)
		{
			// 4���_����3���_�ɕϊ�(4���_��2�ʂ�N/T�̕�)
			for (UINT i = _attribute[j].quadrangleStartIndex;
				i < _attribute[j].quadrangleStartIndex + _attribute[j].quadrangleIndexNum;
				i += (4 * polygonSize), entryPos += (2 * polygonSize))
			{
				// ���_
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

				// ModelMeshCollider����邽�߂ɕK�v
				// �G���[������p
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

				// �@��(�Čv�Z:flat)
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

				// �e�N�X�`���[UV�l
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
				// ���_
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

				// ModelMeshCollider����邽�߂ɕK�v
				// �G���[������p
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

				// �@��(�Čv�Z)
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

				// �e�N�X�`���[UV�l
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
	resDesc.Width = sizeof(vertices[0]) * verticesNum;
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
		IID_PPV_ARGS(mesh->vertexBuffer .ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// ���_���̃R�s�[
	VERTICES* vertexMap = nullptr;
	// ���_�o�b�t�@�[�̃��b�N
	hr = mesh->vertexBuffer->Map(0, nullptr,
		(void**)&vertexMap);
	if (hr != S_OK)
		return hr;
	memcpy(vertexMap, vertices,
		sizeof(vertices[0]) * verticesNum);
	// ���_�o�b�t�@�[�̉���
	mesh->vertexBuffer->Unmap(0, nullptr);

	// ���_�o�b�t�@�[�r���[���쐬
	// ���_�o�b�t�@�[�̃A�h���X[�o�b�t�@�[�̉��z�A�h���X]
	mesh->vertexBufferView.BufferLocation = mesh->vertexBuffer->GetGPUVirtualAddress();
	// ���o�C�g��[�S�o�C�g��]
	mesh->vertexBufferView.SizeInBytes = sizeof(vertices[0]) * verticesNum;
	// 1���_������̃o�C�g��
	mesh->vertexBufferView.StrideInBytes = sizeof(vertices[0]);

	delete[] vertices;

	// �C���f�b�N�X�����쐬
	UINT indicesNum = verticesNum;
	unsigned short* indices = new unsigned short[indicesNum];
	::ZeroMemory(indices, sizeof(indices[0]) * indicesNum);
	for (UINT i = 0; i < indicesNum; ++i)
	{
		indices[i] = (unsigned short)i;
	}

	// �T�C�Y��[���őS���܂��Ȃ��̂�sizeof(�S�C���f�b�N�X)�Ƃ���]
	resDesc.Width = sizeof(indices[0]) * indicesNum;

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
	memcpy(indexMap, indices, sizeof(indices[0]) * indicesNum);
	// �C���f�b�N�X�o�b�t�@�[�̉���
	mesh->indexBuffer->Unmap(0, nullptr);

	// �C���f�b�N�X�o�b�t�@�[�r���[���쐬
	// �C���f�b�N�X�o�b�t�@�[�̃A�h���X[�o�b�t�@�[�̉��z�A�h���X]
	mesh->indexBufferView.BufferLocation = mesh->indexBuffer->GetGPUVirtualAddress();
	// ���o�C�g��[�S�o�C�g��]
	mesh->indexBufferView.SizeInBytes = sizeof(indices[0]) * indicesNum;
	// �t�H�[�}�b�g
	mesh->indexBufferView.Format = DXGI_FORMAT_R16_UINT;

	delete[] indices;

	// �}�e���A���o�b�t�@�[�̐���
	if (FAILED(CreateMaterialBuffer(mesh)))
		return E_FAIL;

	// �e�N�X�`���[�̓o�^
	// �}�e���A�������e�N�X�`���[�ԍ���ݒ�
	// �A���A�f�B�t���[�Y�e�N�X�`���[�̂ݓo�^
	_fileTextures.resize(_material.size());
	mesh->textureBuffer.resize(_material.size());
	entryPos = 0;
	for (std::vector<MATERIAL>::size_type i = 0;
		i < _material.size(); ++i, ++entryPos)
	{
		// �o�^����Ă��邩�m�F
		BOOL entryCheck = FALSE;	// �o�^�̗L��
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
			// �p�X�ƃt�@�C����������
			TCHAR fillPathFilename[MAX_PATH] = _T("");
			if (lstrlenW(_material[i].textureNameKd) > 0)
			{
				lstrcpy(_fileTextures[entryPos].filename, _material[i].textureNameKd);
				lstrcpy(fillPathFilename, _path);
				lstrcat(fillPathFilename, _fileTextures[entryPos].filename);
			}

			// �V�F�[�_���\�[�X�̐���
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
	// �ŏ���������500�Ԗڂ܂Ŏg���؂�����A�܂��͏��߂Ă�filename�Ȃ玟�̃o�b�t�@�[��p�ӂ���
	if (num == 0 && entryPos == -1)
	{
		_objMeshs[filename].push_back(new MESH);

		// �t�@�C���ǂݍ���
		LoadObjFromFile(filename);
		// �C���f�b�N�X�o�[�e�b�N�X�o�b�t�@�[�r���[�̐���
		Convert(_objMeshs[filename].back(), _saveVertices[filename]);
		// �R���X�^���g�o�b�t�@�[�r���[�̐���
		CreateConstantBufferView(_objMeshs[filename].back());

		_objMetaData[filename].resize(_objMeshs[filename].size());
		_objDataCheck[filename].resize(_objMeshs[filename].size());

		// �����ڂ�
		_saveAttribute[filename] = _attribute;
		_saveMaterial[filename] = _material;
		// ������
		_position.clear();	// ���_���W
		_normal.clear();	// �@��
		_textureUV.clear();	// �e�N�X�`���[UV�l
		_triangleIndex.clear();		// �O�p�`�C���f�b�N�X
		_quadrangleIndex.clear();	// �l�p�`�C���f�b�N�X
		_attribute.clear();	// �������
		_material.clear();	// �}�e���A�����		
		_attributeEntryFlag = FALSE;	// �������o�^�̗L��
		_materialEntryFlag = FALSE;		// �}�e���A�����o�^�̗L��
		delete _path;					// �p�X(�t�H���_�[)

	}

	// �G���g���[�ԍ�
	int EntryPos;
	// �g�p���郁�^�f�[�^�̔ԍ�
	int ArrayNum;
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
		// �������郉�C�������
		_objMetaData[filename][ArrayNum].resize(_saveMaterial[filename].size());
		for (std::vector<MATERIAL>::size_type k = 0, size = (int)_saveMaterial[filename].size();
			k < size; ++k)
		{
			_objMetaData[filename][ArrayNum][k].world[EntryPos] = world_matrix;
			_objMetaData[filename][ArrayNum][k].diffuse[EntryPos] = _saveMaterial[filename][k].Kd;
			// ����Ȃ��������Ȃ牼�ɂP
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
			// ����Ȃ��������Ȃ牼��32
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
		// �������郉�C�������
		_objMetaData[filename][ArrayNum].resize(_saveMaterial[filename].size());
		for (std::vector<MATERIAL>::size_type k = 0, size = (int)_saveMaterial[filename].size();
			k < size; ++k)
		{
			_objMetaData[filename][ArrayNum][k].world[EntryPos] = world_matrix;
			_objMetaData[filename][ArrayNum][k].diffuse[EntryPos] = _saveMaterial[filename][k].Kd;
			// ����Ȃ��������Ȃ牼�ɂP
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
			// ����Ȃ��������Ȃ牼��32
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