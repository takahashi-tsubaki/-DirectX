#include "SpriteManager.h"
#include "../Core/WinApp.h"
SpriteManager* SpriteManager::GetInstance()
{
	static SpriteManager instance;
	return &instance;
}
void SpriteManager::Initialize(DirectXCommon* dxCommon)
{
	HRESULT result;
	dxCommon_ = dxCommon;



#pragma region �s��


	
#pragma endregion

#pragma region �e�N�X�`���֘A
	

	//�f�X�N���v�^�����W�̐ݒ�
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
#pragma endregion

	//���[�g�p�����[�^
	D3D12_ROOT_PARAMETER rootParams[3] = {};
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//�萔�o�b�t�@�r���[
	rootParams[0].Descriptor.ShaderRegister = 0;//�萔�o�b�t�@�ԍ�
	rootParams[0].Descriptor.RegisterSpace = 0;//�f�t�H���g�l
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����

	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//���
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;//�f�X�N���v�^�����W
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;//�f�X�N���v�^�����W��
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����

	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//�萔�o�b�t�@�r���[
	rootParams[2].Descriptor.ShaderRegister = 1;//�萔�o�b�t�@�ԍ�
	rootParams[2].Descriptor.RegisterSpace = 0;//�f�t�H���g�l
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_���猩����

	//�e�N�X�`���T���v���[�̐ݒ�
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//���J��Ԃ�
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//�c�J��Ԃ�
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//���s�J��Ԃ�
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//�{�[�_�[�̎��͍�
	samplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;//�S�ă��j�A���
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;//�~�b�v�}�b�v�ő�l
	samplerDesc.MinLOD = 0.0f;//�~�b�v�}�b�v�ŏ��l
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//�s�N�Z���V�F�[�_����̂ݎg�p�\



	ComPtr<ID3DBlob> vsBlob = nullptr;//���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob = nullptr;//�s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob = nullptr;//�G���[�I�u�W�F�N�g

	//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shaders/SpriteVS.hlsl",//�V�F�[�_�[�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h���\�ɂ���
		"main",//�G���g���[�|�C���g
		"vs_5_0",//�V�F�[�_���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�N�p�ݒ�
		0,
		&vsBlob, &errorBlob);

	////�R���p�C���G���[�Ȃ�
	//if (FAILED(result))
	//{
	//	//errorBlob����G���[���e��stirng�^�ɃR�s�[
	//	std::string error;
	//	error.resize(errorBlob->GetBufferSize());

	//	std::copy_n((char*)errorBlob->GetBufferPointer(),
	//		errorBlob->GetBufferPointer(),
	//		error.begin());
	//	error += "\n";
	//	//�G���[���e���o�̓E�B���h�E�ɕ\��
	//	OutputDebugStringA(error.c_str());
	//	assert(0);
	//}

	//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shaders/SpritePS.hlsl",//�V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob);

	////�R���p�C���G���[�Ȃ�
	//if (FAILED(result))
	//{
	//	//errorBlob����G���[���e��stirng�^�ɃR�s�[
	//	std::string error;
	//	error.resize(errorBlob->GetBufferSize());

	//	std::copy_n((char*)errorBlob->GetBufferPointer(),
	//		errorBlob->GetBufferPointer(),
	//		error.begin());
	//	error += "\n";
	//	//�G���[���e���o�̓E�B���h�E�ɕ\��
	//	OutputDebugStringA(error.c_str());
	//	assert(0);
	//}


	//���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{
			//xyz���W
			"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0,
		},
		{
			//uv���W
			"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0,
		},
	};

	//�O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	//�V�F�[�_�̐ݒ�
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	//�T���v���}�X�N�̐ݒ�
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//�W���ݒ�

	//���X�^���C�U�̐ݒ�
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//�J�����O���Ȃ�
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//�|���S�����h��Ԃ�
	pipelineDesc.RasterizerState.DepthClipEnable = true;//�[�x�N���b�s���O��L����

	//�u�����h�X�e�[�g
	pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;//RGBA�S�Ẵ`�����l����`��

	//���_���C�A�E�g�̐ݒ�
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	//�}�`�̌`��ݒ�
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//���̑��̐ݒ�
	pipelineDesc.NumRenderTargets = 1;//�`��Ώۂ�1��
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0~255�w���RGBA
	pipelineDesc.SampleDesc.Count = 1;//1�s�N�Z���ɂ�1�񃌃��_�����O



	//���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;
	rootSignatureDesc.NumParameters = _countof(rootParams);

	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	//���[�g�V�O�l�`���̃V���A���C�Y
	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = dxCommon_->GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();

	//�p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	pipelineDesc.pRootSignature = rootSignature;

	//�p�C�v���C���X�e�[�g�̐���
	result = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));

}

void SpriteManager::Update()
{

	
}


