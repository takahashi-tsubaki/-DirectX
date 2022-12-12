#pragma once
#include "../Core/DirectXCommon.h"
class SpriteManager
{
private:

	//���_�f�[�^
	std::array<XMFLOAT3, 3> vertices;

	//���_�o�b�t�@�[�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	//���[�g�V�O�l�`��
	ID3D12RootSignature* rootSignature;
	//�p�C�v���C���X�e�[�g
	ID3D12PipelineState* pipelineState = nullptr;

	//�X�v���C�g�p�R�}���h���X�g
	ID3D12GraphicsCommandList* sCommandList;

	DirectXCommon* dxCommon_ = nullptr;

public:
	void Initialize(DirectXCommon*dxCommon);

	void Update();

	void Draw();
};