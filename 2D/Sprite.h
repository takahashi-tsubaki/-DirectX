#pragma once
#include "SpriteManager.h"
class Sprite
{
private:

	//�萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferDataMaterial
	{
		XMFLOAT4 color;//�F
	};
	//�萔�o�b�t�@�p�f�[�^�\����(3D�ϊ��s��)
	struct ConstBufferDataTransform
	{
		XMMATRIX mat;//3D�ϊ��s��
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

	ID3D12GraphicsCommandList* sCommandList_ = nullptr;

	SpriteManager* spManager_ = nullptr;
	
	//���_�o�b�t�@�[�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	//�ݒ������SRV�p�f�X�N���v�^�q�[�v�𐶐�
	ID3D12DescriptorHeap* srvHeap = nullptr;
	//SRV�q�[�v�̐擪�n���h�����擾
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;

	//�ˉe�ϊ��s��
	XMMATRIX matProjection;
	//���[���h�ϊ��s��
	XMMATRIX matWorld;
	//��]�s��
	XMMATRIX matRot;
	//���s�ړ��s��
	XMMATRIX matTrans;
	DirectX::XMFLOAT2 rotation = { 0.0f,0.0f};
	DirectX::XMFLOAT2 position_ = { 0.0f,0.0f };
	

	//�萔�o�b�t�@�̐���
	ID3D12Resource* constBuffMaterial = nullptr;

	ID3D12Resource* constBuffTransform = nullptr;
	//�萔�o�b�t�@�̃}�b�s���O�p�|�C���^
	ConstBufferDataTransform* constMapTransform = nullptr;

public:
	void Initialize(SpriteManager*spManager);

	void Draw();

	void preDraw();

	void postDraw();

	void SetPosition(XMFLOAT2 position);

	DirectX::XMFLOAT2 GetPosition() {return position_;}

	void TransferVertex();
};