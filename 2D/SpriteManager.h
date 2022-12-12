#pragma once
#include "../Core/DirectXCommon.h"
class SpriteManager
{
private:
	//�萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferDataMaterial
	{
		XMFLOAT4 color;//�F
	};
	//���_�f�[�^�\����
	struct Vertex
	{
		XMFLOAT3 pos;//xyz���W
		XMFLOAT2 uv;//uv���W
	};

	static const int vertexCount = 4;

	//���_�f�[�^
	std::array<Vertex, vertexCount> vertices;

	//�������s�N�Z����
	const size_t textureWidth = 256;
	//�c�����s�N�Z����
	const size_t textureHeight = 256;
	//�z��̗v�f��
	const size_t imageDataCount = textureWidth * textureHeight;
	//�摜�C���[�W�f�[�^�z��
	XMFLOAT4* imageData = new XMFLOAT4[imageDataCount];

	//���_�o�b�t�@�[�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	//�萔�o�b�t�@�̐���
	ID3D12Resource* constBuffMaterial = nullptr;

	//�ݒ������SRV�p�f�X�N���v�^�q�[�v�𐶐�
	ID3D12DescriptorHeap* srvHeap = nullptr;

	//���[�g�V�O�l�`��
	ID3D12RootSignature* rootSignature;
	//�p�C�v���C���X�e�[�g
	ID3D12PipelineState* pipelineState = nullptr;

	//SRV�q�[�v�̐擪�n���h�����擾
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;

	//�X�v���C�g�p�R�}���h���X�g
	ID3D12GraphicsCommandList* sCommandList;

	DirectXCommon* dxCommon_ = nullptr;

public:
	void Initialize(DirectXCommon*dxCommon);

	void Update();

	void Draw();
};