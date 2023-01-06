#pragma once

#include "../math//MyMath.h"

#include <d3d12.h>
#include <wrl.h>

// �萔�o�b�t�@�p�f�[�^�\����
struct ConstBufferDataViewProjection {
	Matrix4 view;       // ���[���h �� �r���[�ϊ��s��
	Matrix4 projection; // �r���[ �� �v���W�F�N�V�����ϊ��s��
	Vector3 cameraPos;  // �J�������W�i���[���h���W�j
};

// �r���[�v���W�F�N�V�����ϊ��f�[�^
struct ViewProjection {
	// �萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	// �}�b�s���O�ς݃A�h���X
	ConstBufferDataViewProjection* constMap = nullptr;

	// ���_���W
	Vector3 eye = { 0, 0, -50.0f };
	// �����_���W
	Vector3 target = { 0, 0, 0 };
	// ������x�N�g��
	Vector3 up = { 0, 1, 0 };


	// ������������p
	float fovAngleY = 45.0f * MyMath::PI / 180.0f;
	// �r���[�|�[�g�̃A�X�y�N�g��
	float aspectRatio = (float)16 / 9;
	// �[�x���E�i��O���j
	float nearZ = 0.1f;
	// �[�x���E�i�����j
	float farZ = 1000.0f;

	// �r���[�s��
	Matrix4 matView;

	// �ˉe�s��
	Matrix4 matProjection;

	// ������
	void Initialize();

	//�萔�o�b�t�@�̐���
	void CreateConstBuffer();

	/// �}�b�s���O
	void Map();

	// �s��̍X�V
	void UpdateMatrix();

	// �s��̓]��
	void TransferMatrix();
};

