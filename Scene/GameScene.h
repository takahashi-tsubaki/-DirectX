#pragma once
#pragma once

#include "../Core/DirectXCommon.h"

#include "../2D/Sprite.h"

#include "../3D/Model.h"
#include "../3D/ViewProjection.h"
#include "../3D/WorldTransform.h"
#include "../Input.h"

/// <summary>
/// �Q�[���V�[��
/// </summary>
class GameScene {

public: // �����o�֐�
  /// <summary>
  /// �R���X�g�N���^
  /// </summary>
	GameScene();

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~GameScene();

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// ���t���[������
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

private: // �����o�ϐ�
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Model* model_ = nullptr;

	/// <summary>
	/// �Q�[���V�[���p
	/// </summary>
};
#pragma once
