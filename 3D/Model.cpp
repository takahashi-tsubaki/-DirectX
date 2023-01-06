#include "../Core/DirectXCommon.h"
#include "Model.h"
#include <algorithm>
#include <cassert>
#include <d3dcompiler.h>
#include <fstream>
#include <sstream>

#pragma comment(lib, "d3dcompiler.lib")

using namespace std;
using namespace Microsoft::WRL;

/// <summary>
/// �ÓI�����o�ϐ��̎���
/// </summary>
const std::string Model::kBaseDirectory = "Resources/";
const std::string Model::kDefaultModelName = "cube";
UINT Model::sDescriptorHandleIncrementSize_ = 0;
ID3D12GraphicsCommandList* Model::sCommandList_ = nullptr;
ComPtr<ID3D12RootSignature> Model::sRootSignature_;
ComPtr<ID3D12PipelineState> Model::sPipelineState_;

void Model::StaticInitialize() {

	// �p�C�v���C��������
	InitializeGraphicsPipeline();

}

void Model::InitializeGraphicsPipeline() {
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob;    // ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob;    // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g

	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shaders/ObjVS.hlsl", // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0, &vsBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n(
			(char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shaders/ObjPS.hlsl", // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0, &psBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n(
			(char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	  {// xy���W(1�s�ŏ������ق������₷��)
	   "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
	   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	  {// �@���x�N�g��(1�s�ŏ������ق������₷��)
	   "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
	   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	  {// uv���W(1�s�ŏ������ق������₷��)
	   "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT,
	   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	// �O���t�B�b�N�X�p�C�v���C���̗����ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// �T���v���}�X�N
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
	// ���X�^���C�U�X�e�[�g
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	// gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	// gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	//  �f�v�X�X�e���V���X�e�[�g
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA�S�Ẵ`�����l����`��
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// �u�����h�X�e�[�g�̐ݒ�
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// �[�x�o�b�t�@�̃t�H�[�}�b�g
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// ���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// �}�`�̌`��ݒ�i�O�p�`�j
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;                       // �`��Ώۂ�1��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0�`255�w���RGBA
	gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	// �f�X�N���v�^�����W
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 ���W�X�^

	// ���[�g�p�����[�^
	CD3DX12_ROOT_PARAMETER rootparams[5];
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[2].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[3].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[4].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);

	// �X�^�e�B�b�N�T���v���[
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(
		_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// �o�[�W������������̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(
		&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	// ���[�g�V�O�l�`���̐���
	result = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(
		0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&sRootSignature_));
	assert(SUCCEEDED(result));

	gpipeline.pRootSignature = sRootSignature_.Get();

	// �O���t�B�b�N�X�p�C�v���C���̐���
	result = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(
		&gpipeline, IID_PPV_ARGS(&sPipelineState_));
	assert(SUCCEEDED(result));
}

Model* Model::Create() {
	// �������m��
	Model* instance = new Model;
	instance->Initialize(kDefaultModelName, false);

	return instance;
}

Model* Model::CreateFromOBJ(const std::string& modelname, bool smoothing) {
	// �������m��
	Model* instance = new Model;
	instance->Initialize(modelname, smoothing);

	return instance;
}

void Model::PreDraw(ID3D12GraphicsCommandList* commandList) {
	// PreDraw��PostDraw���y�A�ŌĂ΂�Ă��Ȃ���΃G���[
	assert(Model::sCommandList_ == nullptr);

	// �R�}���h���X�g���Z�b�g
	sCommandList_ = commandList;

	// �p�C�v���C���X�e�[�g�̐ݒ�
	commandList->SetPipelineState(sPipelineState_.Get());
	// ���[�g�V�O�l�`���̐ݒ�
	commandList->SetGraphicsRootSignature(sRootSignature_.Get());
	// �v���~�e�B�u�`���ݒ�
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Model::PostDraw() {
	// �R�}���h���X�g������
	sCommandList_ = nullptr;
}

Model::~Model() {
	for (auto m : meshes_) {
		delete m;
	}
	meshes_.clear();

	for (auto m : materials_) {
		delete m.second;
	}
	materials_.clear();
}

void Model::Initialize(const std::string& modelname, bool smoothing) {
	// ���f���ǂݍ���
	LoadModel(modelname, smoothing);

	// ���b�V���̃}�e���A���`�F�b�N
	for (auto& m : meshes_) {
		// �}�e���A���̊��蓖�Ă��Ȃ�
		if (m->GetMaterial() == nullptr) {
			if (defaultMaterial_ == nullptr) {
				// �f�t�H���g�}�e���A���𐶐�
				defaultMaterial_ = Material::Create();
				defaultMaterial_->name_ = "no material";
				materials_.emplace(defaultMaterial_->name_, defaultMaterial_);
			}
			// �f�t�H���g�}�e���A�����Z�b�g
			m->SetMaterial(defaultMaterial_);
		}
	}

	// ���b�V���̃o�b�t�@����
	for (auto& m : meshes_) {
		m->CreateBuffers();
	}

	// �}�e���A���̐��l��萔�o�b�t�@�ɔ��f
	for (auto& m : materials_) {
		m.second->Update();
	}

	// �e�N�X�`���̓ǂݍ���
	LoadTextures();
}

void Model::LoadModel(const std::string& modelname, bool smoothing) {
	const string filename = modelname + ".obj";
	const string directoryPath = kBaseDirectory + modelname + "/";

	// �t�@�C���X�g���[��
	std::ifstream file;
	// .obj�t�@�C�����J��
	file.open(directoryPath + filename);
	// �t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail()) {
		assert(0);
	}

	name_ = modelname;

	// ���b�V������
	meshes_.emplace_back(new Mesh);
	Mesh* mesh = meshes_.back();
	int indexCountTex = 0;
	int indexCountNoTex = 0;

	vector<Vector3> positions; // ���_���W
	vector<Vector3> normals;   // �@���x�N�g��
	vector<Vector2> texcoords; // �e�N�X�`��UV
	// 1�s���ǂݍ���
	string line;
	while (getline(file, line)) {

		// 1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
		std::istringstream line_stream(line);

		// ���p�X�y�[�X��؂�ōs�̐擪��������擾
		string key;
		getline(line_stream, key, ' ');

		//�}�e���A��
		if (key == "mtllib") {
			// �}�e���A���̃t�@�C�����ǂݍ���
			string filename;
			line_stream >> filename;
			// �}�e���A���ǂݍ���
			LoadMaterial(directoryPath, filename);
		}
		// �擪������g�Ȃ�O���[�v�̊J�n
		if (key == "g") {

			// �J�����g���b�V���̏�񂪑����Ă���Ȃ�
			if (mesh->GetName().size() > 0 && mesh->GetVertexCount() > 0) {
				// ���_�@���̕��ςɂ��G�b�W�̕�����
				if (smoothing) {
					mesh->CalculateSmoothedVertexNormals();
				}
				// ���̃��b�V������
				meshes_.emplace_back(new Mesh);
				mesh = meshes_.back();
				indexCountTex = 0;
			}

			// �O���[�v���ǂݍ���
			string groupName;
			line_stream >> groupName;

			// ���b�V���ɖ��O���Z�b�g
			mesh->SetName(groupName);
		}
		// �擪������v�Ȃ璸�_���W
		if (key == "v") {
			// X,Y,Z���W�ǂݍ���
			Vector3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			// X���]�ɂ��A�E��n�̃��f���f�[�^������n�ɕϊ�
			position.x = -position.x;
			positions.emplace_back(position);
		}
		// �擪������vt�Ȃ�e�N�X�`��
		if (key == "vt") {
			// U,V�����ǂݍ���
			Vector2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			// V�������]
			texcoord.y = 1.0f - texcoord.y;
			// �e�N�X�`�����W�f�[�^�ɒǉ�
			texcoords.emplace_back(texcoord);
		}
		// �擪������vn�Ȃ�@���x�N�g��
		if (key == "vn") {
			// X,Y,Z�����ǂݍ���
			Vector3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			// �@���x�N�g���f�[�^�ɒǉ�
			normals.emplace_back(normal);
		}
		// �擪������usemtl�Ȃ�}�e���A�������蓖�Ă�
		if (key == "usemtl") {
			if (mesh->GetMaterial() == nullptr) {
				// �}�e���A���̖��ǂݍ���
				string materialName;
				line_stream >> materialName;

				// �}�e���A�����Ō������A�}�e���A�������蓖�Ă�
				auto itr = materials_.find(materialName);
				if (itr != materials_.end()) {
					mesh->SetMaterial(itr->second);
				}
			}
		}
		// �擪������f�Ȃ�|���S���i�O�p�`�j
		if (key == "f") {
			int faceIndexCount = 0;
			// ���p�X�y�[�X��؂�ōs�̑�����ǂݍ���
			string index_string;
			std::array<uint16_t, 4> tempIndices;
			while (getline(line_stream, index_string, ' ')) {
				// ���_�C���f�b�N�X1���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				// ���_�ԍ�
				index_stream >> indexPosition;

				Material* material = mesh->GetMaterial();
				index_stream.seekg(1, ios_base::cur); // �X���b�V�����΂�
				// �}�e���A���A�e�N�X�`��������ꍇ
				if (material && material->textureFilename_.size() > 0) {
					index_stream >> indexTexcoord;
					index_stream.seekg(1, ios_base::cur); // �X���b�V�����΂�
					index_stream >> indexNormal;
					// ���_�f�[�^�̒ǉ�
					Mesh::VertexPosNormalUv vertex{};
					vertex.pos = positions[indexPosition - 1];
					vertex.normal = normals[indexNormal - 1];
					vertex.uv = texcoords[indexTexcoord - 1];
					mesh->AddVertex(vertex);
					// �G�b�W�������p�̃f�[�^��ǉ�
					if (smoothing) {
						mesh->AddSmoothData(
							indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
					}
				}
				else {
					char c;
					index_stream >> c;
					// �X���b�V��2�A���̏ꍇ�A���_�ԍ��̂�
					if (c == '/') {
						// ���_�f�[�^�̒ǉ�
						Mesh::VertexPosNormalUv vertex{};
						vertex.pos = positions[indexPosition - 1];
						vertex.normal = { 0, 0, 1 };
						vertex.uv = { 0, 0 };
						mesh->AddVertex(vertex);
					}
					else {
						index_stream.seekg(-1, ios_base::cur); // 1�����߂�
						index_stream >> indexTexcoord;
						index_stream.seekg(1, ios_base::cur); // �X���b�V�����΂�
						index_stream >> indexNormal;
						// ���_�f�[�^�̒ǉ�
						Mesh::VertexPosNormalUv vertex{};
						vertex.pos = positions[indexPosition - 1];
						vertex.normal = normals[indexNormal - 1];
						vertex.uv = { 0, 0 };
						mesh->AddVertex(vertex);
						// �G�b�W�������p�̃f�[�^��ǉ�
						if (smoothing) {
							mesh->AddSmoothData(
								indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
						}
					}
				}

				assert(faceIndexCount < 4 && "5�p�`�|���S���ȏ�͔�Ή��ł�");

				// �C���f�b�N�X�f�[�^�̒ǉ�
				tempIndices[faceIndexCount] = indexCountTex;

				indexCountTex++;
				faceIndexCount++;
			}

			// �C���f�b�N�X�f�[�^�̏����ύX�Ŏ��v��聨�����v���ϊ�
			mesh->AddIndex(tempIndices[0]);
			mesh->AddIndex(tempIndices[2]);
			mesh->AddIndex(tempIndices[1]);
			// �l�p�`�Ȃ�O�p�`��ǉ�
			if (faceIndexCount == 4) {
				mesh->AddIndex(tempIndices[0]);
				mesh->AddIndex(tempIndices[3]);
				mesh->AddIndex(tempIndices[2]);
			}
		}
	}
	file.close();

	// ���_�@���̕��ςɂ��G�b�W�̕�����
	if (smoothing) {
		mesh->CalculateSmoothedVertexNormals();
	}
}

void Model::LoadMaterial(const std::string& directoryPath, const std::string& filename) {
	// �t�@�C���X�g���[��
	std::ifstream file;
	// �}�e���A���t�@�C�����J��
	file.open(directoryPath + filename);
	// �t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail()) {
		assert(0);
	}

	Material* material = nullptr;

	// 1�s���ǂݍ���
	string line;
	while (getline(file, line)) {

		// 1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
		std::istringstream line_stream(line);

		// ���p�X�y�[�X��؂�ōs�̐擪��������擾
		string key;
		getline(line_stream, key, ' ');

		// �擪�̃^�u�����͖�������
		if (key[0] == '\t') {
			key.erase(key.begin()); // �擪�̕������폜
		}

		// �擪������newmtl�Ȃ�}�e���A����
		if (key == "newmtl") {

			// ���Ƀ}�e���A���������
			if (material) {
				// �}�e���A�����R���e�i�ɓo�^
				AddMaterial(material);
			}

			// �V�����}�e���A���𐶐�
			material = Material::Create();
			// �}�e���A�����ǂݍ���
			line_stream >> material->name_;
		}
		// �擪������Ka�Ȃ�A���r�G���g�F
		if (key == "Ka") {
			line_stream >> material->ambient_.x;
			line_stream >> material->ambient_.y;
			line_stream >> material->ambient_.z;
		}
		// �擪������Kd�Ȃ�f�B�t���[�Y�F
		if (key == "Kd") {
			line_stream >> material->diffuse_.x;
			line_stream >> material->diffuse_.y;
			line_stream >> material->diffuse_.z;
		}
		// �擪������Ks�Ȃ�X�y�L�����[�F
		if (key == "Ks") {
			line_stream >> material->specular_.x;
			line_stream >> material->specular_.y;
			line_stream >> material->specular_.z;
		}
		// �擪������map_Kd�Ȃ�e�N�X�`���t�@�C����
		if (key == "map_Kd") {
			// �e�N�X�`���̃t�@�C�����ǂݍ���
			line_stream >> material->textureFilename_;

			// �t���p�X����t�@�C���������o��
			size_t pos1;
			pos1 = material->textureFilename_.rfind('\\');
			if (pos1 != string::npos) {
				material->textureFilename_ = material->textureFilename_.substr(
					pos1 + 1, material->textureFilename_.size() - pos1 - 1);
			}

			pos1 = material->textureFilename_.rfind('/');
			if (pos1 != string::npos) {
				material->textureFilename_ = material->textureFilename_.substr(
					pos1 + 1, material->textureFilename_.size() - pos1 - 1);
			}
		}
	}
	// �t�@�C�������
	file.close();

	if (material) {
		// �}�e���A����o�^
		AddMaterial(material);
	}
}

void Model::AddMaterial(Material* material) {
	// �R���e�i�ɓo�^
	materials_.emplace(material->name_, material);
}

void Model::LoadTextures() {
	int textureIndex = 0;
	string directoryPath = name_ + "/";

	for (auto& m : materials_) {
		Material* material = m.second;

		// �e�N�X�`������
		if (material->textureFilename_.size() > 0) {
			// �}�e���A���Ƀe�N�X�`���ǂݍ���
			material->LoadTexture(directoryPath);
			textureIndex++;
		}
		// �e�N�X�`���Ȃ�
		else {
			// �}�e���A���Ƀe�N�X�`���ǂݍ���
			material->LoadTexture("white1x1.png");
			textureIndex++;
		}
	}
}

void Model::Draw(
	const WorldTransform& worldTransform, const ViewProjection& viewProjection) {


	// CBV���Z�b�g�i���[���h�s��j
	sCommandList_->SetGraphicsRootConstantBufferView(
		static_cast<UINT>(RoomParameter::kWorldTransform),
		worldTransform.constBuff_->GetGPUVirtualAddress());

	// CBV���Z�b�g�i�r���[�v���W�F�N�V�����s��j
	sCommandList_->SetGraphicsRootConstantBufferView(
		static_cast<UINT>(RoomParameter::kViewProjection),
		viewProjection.constBuff_->GetGPUVirtualAddress());

	// �S���b�V����`��
	for (auto& mesh : meshes_) {
		mesh->Draw(sCommandList_, (UINT)RoomParameter::kMaterial, (UINT)RoomParameter::kTexture);
	}
}

void Model::Draw(
	const WorldTransform& worldTransform, const ViewProjection& viewProjection,
	uint32_t textureHadle) 
{

	// CBV���Z�b�g�i���[���h�s��j
	sCommandList_->SetGraphicsRootConstantBufferView(
		static_cast<UINT>(RoomParameter::kWorldTransform),
		worldTransform.constBuff_->GetGPUVirtualAddress());

	// CBV���Z�b�g�i�r���[�v���W�F�N�V�����s��j
	sCommandList_->SetGraphicsRootConstantBufferView(
		static_cast<UINT>(RoomParameter::kViewProjection),
		viewProjection.constBuff_->GetGPUVirtualAddress());

	// �S���b�V����`��
	for (auto& mesh : meshes_) {
		mesh->Draw(
			sCommandList_, (UINT)RoomParameter::kMaterial, (UINT)RoomParameter::kTexture,
			textureHadle);
	}
}
