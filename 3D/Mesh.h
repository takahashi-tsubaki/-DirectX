#pragma once

#include "../math/Vector2.h"
#include "../math/Vector3.h"

#include "Material.h"
#include <Windows.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <unordered_map>
#include <vector>
#include <wrl.h>

class Mesh {
private:
	// Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	//���_�f�[�^�\���́i�e�N�X�`������j
	struct VertexPosNormalUv {
		Vector3 pos;    // xyz���W
		Vector3 normal; // �@���x�N�g��
		Vector2 uv;     // uv���W
	};

// �����o�֐�

	// ���O���擾
	const std::string& GetName() { return name_; }

	// ���O���Z�b�g
	void SetName(const std::string& name_);

	///���_�f�[�^�̒ǉ�
	void AddVertex(const VertexPosNormalUv& vertex);

	// �C���f�b�N�X�f�[�^�̒ǉ�
	void AddIndex(unsigned short index);

	//�@���_�f�[�^�̐����擾
	inline size_t GetVertexCount() { return vertices_.size(); }

	// �G�b�W�������f�[�^�̒ǉ�
	void AddSmoothData(unsigned short indexPosition, unsigned short indexVertex);

	// ���������ꂽ���_�@���̌v�Z
	void CalculateSmoothedVertexNormals();

	// �}�e���A���̎擾
	Material* GetMaterial() { return material_; }

	// �}�e���A���̊��蓖��
	void SetMaterial(Material* material);

	// �o�b�t�@�̐���
	void CreateBuffers();

	// ���_�o�b�t�@�̎擾
	const D3D12_VERTEX_BUFFER_VIEW& GetVBView() { return vbView_; }

	/// �C���f�b�N�X�o�b�t�@�̎擾
	const D3D12_INDEX_BUFFER_VIEW& GetIBView() { return ibView_; }

	// �`��
	void Draw(
		ID3D12GraphicsCommandList* commandList, UINT rooParameterIndexMaterial,
		UINT rooParameterIndexTexture);

	// �`��i�e�N�X�`�������ւ��Łj
	//�@�����ւ���e�N�X�`���n���h��
	void Draw(
		ID3D12GraphicsCommandList* commandList, UINT rooParameterIndexMaterial,
		UINT rooParameterIndexTexture, uint32_t textureHandle);

	// ���_�z��̎擾
	inline const std::vector<VertexPosNormalUv>& GetVertices() { return vertices_; }

	// �C���f�b�N�X�z��̎擾
	inline const std::vector<unsigned short>& GetIndices() { return indices_; }

private: // �����o�ϐ�
  // ���O
	std::string name_;
	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff_;
	// �C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff_;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};
	// �C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};
	// ���_�f�[�^�z��
	std::vector<VertexPosNormalUv> vertices_;
	// ���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices_;
	// ���_�@���X���[�W���O�p�f�[�^
	std::unordered_map<unsigned short, std::vector<unsigned short>> smoothData_;
	// �}�e���A��
	Material* material_ = nullptr;
};
