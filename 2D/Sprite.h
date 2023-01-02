#pragma once

#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"
#include "../math/Matrix4.h"
#include <Windows.h>
#include <d3d12.h>
#include <string>
#include <wrl.h>

/// <summary>
/// �X�v���C�g
/// </summary>
class Sprite {
public:
	enum class BlendMode {
		kNone,     //!< �u�����h�Ȃ�
		kNormal,   //!< �ʏ탿�u�����h�B�f�t�H���g�B Src * SrcA + Dest * (1 - SrcA)
		kAdd,      //!< ���Z�BSrc * SrcA + Dest * 1
		kSubtract, //!< ���Z�BDest * 1 - Src * SrcA
		kMultily,  //!< ��Z�BSrc * 0 + Dest * Src
		kScreen,   //!< �X�N���[���BSrc * (1 - Dest) + Dest * 1

		kCountOfBlendMode, //!< �u�����h���[�h���B�w��͂��Ȃ�
	};

public: // �T�u�N���X
  /// <summary>
  /// ���_�f�[�^�\����
  /// </summary>
	struct VertexPosUv {
		Vector3 pos; // xyz���W
		Vector2 uv;  // uv���W
	};

	/// <summary>
	/// �萔�o�b�t�@�p�f�[�^�\����
	/// </summary>
	struct ConstBufferData {
		Vector4 color; // �F (RGBA)
		Matrix4 mat;   // �R�c�ϊ��s��
	};

public: // �ÓI�����o�֐�
  /// <summary>
  /// �ÓI������
  /// </summary>
  /// <param name="device">�f�o�C�X</param>
  /// <param name="window_width">��ʕ�</param>
  /// <param name="window_height">��ʍ���</param>
	static void StaticInitialize(
		ID3D12Device* device, int window_width, int window_height,
		const std::wstring& directoryPath = L"Resources/");

	/// <summary>
	/// �`��O����
	/// </summary>
	/// <param name="cmdList">�`��R�}���h���X�g</param>
	static void
		PreDraw(ID3D12GraphicsCommandList* cmdList, BlendMode blendMode = BlendMode::kNormal);

	/// <summary>
	/// �`��㏈��
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// �X�v���C�g����
	/// </summary>
	/// <param name="texNumber">�e�N�X�`���n���h��</param>
	/// <param name="position">���W</param>
	/// <param name="color">�F</param>
	/// <param name="anchorpoint">�A���J�[�|�C���g</param>
	/// <param name="isFlipX">���E���]</param>
	/// <param name="isFlipY">�㉺���]</param>
	/// <returns>�������ꂽ�X�v���C�g</returns>
	static Sprite* Create(
		uint32_t textureHandle, Vector2 position, Vector4 color = { 1, 1, 1, 1 },
		Vector2 anchorpoint = { 0.0f, 0.0f }, bool isFlipX = false, bool isFlipY = false);

private: // �ÓI�����o�ϐ�
  // ���_��
	static const int kVertNum = 4;
	// �f�o�C�X
	static ID3D12Device* sDevice_;
	// �f�X�N���v�^�T�C�Y
	static UINT sDescriptorHandleIncrementSize_;
	// �R�}���h���X�g
	static ID3D12GraphicsCommandList* sCommandList_;
	// ���[�g�V�O�l�`��
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature_;
	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static std::array<
		Microsoft::WRL::ComPtr<ID3D12PipelineState>, size_t(BlendMode::kCountOfBlendMode)>
		sPipelineStates_;
	// �ˉe�s��
	static Matrix4 sMatProjection_;

public: // �����o�֐�
  /// <summary>
  /// �R���X�g���N�^
  /// </summary>
	Sprite();
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	Sprite(
		uint32_t textureHandle, Vector2 position, Vector2 size,
		Vector4 color, Vector2 anchorpoint, bool isFlipX, bool isFlipY);

	/// <summary>
	/// ������
	/// </summary>
	/// <returns>����</returns>
	bool Initialize();

	/// <summary>
	/// �e�N�X�`���n���h���̐ݒ�
	/// </summary>
	/// <param name="textureHandle">�e�N�X�`���n���h��</param>
	void SetTextureHandle(uint32_t textureHandle);

	uint32_t GetTextureHandle() { return textureHandle_; }

	/// <summary>
	/// ���W�̐ݒ�
	/// </summary>
	/// <param name="position">���W</param>
	void SetPosition(const Vector2& position);

	const Vector2& GetPosition() { return position_; }

	/// <summary>
	/// �p�x�̐ݒ�
	/// </summary>
	/// <param name="rotation">�p�x</param>
	void SetRotation(float rotation);

	float GetRotation() { return rotation_; }

	/// <summary>
	/// �T�C�Y�̐ݒ�
	/// </summary>
	/// <param name="size">�T�C�Y</param>
	void SetSize(const Vector2& size);

	const Vector2& GetSize() { return size_; }

	/// <summary>
	/// �A���J�[�|�C���g�̐ݒ�
	/// </summary>
	/// <param name="anchorpoint">�A���J�[�|�C���g</param>
	void SetAnchorPoint(const Vector2& anchorpoint);

	const Vector2& GetAnchorPoint() { return anchorPoint_; }

	/// <summary>
	/// �F�̐ݒ�
	/// </summary>
	/// <param name="color">�F</param>
	void SetColor(const Vector4& color) { color_ = color; };

	const Vector4& GetColor() { return color_; }

	/// <summary>
	/// ���E���]�̐ݒ�
	/// </summary>
	/// <param name="isFlipX">���E���]</param>
	void SetIsFlipX(bool isFlipX);

	bool GetIsFlipX() { return isFlipX_; }

	/// <summary>
	/// �㉺���]�̐ݒ�
	/// </summary>
	/// <param name="isFlipX">�㉺���]</param>
	void SetIsFlipY(bool isFlipY);

	bool GetIsFlipY() { return isFlipY_; }

	/// <summary>
	/// �e�N�X�`���͈͐ݒ�
	/// </summary>
	/// <param name="texBase">�e�N�X�`��������W</param>
	/// <param name="texSize">�e�N�X�`���T�C�Y</param>
	void SetTextureRect(const Vector2& texBase, const Vector2& texSize);

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

private: // �����o�ϐ�
  // ���_�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// �萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	// ���_�o�b�t�@�}�b�v
	VertexPosUv* vertMap_ = nullptr;
	// �萔�o�b�t�@�}�b�v
	ConstBufferData* constMap_ = nullptr;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	// �e�N�X�`���ԍ�
	UINT textureHandle_ = 0;
	// Z�����̉�]�p
	float rotation_ = 0.0f;
	// ���W
	Vector2 position_{};
	// �X�v���C�g���A����
	Vector2 size_ = { 100.0f, 100.0f };
	// �A���J�[�|�C���g
	Vector2 anchorPoint_ = { 0, 0 };
	// ���[���h�s��
	Matrix4 matWorld_{};
	// �F
	Vector4 color_ = { 1, 1, 1, 1 };
	// ���E���]
	bool isFlipX_ = false;
	// �㉺���]
	bool isFlipY_ = false;
	// �e�N�X�`���n�_
	Vector2 texBase_ = { 0, 0 };
	// �e�N�X�`�����A����
	Vector2 texSize_ = { 100.0f, 100.0f };
	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resourceDesc_;

private: // �����o�֐�
  /// <summary>
  /// ���_�f�[�^�]��
  /// </summary>
	void TransferVertices();
};
