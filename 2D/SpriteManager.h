#pragma once
#include "../Core/DirectXCommon.h"
class SpriteManager
{
private:

	

	//�������s�N�Z����
	const size_t textureWidth = 256;
	//�c�����s�N�Z����
	const size_t textureHeight = 256;
	//�z��̗v�f��
	const size_t imageDataCount = textureWidth * textureHeight;
	//�摜�C���[�W�f�[�^�z��
	XMFLOAT4* imageData = new XMFLOAT4[imageDataCount];

	//���[�g�V�O�l�`��
	ID3D12RootSignature* rootSignature;
	//�p�C�v���C���X�e�[�g
	ID3D12PipelineState* pipelineState = nullptr;

	//�X�v���C�g�p�R�}���h���X�g
	ID3D12GraphicsCommandList* sCommandList;

	DirectXCommon* dxCommon_ = nullptr;

	

public:

	uint32_t Load(const std::string& fileName);

	static SpriteManager* GetInstance();

	DirectXCommon* GetDxCommon() { return dxCommon_; }

	ID3D12RootSignature* GetRootSignature() {return rootSignature;}

	ID3D12PipelineState* GetPipelineState() { return pipelineState; }

	void Initialize(DirectXCommon* dxCommon);

	void Update();
};