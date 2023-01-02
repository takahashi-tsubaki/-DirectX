#include "../math/MyMath.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include <cassert>
#include <d3dcompiler.h>
#include <d3dx12.h>

#pragma comment(lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;
using namespace MyMath;

/// <summary>
/// �ÓI�����o�ϐ��̎���
/// </summary>
ID3D12Device* Sprite::sDevice_ = nullptr;
UINT Sprite::sDescriptorHandleIncrementSize_;
ID3D12GraphicsCommandList* Sprite::sCommandList_ = nullptr;
ComPtr<ID3D12RootSignature> Sprite::sRootSignature_;
std::array<ComPtr<ID3D12PipelineState>, size_t(Sprite::BlendMode::kCountOfBlendMode)>
Sprite::sPipelineStates_;
Matrix4 Sprite::sMatProjection_;

void Sprite::StaticInitialize(
	ID3D12Device* device, int window_width, int window_height, const std::wstring& directoryPath) {
	// nullptr�`�F�b�N
	assert(device);

	sDevice_ = device;

	// �f�X�N���v�^�T�C�Y���擾
	sDescriptorHandleIncrementSize_ =
		sDevice_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob;    // ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob;    // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g

	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	std::wstring vsFile = directoryPath + L"/shaders/SpriteVS.hlsl";
	result = D3DCompileFromFile(
		vsFile.c_str(), // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0, &vsBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n(
			(char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	std::wstring psFile = directoryPath + L"/shaders/SpritePS.hlsl";
	result = D3DCompileFromFile(
		psFile.c_str(), // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0, &psBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n(
			(char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());

		exit(1);
	}

	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	  {// xy���W(1�s�ŏ������ق������₷��)
	   "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
	   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	  {// uv���W(1�s�ŏ������ق������₷��)
	   "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT,
	   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	// �O���t�B�b�N�X�p�C�v���C���̗����ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// �T���v���}�X�N
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
	// ���X�^���C�U�X�e�[�g
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	// �f�v�X�X�e���V���X�e�[�g
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS; // ��ɏ㏑�����[��

	// �[�x�o�b�t�@�̃t�H�[�}�b�g
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// ���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// �}�`�̌`��ݒ�i�O�p�`�j
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;                            // �`��Ώۂ�1��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0�`255�w���RGBA
	gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	// �f�X�N���v�^�����W
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 ���W�X�^

	// ���[�g�p�����[�^
	CD3DX12_ROOT_PARAMETER rootparams[2] = {};
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	// �X�^�e�B�b�N�T���v���[
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc =
		CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR); // s0 ���W�X�^
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	// ���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(
		_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// �o�[�W������������̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(
		&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	// ���[�g�V�O�l�`���̐���
	result = sDevice_->CreateRootSignature(
		0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&sRootSignature_));
	assert(SUCCEEDED(result));

	gpipeline.pRootSignature = sRootSignature_.Get();

	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�B�u�����h�Ȃ�
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA�S�Ẵ`�����l����`��
	blenddesc.BlendEnable = false;
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// �O���t�B�b�N�X�p�C�v���C���̐���
	result = sDevice_->CreateGraphicsPipelineState(
		&gpipeline, IID_PPV_ARGS(&sPipelineStates_[size_t(BlendMode::kNone)]));
	assert(SUCCEEDED(result));

	// �ʏ탿�u�����h
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	result = sDevice_->CreateGraphicsPipelineState(
		&gpipeline, IID_PPV_ARGS(&sPipelineStates_[size_t(BlendMode::kNormal)]));
	assert(SUCCEEDED(result));

	// ���Z
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_ONE;
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	result = sDevice_->CreateGraphicsPipelineState(
		&gpipeline, IID_PPV_ARGS(&sPipelineStates_[size_t(BlendMode::kAdd)]));
	assert(SUCCEEDED(result));

	// ���Z
	blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_ONE;
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	result = sDevice_->CreateGraphicsPipelineState(
		&gpipeline, IID_PPV_ARGS(&sPipelineStates_[size_t(BlendMode::kSubtract)]));
	assert(SUCCEEDED(result));

	// ��Z
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_ZERO;
	blenddesc.DestBlend = D3D12_BLEND_SRC_COLOR;
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	result = sDevice_->CreateGraphicsPipelineState(
		&gpipeline, IID_PPV_ARGS(&sPipelineStates_[size_t(BlendMode::kMultily)]));
	assert(SUCCEEDED(result));

	// �X�N���[��
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blenddesc.DestBlend = D3D12_BLEND_ONE;
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	result = sDevice_->CreateGraphicsPipelineState(
		&gpipeline, IID_PPV_ARGS(&sPipelineStates_[size_t(BlendMode::kScreen)]));
	assert(SUCCEEDED(result));

	// �ˉe�s��v�Z
	sMatProjection_ =
		Matrix4Orthographic(
			0.0f, (float)window_width, (float)window_height, 0.0f, 0.0f, 1.0f);
}

void Sprite::PreDraw(ID3D12GraphicsCommandList* commandList, BlendMode blendMode) {
	// PreDraw��PostDraw���y�A�ŌĂ΂�Ă��Ȃ���΃G���[
	assert(Sprite::sCommandList_ == nullptr);

	// �u�����h���[�h�ݒ肪�Ԉ���Ă�
	assert(0 <= size_t(blendMode) && size_t(blendMode) < size_t(BlendMode::kCountOfBlendMode));

	// �R�}���h���X�g���Z�b�g
	sCommandList_ = commandList;

	// �p�C�v���C���X�e�[�g�̐ݒ�
	sCommandList_->SetPipelineState(sPipelineStates_[size_t(blendMode)].Get());
	// ���[�g�V�O�l�`���̐ݒ�
	sCommandList_->SetGraphicsRootSignature(sRootSignature_.Get());
	// �v���~�e�B�u�`���ݒ�
	sCommandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void Sprite::PostDraw() {
	// �R�}���h���X�g������
	Sprite::sCommandList_ = nullptr;
}

Sprite* Sprite::Create(
	uint32_t textureHandle, Vector2 position, Vector4 color, Vector2 anchorpoint, bool isFlipX,
	bool isFlipY) {
	// ���T�C�Y
	Vector2 size = { 100.0f, 100.0f };

	{
		// �e�N�X�`�����擾
		const D3D12_RESOURCE_DESC& resDesc =
			SpriteManager::GetInstance()->GetResoureDesc(textureHandle);
		// �X�v���C�g�̃T�C�Y���e�N�X�`���̃T�C�Y�ɐݒ�
		size = { (float)resDesc.Width, (float)resDesc.Height };
	}

	// Sprite�̃C���X�^���X�𐶐�
	Sprite* sprite =
		new Sprite(textureHandle, position, size, color, anchorpoint, isFlipX, isFlipY);
	if (sprite == nullptr) {
		return nullptr;
	}

	// ������
	if (!sprite->Initialize()) {
		delete sprite;
		assert(0);
		return nullptr;
	}

	return sprite;
}

Sprite::Sprite() {}

Sprite::Sprite(
	uint32_t textureHandle, Vector2 position, Vector2 size, Vector4 color, Vector2 anchorpoint,
	bool isFlipX, bool isFlipY) {
	position_ = position;
	size_ = size;
	anchorPoint_ = anchorpoint;
	matWorld_ = Matrix4Identity();
	color_ = color;
	textureHandle_ = textureHandle;
	isFlipX_ = isFlipX;
	isFlipY_ = isFlipY;
	texSize_ = size;
}

bool Sprite::Initialize() {
	// nullptr�`�F�b�N
	assert(sDevice_);

	HRESULT result = S_FALSE;

	resourceDesc_ = SpriteManager::GetInstance()->GetResoureDesc(textureHandle_);

	{
		// �q�[�v�v���p�e�B
		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		// ���\�[�X�ݒ�
		CD3DX12_RESOURCE_DESC resourceDesc =
			CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexPosUv) * kVertNum);

		// ���_�o�b�t�@����
		result = sDevice_->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&vertBuff_));
		assert(SUCCEEDED(result));

		// ���_�o�b�t�@�}�b�s���O
		result = vertBuff_->Map(0, nullptr, (void**)&vertMap_);
		assert(SUCCEEDED(result));
	}

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	TransferVertices();

	// ���_�o�b�t�@�r���[�̍쐬
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeof(VertexPosUv) * 4;
	vbView_.StrideInBytes = sizeof(VertexPosUv);

	{
		// �q�[�v�v���p�e�B
		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		// ���\�[�X�ݒ�
		CD3DX12_RESOURCE_DESC resourceDesc =
			CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff);

		// �萔�o�b�t�@�̐���
		result = sDevice_->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&constBuff_));
		assert(SUCCEEDED(result));
	}

	// �萔�o�b�t�@�}�b�s���O
	result = constBuff_->Map(0, nullptr, (void**)&constMap_);
	assert(SUCCEEDED(result));

	return true;
}

