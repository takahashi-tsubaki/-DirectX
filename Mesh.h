#pragma once
#include <d3d12.h>
#include <vector>
#include <DirectXMath.h>
#include <d3dcompiler.h>

using namespace DirectX;

//�萔�o�b�t�@
struct ConstBufferDataMaterial
{
	XMFLOAT4 color;//�F
};

class Mesh
{
public:
	//�R���X�g���N�^
	Mesh(XMFLOAT3 vertices[3], ID3D12Device* dev);

	//�`��֐�
	void Draw(ID3D12GraphicsCommandList* commandList);

private:

	//���_�f�[�^
	XMFLOAT3 vertices[3] =
	{
		
	};

	uint16_t indices[3] =
	{
		0,1,2,
	};

	HRESULT result;

	UINT sizeVB;

	//���_�o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp{};//�q�[�v�ݒ�

	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};

	//GPU��̃o�b�t�@�ɑΉ��������z������(���C����������)���擾
	XMFLOAT3* vertMap = nullptr;

	//���_�o�b�t�@�[�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	ID3DBlob* vsBlob = nullptr;//���_�V�F�[�_�I�u�W�F�N�g
	ID3DBlob* psBlob = nullptr;//�s�N�Z���V�F�[�_�I�u�W�F�N�g
	ID3DBlob* errorBlob = nullptr;//�G���[�I�u�W�F�N�g

	//���_�o�b�t�@�̍쐬
	ID3D12Resource* vertBuff = nullptr;

	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};

	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};

	ID3D12Resource* constBuffMaterial = nullptr;

	//�萔�o�b�t�@�̃}�b�s���O
	ConstBufferDataMaterial* constMapMaterial = nullptr;

	//���[�g�p�����[�^
	D3D12_ROOT_PARAMETER rootParam = {};

	//�C���f�b�N�X�f�[�^�T�C�Y
	UINT sizeIB;

	//�C���f�b�N�X�o�b�t�@�̐���
	ID3D12Resource* indexBuff = nullptr;

	//�C���f�b�N�X�o�b�t�@���}�b�s���O
	uint16_t* indexMap = nullptr;

	//�C���f�b�N�X�o�b�t�@�r���[�̐���
	D3D12_INDEX_BUFFER_VIEW ibView{};

	//�O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	//���[�g�V�O�l�`��
	ID3D12RootSignature* rootSignature;

	//���[�g�V�O�l�`���̐ݒ�(����)
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};

	//���[�g�V�O�l�`���̃V���A���C�Y
	ID3DBlob* rootSigBlob = nullptr;

	//�p�C�v���C���X�e�[�g�̐���
	ID3D12PipelineState* pipelineState;

};