#include "SpriteManager.h"
#include <DirectXTex.h>
#include <cassert>

using namespace DirectX;

uint32_t SpriteManager::Load(const std::string& fileName) 
{
	return SpriteManager::GetInstance()->LoadInternal(fileName);
}

SpriteManager* SpriteManager::GetInstance() 
{
	static SpriteManager instance;
	return &instance;
}

void SpriteManager::Initialize(ID3D12Device* device, std::string directoryPath) 
{
	assert(device);

	device_ = device;
	directoryPath_ = directoryPath;

	// �f�X�N���v�^�T�C�Y���擾
	sDescriptorHandleIncrementSize_ =
		device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// �S�e�N�X�`�����Z�b�g
	ResetAll();
}

void SpriteManager::ResetAll() 
{
	HRESULT result = S_FALSE;

	// �f�X�N���v�^�q�[�v�𐶐�
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // �V�F�[�_���猩����悤��
	descHeapDesc.NumDescriptors = kNumDescriptors; // �V�F�[�_�[���\�[�X�r���[1��
	result = device_->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descriptorHeap_)); // ����
	assert(SUCCEEDED(result));

	indexNextDescriptorHeap_ = 0;

	// �S�e�N�X�`����������
	for (size_t i = 0; i < kNumDescriptors; i++) {
		textures_[i].resource.Reset();
		textures_[i].cpuDescHandleSRV.ptr = 0;
		textures_[i].gpuDescHandleSRV.ptr = 0;
		textures_[i].name.clear();
	}
}

const D3D12_RESOURCE_DESC SpriteManager::GetResoureDesc(uint32_t textureHandle) 
{

	assert(textureHandle < textures_.size());
	Texture& texture = textures_.at(textureHandle);
	return texture.resource->GetDesc();
}

void SpriteManager::SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* commandList, UINT rootParamIndex,uint32_t textureHandle) 
{ // �f�X�N���v�^�q�[�v�̔z��
	assert(textureHandle < textures_.size());
	ID3D12DescriptorHeap* ppHeaps[] = { descriptorHeap_.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// �V�F�[�_���\�[�X�r���[���Z�b�g
	commandList->SetGraphicsRootDescriptorTable(
		rootParamIndex, textures_[textureHandle].gpuDescHandleSRV);
}

uint32_t SpriteManager::LoadInternal(const std::string& fileName) 
{

	assert(indexNextDescriptorHeap_ < kNumDescriptors);
	uint32_t handle = indexNextDescriptorHeap_;

	// �ǂݍ��ݍς݃e�N�X�`��������
	auto it = std::find_if(textures_.begin(), textures_.end(), [&](const auto& texture) {
		return texture.name == fileName;
		});
	if (it != textures_.end()) {
		// �ǂݍ��ݍς݃e�N�X�`���̗v�f�ԍ����擾
		handle = static_cast<uint32_t>(std::distance(textures_.begin(), it));
		return handle;
	}

	// �������ރe�N�X�`���̎Q��
	Texture& texture = textures_.at(handle);
	texture.name = fileName;

	// �f�B���N�g���p�X�ƃt�@�C������A�����ăt���p�X�𓾂�
	bool currentRelative = false;
	if (2 < fileName.size()) {
		currentRelative = (fileName[0] == '.') && (fileName[1] == '/');
	}
	std::string fullPath = currentRelative ? fileName : directoryPath_ + fileName;

	// ���j�R�[�h������ɕϊ�
	wchar_t wfilePath[256];
	MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wfilePath, _countof(wfilePath));

	HRESULT result;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// WIC�e�N�X�`���̃��[�h
	result = LoadFromWICFile(wfilePath, WIC_FLAGS_NONE, &metadata, scratchImg);
	assert(SUCCEEDED(result));


	ScratchImage mipChain{};
	// �~�b�v�}�b�v����
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	// �ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata.format = MakeSRGB(metadata.format);

	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format, metadata.width, (UINT)metadata.height, (UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels);

	// �q�[�v�v���p�e�B
	CD3DX12_HEAP_PROPERTIES heapProps =
		CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);

	// �e�N�X�`���p�o�b�t�@�̐���
	result = device_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // �e�N�X�`���p�w��
		nullptr, IID_PPV_ARGS(&texture.resource));
	assert(SUCCEEDED(result));

	// �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	for (size_t i = 0; i < metadata.mipLevels; i++) {
		const Image* img = scratchImg.GetImage(i, 0, 0); // ���f�[�^���o
		result = texture.resource->WriteToSubresource(
			(UINT)i,
			nullptr,              // �S�̈�փR�s�[
			img->pixels,          // ���f�[�^�A�h���X
			(UINT)img->rowPitch,  // 1���C���T�C�Y
			(UINT)img->slicePitch // 1���T�C�Y
		);
		assert(SUCCEEDED(result));
	}

	// �V�F�[�_���\�[�X�r���[�쐬
	texture.cpuDescHandleSRV = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		descriptorHeap_->GetCPUDescriptorHandleForHeapStart(), handle, sDescriptorHandleIncrementSize_);
	texture.gpuDescHandleSRV = CD3DX12_GPU_DESCRIPTOR_HANDLE(
		descriptorHeap_->GetGPUDescriptorHandleForHeapStart(), handle, sDescriptorHandleIncrementSize_);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // �ݒ�\����
	D3D12_RESOURCE_DESC resDesc = texture.resource->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = (UINT)metadata.mipLevels;

	device_->CreateShaderResourceView(
		texture.resource.Get(), //�r���[�Ɗ֘A�t����o�b�t�@
		&srvDesc,               //�e�N�X�`���ݒ���
		texture.cpuDescHandleSRV);

	indexNextDescriptorHeap_++;

	return handle;
}
