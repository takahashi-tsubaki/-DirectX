#include "../Core/DirectXCommon.h"
#include "Material.h"
//#include "TextureManager.h"
#include <DirectXTex.h>
#include <cassert>

using namespace DirectX;
using namespace std;

Material* Material::Create() {
	Material* instance = new Material;

	instance->Initialize();

	return instance;
}

void Material::Initialize() {
	// �萔�o�b�t�@�̐���
	CreateConstantBuffer();
}

void Material::CreateConstantBuffer() {
	HRESULT result;

	// �q�[�v�v���p�e�B
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff);

	// �萔�o�b�t�@�̐���
	result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));

	// �萔�o�b�t�@�Ƃ̃f�[�^�����N
	result = constBuff_->Map(0, nullptr, (void**)&constMap_);
	assert(SUCCEEDED(result));
}

void Material::LoadTexture(const std::string& directoryPath) {
	// �e�N�X�`���Ȃ�
	if (textureFilename_.size() == 0) {
		textureFilename_ = "white1x1.png";
	}

	HRESULT result = S_FALSE;

	// WIC�e�N�X�`���̃��[�h
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// �t�@�C���p�X������
	string filepath = directoryPath + textureFilename_;

	// �e�N�X�`���ǂݍ���
	/*textureHandle_ = TextureManager::Load(filepath);*/
}

void Material::Update() {
	// �萔�o�b�t�@�փf�[�^�]��
	constMap_->ambient = ambient_;
	constMap_->diffuse = diffuse_;
	constMap_->specular = specular_;
	constMap_->alpha = alpha_;
}

void Material::SetGraphicsCommand(
	ID3D12GraphicsCommandList* commandList, UINT rooParameterIndexMaterial,
	UINT rooParameterIndexTexture) {

	// SRV���Z�b�g
	/*TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(
		commandList, rooParameterIndexTexture, textureHandle_);*/

	// �}�e���A���̒萔�o�b�t�@���Z�b�g
	commandList->SetGraphicsRootConstantBufferView(
		rooParameterIndexMaterial, constBuff_->GetGPUVirtualAddress());
}

void Material::SetGraphicsCommand(
	ID3D12GraphicsCommandList* commandList, UINT rooParameterIndexMaterial,
	UINT rooParameterIndexTexture, uint32_t textureHandle) {

	// SRV���Z�b�g
	/*TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(
		commandList, rooParameterIndexTexture, textureHandle);*/

	// �}�e���A���̒萔�o�b�t�@���Z�b�g
	commandList->SetGraphicsRootConstantBufferView(
		rooParameterIndexMaterial, constBuff_->GetGPUVirtualAddress());
}