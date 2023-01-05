#pragma once

#include "../math/Vector3.h"
#include "../math/Matrix4.h"
#include <d3d12.h>
#include <wrl.h>

// �萔�o�b�t�@�p�f�[�^�\����
struct ConstBufferDataWorldTransform {
	Matrix4 matWorld;           // ���[�J�� �� ���[���h�ϊ��s��
};

/// <summary>
/// ���[���h�ϊ��f�[�^
/// </summary>
struct WorldTransform {
	// �萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	// �}�b�s���O�ς݃A�h���X
	ConstBufferDataWorldTransform* constMap = nullptr;
	// ���[�J���X�P�[��
	Vector3 scale_ = { 1, 1, 1 };
	// X,Y,Z�����̃��[�J����]�p
	Vector3 rotation_ = { 0, 0, 0 };
	// ���[�J�����W
	Vector3 translation_ = { 0, 0, 0 };
	// ���[�J�� �� ���[���h�ϊ��s��
	Matrix4 matWorld_;
	// �e�ƂȂ郏�[���h�ϊ��ւ̃|�C���^
	const WorldTransform* parent_ = nullptr;

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();
	/// <summary>
	/// �萔�o�b�t�@����
	/// </summary>
	void CreateConstBuffer();
	/// <summary>
	/// �}�b�s���O����
	/// </summary>
	void Map();
	/// <summary>
	/// �s���]������
	/// </summary>
	void TransferMatrix();
};

