#pragma once

#include "../2D/SpriteManager.h"

#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Mesh.h"

#include <string>
#include <unordered_map>
#include <vector>

/// <summary>
/// ���f���f�[�^
/// </summary>
class Model {
private: // �G�C���A�X
  // Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: // �񋓎q
  /// <summary>
  /// ���[�g�p�����[�^�ԍ�
  /// </summary>
	enum class RoomParameter {
		kWorldTransform, // ���[���h�ϊ��s��
		kViewProjection, // �r���[�v���W�F�N�V�����ϊ��s��
		kMaterial,       // �}�e���A��
		kTexture,        // �e�N�X�`��
		kLight,          // ���C�g
	};

private:
	static const std::string kBaseDirectory;
	static const std::string kDefaultModelName;

private: // �ÓI�����o�ϐ�
  // �f�X�N���v�^�T�C�Y
	static UINT sDescriptorHandleIncrementSize_;
	// �R�}���h���X�g
	static ID3D12GraphicsCommandList* sCommandList_;
	// ���[�g�V�O�l�`��
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature_;
	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;

public: // �ÓI�����o�֐�
  /// <summary>
  /// �ÓI������
  /// </summary>
	static void StaticInitialize();

	/// <summary>
	/// �O���t�B�b�N�X�p�C�v���C���̏�����
	/// </summary>
	static void InitializeGraphicsPipeline();

	/// <summary>
/// 3D���f������
/// </summary>
/// <returns></returns>
	static Model* Create();

	// OBJ�t�@�C�����烁�b�V������
	///
	//���f����
	//�G�b�W�������t���O
	static Model* CreateFromOBJ(const std::string& modelname, bool smoothing = false);

/// �`��O����
//�`��R�}���h���X�g
	static void PreDraw(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// �`��㏈��
	/// </summary>
	static void PostDraw();

public: // �����o�֐�
  /// �f�X�g���N�^
	~Model();

	// ������
	//���f����</param>
	//�G�b�W�������t���O
	void Initialize(const std::string& modelname, bool smoothing = false);

	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="worldTransform">���[���h�g�����X�t�H�[��</param>
	/// <param name="viewProjection">�r���[�v���W�F�N�V����</param>
	void Draw(
		const WorldTransform& worldTransform, const ViewProjection& viewProjection);

	/// <summary>
	/// �`��i�e�N�X�`�������ւ��j
	/// </summary>
	/// <param name="worldTransform">���[���h�g�����X�t�H�[��</param>
	/// <param name="viewProjection">�r���[�v���W�F�N�V����</param>
	/// <param name="textureHadle">�e�N�X�`���n���h��</param>
	void Draw(
		const WorldTransform& worldTransform, const ViewProjection& viewProjection,
		uint32_t textureHadle);

	/// <summary>
	/// ���b�V���R���e�i���擾
	/// </summary>
	/// <returns>���b�V���R���e�i</returns>
	inline const std::vector<Mesh*>& GetMeshes() { return meshes_; }

private: // �����o�ϐ�
  // ���O
	std::string name_;
	// ���b�V���R���e�i
	std::vector<Mesh*> meshes_;
	// �}�e���A���R���e�i
	std::unordered_map<std::string, Material*> materials_;
	// �f�t�H���g�}�e���A��
	Material* defaultMaterial_ = nullptr;

private: // �����o�֐�
  /// <summary>
  /// ���f���ǂݍ���
  /// </summary>
  /// <param name="modelname">���f����</param>
  /// <param name="modelname">�G�b�W�������t���O</param>
	void LoadModel(const std::string& modelname, bool smoothing);

	/// <summary>
	/// �}�e���A���ǂݍ���
	/// </summary>
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// �}�e���A���o�^
	/// </summary>
	void AddMaterial(Material* material);

	/// <summary>
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	void LoadTextures();
};