void Sprite::SetTextureHandle(uint32_t textureHandle) {
	textureHandle_ = textureHandle;
	resourceDesc_ = SpriteManager::GetInstance()->GetResoureDesc(textureHandle_);
}

void Sprite::SetRotation(float rotation) {
	rotation_ = rotation;

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	TransferVertices();
}

void Sprite::SetPosition(const Vector2& position) {
	position_ = position;

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	TransferVertices();
}

void Sprite::SetSize(const Vector2& size) {
	size_ = size;

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	TransferVertices();
}

void Sprite::SetAnchorPoint(const Vector2& anchorpoint) {
	anchorPoint_ = anchorpoint;

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	TransferVertices();
}

void Sprite::SetIsFlipX(bool isFlipX) {
	isFlipX_ = isFlipX;

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	TransferVertices();
}

void Sprite::SetIsFlipY(bool isFlipY) {
	isFlipY_ = isFlipY;

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	TransferVertices();
}

void Sprite::SetTextureRect(const Vector2& texBase, const Vector2& texSize) {
	texBase_ = texBase;
	texSize_ = texSize;

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	TransferVertices();
}

void Sprite::Draw() {
	// ���[���h�s��̍X�V
	matWorld_ = Matrix4Identity();
	matWorld_ *= Matrix4RotationZ(rotation_);
	matWorld_ *= Matrix4Translation(position_.x, position_.y, 0.0f);

	// �萔�o�b�t�@�Ƀf�[�^�]��
	constMap_->color = color_;
	constMap_->mat = matWorld_ * sMatProjection_; // �s��̍���

	// ���_�o�b�t�@�̐ݒ�
	sCommandList_->IASetVertexBuffers(0, 1, &vbView_);

	// �萔�o�b�t�@�r���[���Z�b�g
	sCommandList_->SetGraphicsRootConstantBufferView(0, constBuff_->GetGPUVirtualAddress());
	// �V�F�[�_���\�[�X�r���[���Z�b�g
	SpriteManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList_, 1, textureHandle_);
	// �`��R�}���h
	sCommandList_->DrawInstanced(4, 1, 0, 0);
}

