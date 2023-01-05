#include "../Core/DirectXCommon.h"
#include "../math/MyMath.h"
#include "Mesh.h"
#include <cassert>
#include <d3dcompiler.h>

using namespace MyMath;

#pragma comment(lib, "d3dcompiler.lib")

void Mesh::SetName(const std::string& name_) { this->name_ = name_; }

void Mesh::AddVertex(const VertexPosNormalUv& vertex) { vertices_.emplace_back(vertex); }

void Mesh::AddIndex(unsigned short index) { indices_.emplace_back(index); }

void Mesh::AddSmoothData(unsigned short indexPosition, unsigned short indexVertex) {
	smoothData_[indexPosition].emplace_back(indexVertex);
}

void Mesh::CalculateSmoothedVertexNormals() {
	auto itr = smoothData_.begin();
	for (; itr != smoothData_.end(); ++itr) {
		// �e�ʗp�̋��ʒ��_�R���N�V����
		std::vector<unsigned short>& v = itr->second;
		// �S���_�̖@���𕽋ς���
		Vector3 normal = {};
		for (unsigned short index : v) {
			normal += vertices_[index].normal;
		}
		normal /= (float)v.size();
		normal = Vector3Normalize(normal);

		for (unsigned short index : v) {
			vertices_[index].normal = normal;
		}
	}
}

void Mesh::SetMaterial(Material* material) { this->material_ = material; }

void Mesh::CreateBuffers() {
	HRESULT result;

	UINT sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUv) * vertices_.size());

	// �q�[�v�v���p�e�B
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

	// ���_�o�b�t�@����
	result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertBuff_));
	assert(SUCCEEDED(result));

	// ���_�o�b�t�@�ւ̃f�[�^�]��
	VertexPosNormalUv* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices_.begin(), vertices_.end(), vertMap);
		vertBuff_->Unmap(0, nullptr);
	}

	// ���_�o�b�t�@�r���[�̍쐬
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	vbView_.StrideInBytes = sizeof(vertices_[0]);

	if (FAILED(result)) {
		assert(0);
		return;
	}

	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices_.size());
	// ���\�[�X�ݒ�
	resourceDesc.Width = sizeIB;
	// �C���f�b�N�X�o�b�t�@����
	result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&indexBuff_));
	if (FAILED(result)) {
		assert(0);
		return;
	}

	// �C���f�b�N�X�o�b�t�@�ւ̃f�[�^�]��
	unsigned short* indexMap = nullptr;
	result = indexBuff_->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result)) {
		std::copy(indices_.begin(), indices_.end(), indexMap);
		indexBuff_->Unmap(0, nullptr);
	}

	// �C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = sizeIB;
}

void Mesh::Draw(
	ID3D12GraphicsCommandList* commandList, UINT rooParameterIndexMaterial,
	UINT rooParameterIndexTexture) {
	// ���_�o�b�t�@���Z�b�g
	commandList->IASetVertexBuffers(0, 1, &vbView_);
	// �C���f�b�N�X�o�b�t�@���Z�b�g
	commandList->IASetIndexBuffer(&ibView_);

	// �}�e���A���̃O���t�B�b�N�X�R�}���h���Z�b�g
	material_->SetGraphicsCommand(commandList, rooParameterIndexMaterial, rooParameterIndexTexture);

	// �`��R�}���h
	commandList->DrawIndexedInstanced((UINT)indices_.size(), 1, 0, 0, 0);
}

void Mesh::Draw(
	ID3D12GraphicsCommandList* commandList, UINT rooParameterIndexMaterial,
	UINT rooParameterIndexTexture, uint32_t textureHandle) {
	// ���_�o�b�t�@���Z�b�g
	commandList->IASetVertexBuffers(0, 1, &vbView_);
	// �C���f�b�N�X�o�b�t�@���Z�b�g
	commandList->IASetIndexBuffer(&ibView_);

	// �}�e���A���̃O���t�B�b�N�X�R�}���h���Z�b�g
	material_->SetGraphicsCommand(
		commandList, rooParameterIndexMaterial, rooParameterIndexTexture, textureHandle);

	// �`��R�}���h
	commandList->DrawIndexedInstanced((UINT)indices_.size(), 1, 0, 0, 0);
}
