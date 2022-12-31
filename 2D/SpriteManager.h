#pragma once

#include <array>
#include <d3dx12.h>
#include <string>
#include <unordered_map>
#include <wrl.h>

/// <summary>
/// �X�v���C�g�}�l�[�W��
/// </summary>
class SpriteManager {
public:
	// �f�X�N���v�^�[�̐�
	static const size_t kNumDescriptors = 256;

	/// <summary>
	/// �e�N�X�`��
	/// </summary>
	struct Texture {
		// �e�N�X�`�����\�[�X
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
		// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
		// ���O
		std::string name;
	};

	/// <summary>
	/// �ǂݍ���
	/// </summary>
	/// <param name="fileName">�t�@�C����</param>
	/// <returns>�e�N�X�`���n���h��</returns>
	static uint32_t Load(const std::string& fileName);

	/// <summary>
	/// �V���O���g���C���X�^���X�̎擾
	/// </summary>
	/// <returns>�V���O���g���C���X�^���X</returns>
	static SpriteManager* GetInstance();

	/// <summary>
	/// �V�X�e��������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	void Initialize(ID3D12Device* device, std::string directoryPath = "Resources/");

	/// <summary>
	/// �S�e�N�X�`�����Z�b�g
	/// </summary>
	void ResetAll();

	/// <summary>
	/// ���\�[�X���擾
	/// </summary>
	/// <param name="textureHandle">�e�N�X�`���n���h��</param>
	/// <returns>���\�[�X���</returns>
	const D3D12_RESOURCE_DESC GetResoureDesc(uint32_t textureHandle);

	/// <summary>
	/// �f�X�N���v�^�e�[�u�����Z�b�g
	/// </summary>
	/// <param name="commandList">�R�}���h���X�g</param>
	/// <param name="rootParamIndex">���[�g�p�����[�^�ԍ�</param>
	/// <param name="textureHandle">�e�N�X�`���n���h��</param>
	void SetGraphicsRootDescriptorTable(
		ID3D12GraphicsCommandList* commandList, UINT rootParamIndex, uint32_t textureHandle);

private:
	SpriteManager() = default;
	~SpriteManager() = default;
	SpriteManager(const SpriteManager&) = delete;
	SpriteManager& operator=(const SpriteManager&) = delete;

	// �f�o�C�X
	ID3D12Device* device_;
	// �f�X�N���v�^�T�C�Y
	UINT sDescriptorHandleIncrementSize_ = 0u;
	// �f�B���N�g���p�X
	std::string directoryPath_;
	// �f�X�N���v�^�q�[�v
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
	// ���Ɏg���f�X�N���v�^�q�[�v�̔ԍ�
	uint32_t indexNextDescriptorHeap_ = 0u;
	// �e�N�X�`���R���e�i
	std::array<Texture, kNumDescriptors> textures_;

	/// <summary>
	/// �ǂݍ���
	/// </summary>
	/// <param name="fileName">�t�@�C����</param>
	uint32_t LoadInternal(const std::string& fileName);
};