void Sprite::TransferVertices() {
	HRESULT result = S_FALSE;

	// �����A����A�E���A�E��
	enum { LB, LT, RB, RT };

	float left = (0.0f - anchorPoint_.x) * size_.x;
	float right = (1.0f - anchorPoint_.x) * size_.x;
	float top = (0.0f - anchorPoint_.y) * size_.y;
	float bottom = (1.0f - anchorPoint_.y) * size_.y;
	if (isFlipX_) { // ���E����ւ�
		left = -left;
		right = -right;
	}

	if (isFlipY_) { // �㉺����ւ�
		top = -top;
		bottom = -bottom;
	}

	// ���_�f�[�^
	VertexPosUv vertices[kVertNum];

	vertices[LB].pos = { left, bottom, 0.0f };  // ����
	vertices[LT].pos = { left, top, 0.0f };     // ����
	vertices[RB].pos = { right, bottom, 0.0f }; // �E��
	vertices[RT].pos = { right, top, 0.0f };    // �E��

	// �e�N�X�`�����擾
	{
		float tex_left = texBase_.x / resourceDesc_.Width;
		float tex_right = (texBase_.x + texSize_.x) / resourceDesc_.Width;
		float tex_top = texBase_.y / resourceDesc_.Height;
		float tex_bottom = (texBase_.y + texSize_.y) / resourceDesc_.Height;

		vertices[LB].uv = { tex_left, tex_bottom };  // ����
		vertices[LT].uv = { tex_left, tex_top };     // ����
		vertices[RB].uv = { tex_right, tex_bottom }; // �E��
		vertices[RT].uv = { tex_right, tex_top };    // �E��
	}

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	memcpy(vertMap_, vertices, sizeof(vertices));
}

