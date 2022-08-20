// --------------------- Include File --------------------
#include "Field.h"
#include "../Camera/Camera.h"
#include "../DX12System.h"
Field* Field::_instance = nullptr;			// �C���X�^���X�̎���
// �R���X�g���N�^�[
Field::Field()
	: _vertexBufferView(), _indexBufferView()
{

}
// �f�X�g���N�^�[
Field::~Field()
{

}
// ����
void Field::Create()
{
	if (!_instance)
		_instance = new Field();

	// ���[�g�V�O�l�`���[�̐���
	_instance->CreateRootSignature();

	// �`��p�C�v���C���̐���
	_instance->CreateGraphicsPipelineState();

	// ���_/�C���f�N�X�o�b�t�@�[�r���[�̐���
	_instance->CreateVertexIndexBufferView();

	// �R���X�^���g�o�b�t�@�[�r���[�̐���
	_instance->CreateConstantBufferView();

}
// �j��
void Field::Destroy()
{
	delete _instance;
}

// ���[�g�V�O�l�`���[�̐���
HRESULT Field::CreateRootSignature()
{
	HRESULT hr = S_OK;

	// �f�B�X�N���v�^�[�����W�̐ݒ�
	D3D12_DESCRIPTOR_RANGE descTblRange[1] = {};
	// �f�B�X�N���v�^�[�̐�[�R���X�^���g�o�b�t�@�[]
	descTblRange[0].NumDescriptors = 1;
	// ���[�萔]
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// �擪���W�X�^�[�ԍ�[0�ԃX���b�g����]
	descTblRange[0].BaseShaderRegister = 0;
	// �A�������f�B�X�N���v�^�[�����W���O�̃f�B�X�N���v�^�[�����W�̒���ɗ���
	descTblRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ���[�g�p�����[�^�[�̐ݒ�
	D3D12_ROOT_PARAMETER rootParam[1] = {};
	// ���[�g�p�����[�^�[�̎��[����̓f�B�X�N���v�^�[�e�[�u���Ƃ��ė��p]
	rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// �ǂ̃V�F�[�_�[���痘�p�\��[�S�V�F�[�_�[]
	rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// �f�B�X�N���v�^�[�����W�̃A�h���X
	rootParam[0].DescriptorTable.pDescriptorRanges = &descTblRange[0];
	// �f�B�X�N���v�^�[�����W�̐�
	rootParam[0].DescriptorTable.NumDescriptorRanges = 1;

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
	rootSignatureDesc.NumParameters = 1;
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

	return hr;
}
// �`��p�C�v���C���̐���
HRESULT Field::CreateGraphicsPipelineState()
{
	HRESULT hr = S_OK;

	// �V�F�[�_�[�I�u�W�F�N�g�ǂݍ��ݗp
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// ���_�V�F�[�_�[�̓ǂݍ���
	hr = D3DCompileFromFile(
		L".\\Shader\\Field\\FieldVertexShader.hlsl",	// �V�F�[�_�[��
		nullptr,							// define�͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// �C���N���[�h�̓f�t�H���g
		"FieldVS", "vs_5_0",				// �֐��� BasicVS�A�ΏۃV�F�[�_�[�� vs_5_0
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
		L".\\Shader\\Field\\FieldPixelShader.hlsl",	// �V�F�[�_�[��
		nullptr,							// define�͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// �C���N���[�h�̓f�t�H���g
		"FieldPS", "ps_5_0",				// �֐��� BasicPS�A�ΏۃV�F�[�_�[�� ps_5_0
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
		{	// color���
			"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
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

	// �v���~�e�B�u�̃^�C�v[���C��]
	gpipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

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

	// �[�x�o�b�t�@�[�̐ݒ�
	gpipelineDesc.DepthStencilState.DepthEnable = TRUE;	// �[�x�o�b�t�@�[���g��
	gpipelineDesc.DepthStencilState.DepthWriteMask
		= D3D12_DEPTH_WRITE_MASK_ALL;	// ��������
	gpipelineDesc.DepthStencilState.DepthFunc =
		D3D12_COMPARISON_FUNC_LESS;		// �������ق����̗p
	gpipelineDesc.DSVFormat =
		DXGI_FORMAT_D32_FLOAT;	// 32bit float��[�x�l�Ƃ��Ďg�p

	// �O���t�B�b�N�p�C�v���C���X�e�[�g�I�u�W�F�N�g�𐶐�
	hr = DX12::GetInstance()->GetDevice()->CreateGraphicsPipelineState(
		&gpipelineDesc, IID_PPV_ARGS(_pipelineState.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	return hr;
}
// ���_/�C���f�b�N�X�o�b�t�@�[�r���[�̐���
HRESULT Field::CreateVertexIndexBufferView()
{
	HRESULT hr = S_OK;

	const float gridSize = 1.0f;// �O���b�h�T�C�Y
	const int divideNum = 99;	// ������(�)

	// ���_�����쐬
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

		// ������Z�q
		VECTOR4& operator += (CONST VECTOR4& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
			w = 1.0f;
			return *this;
		}
		// 2 �����Z�q
		VECTOR4 operator + (CONST VECTOR4& v) const
		{
			return VECTOR4(x + v.x, y + v.y, z + v.z, 1.0f);
		}
	};

	// ���_�o�b�t�@����
	FIELDVERTEX vertices[(divideNum + 2) * 2 * 2] = {};
	// ���_�𒆐S�Ƃ���XZ���ʏ�Ɋi�q��̘g����
	UINT entryPos = 0;
	// ���`X��(��->�E)�𐶐�
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

	// ��`Y��(��->��O)�𐶐�
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

	// ���_�C���f�N�X�����쐬
	for (int i = 0; i < (divideNum + 2) * 2 * 2; ++i)
	{
		_indices.push_back((unsigned short)i);
	}

	// ���_�o�b�t�@�[
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
	resDesc.Width = _vertices.size() * sizeof(_vertices[0]);
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
		IID_PPV_ARGS(_vertexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// ���\�[�X�I�u�W�F�N�g�ɒ��_�����R�s�[
	FIELDVERTEX* vertexMap = nullptr;
	// �o�b�t�@�[�̃A�h���X���擾���A���b�N[�S�͈͎w��]
	hr = _vertexBuffer->Map(0, nullptr, (void**)&vertexMap);
	if (FAILED(hr))
		return hr;
	// �R�s�[
	std::copy(std::begin(_vertices), std::end(_vertices), vertexMap);
	// ���b�N����
	_vertexBuffer->Unmap(0, nullptr);

	// ���_�o�b�t�@�[�r���[�̐ݒ�
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	// ���_�o�b�t�@�[�̃A�h���X
	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
	// �P���_������̃o�C�g��
	_vertexBufferView.StrideInBytes = (UINT)sizeof(_vertices[0]);	// ���_�P������̃T�C�Y
	// ���o�C�g��
	_vertexBufferView.SizeInBytes = (UINT)(_vertices.size() * sizeof(_vertices[0]));	// �S�o�C�g��

	// �C���f�b�N�X�o�b�t�@�[
	// ���\�[�X�T�C�Y�ύX
	resDesc.Width = _indices.size() * sizeof(_indices[0]);
	// ���\�[�X�I�u�W�F�N�g���쐬
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_indexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// ���\�[�X�I�u�W�F�N�g�ɃC���f�b�N�X�����R�s�[
	unsigned short* mappedIdx = nullptr;
	// �o�b�t�@�[�̃A�h���X���擾���A���b�N[�S�͈͎w��]
	hr = _indexBuffer->Map(0, nullptr, (void**)&mappedIdx);
	if (FAILED(hr))
		return hr;
	// �R�s�[
	std::copy(std::begin(_indices), std::end(_indices), mappedIdx);
	// ���b�N����
	_indexBuffer->Unmap(0, nullptr);

	// �C���f�b�N�X�o�b�t�@�[�r���[�̐ݒ�
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	// �C���f�b�N�X�o�b�t�@�[�̃A�h���X
	_indexBufferView.BufferLocation = _indexBuffer->GetGPUVirtualAddress();
	// �t�H�[�}�b�g
	_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	// �S�o�C�g��
	_indexBufferView.SizeInBytes = (UINT)(_indices.size() * sizeof(_indices[0]));

	return hr;
}
// �R���X�^���g�o�b�t�@�[�r���[�̐���
HRESULT Field::CreateConstantBufferView()
{
	HRESULT hr = S_OK;

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
	resDesc.Width = (sizeof(SceneMatrix) + 0xff) & ~0xff;
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

	// �R���X�^���g�o�b�t�@�[���쐬
	hr = DX12::GetInstance()->GetDevice()->CreateCommittedResource(
		&uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,	// ���Ɏw��Ȃ�
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,	// CPU����̏������݉\�AGPU���猩��Ɠǂݎ��̂�
		nullptr,
		IID_PPV_ARGS(_constBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �f�X�N���v�^�q�[�v���쐬
	// �q�[�v�̋L�q��p��
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	// �t���O[�V�F�[�_�[�����猩����悤��]
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// GPU���P�����g�p����z��
	descHeapDesc.NodeMask = 0;
	// �f�X�N���v�^�q�[�v�̐�[CBV1��]
	descHeapDesc.NumDescriptors = 1;
	// ���[�V�F�[�_�[���\�[�X�r���[�p]
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	// �L�q�q�q�[�v���쐬
	hr = DX12::GetInstance()->GetDevice()->CreateDescriptorHeap(&descHeapDesc
		, IID_PPV_ARGS(_basicDescHeap.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
		return hr;

	// �f�B�X�N���v�^�̐擪�n���h�����擾���Ă���
	auto basicHeapHandle = _basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = _constBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)_constBuffer->GetDesc().Width;

	// �萔�o�b�t�@�[�r���[�̍쐬
	DX12::GetInstance()->GetDevice()->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	return hr;
}
// �`��
void Field::Draw()
{
	XMMATRIX worldMat = XMMatrixIdentity();

	// �r���[�s��
	XMMATRIX viewMat = Camera::GetInstance()->GetView();

	// �v���W�F�N�V�����s��
	XMMATRIX projectionMat = Camera::GetInstance()->GetProjection();

	// �J�������W
	XMFLOAT3 eyePosition = Camera::GetInstance()->GetEyePosition();

	SceneMatrix* mapMatrix = nullptr;	// �}�b�v��������|�C���^�[
	HRESULT hr = _constBuffer->Map(0, nullptr, (void**)&mapMatrix);	// �}�b�v
	// �s��̓��e���R�s�[
	mapMatrix->world = worldMat;
	mapMatrix->view = viewMat;
	mapMatrix->projection = projectionMat;
	mapMatrix->eye = eyePosition;

	// �O���t�B�b�N�X�p�C�v���C���X�e�[�g�̎w��
	DX12::GetInstance()->CommandList()->SetPipelineState(_pipelineState.Get());
	// ���[�g�V�O�l�`���[�̎w��
	DX12::GetInstance()->CommandList()->SetGraphicsRootSignature(_rootSignature.Get());


	// �f�B�X�N���v�^�[�q�[�v�̎w��
	DX12::GetInstance()->CommandList()->SetDescriptorHeaps(1, _basicDescHeap.GetAddressOf());

	// ���[�g�p�����[�^�[�ƃf�B�X�N���v�^�[�q�[�v�̊֘A�t��
	DX12::GetInstance()->CommandList()->SetGraphicsRootDescriptorTable(
		0,	// ���[�g�p�����[�^�[�C���f�b�N�X
		_basicDescHeap->GetGPUDescriptorHandleForHeapStart());    // �q�[�v�A�h���X

	// ���_���̎w��[���C��]
	DX12::GetInstance()->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	// �`��(Draw�֐�)����
	// ���_�o�b�t�@�[�̎w��
	DX12::GetInstance()->CommandList()->IASetVertexBuffers(0, 1, &_vertexBufferView);
	// �C���f�b�N�X�o�b�t�@�[�̎w��
	DX12::GetInstance()->CommandList()->IASetIndexBuffer(&_indexBufferView);

	// �ŏ��̓I�t�Z�b�g�Ȃ�
	unsigned int idxOffset = 0;

	// �`�施��
	DX12::GetInstance()->CommandList()->DrawIndexedInstanced((UINT)_indices.size(), 1, idxOffset, 0, 0);
}