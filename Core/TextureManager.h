#pragma once
#include <d3dx12.h>
#include "ErrorException.h"
using namespace Microsoft::WRL;

class TextureManager
{
public:
	// �f�X�N���v�^�[�̐�
	static const size_t kNumDescriptors = 256;

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
private:

	//�f�o�C�X
	ComPtr<ID3D12Device> dev;

	//�f�X�N���v�^�q�[�v
	ComPtr <ID3D12DescriptorHeap> srvHeap;

	// �q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp{};

	// �e�N�X�`���R���e�i
	std::array<Texture, kNumDescriptors> textures_;
	TextureManager() = default;
	~TextureManager() = default;

	
public:



	/// <summary>
	/// �V���O���g���C���X�^���X�̎擾
	/// </summary>
	/// <returns>�V���O���g���C���X�^���X</returns>
	static TextureManager* GetInstance();

	void Initalize();

	/// <returns>�e�N�X�`���n���h��</returns>
	static uint32_t Load(const std::string& fileName);

};