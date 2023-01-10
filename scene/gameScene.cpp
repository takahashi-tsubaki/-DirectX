#include "gameScene.h"

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initalize()
{
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	camera_ = new Camera(WinApp::window_width, WinApp::window_height);
	// �J���������_���Z�b�g
	camera_->SetTarget({ 0, 1, 0 });
	// 3D�I�u�W�F�N�g�ɃJ�������Z�b�g
	Object3d::SetCamera(camera_);
	//���C�g����
	light_ = Light::Create();
	//���C�g�F��ݒ�
	light_->SetLightColor({ 1,1,1 });
	//3D�I�u�W�F�N�g�Ƀ��C�g���Z�b�g
	Object3d::SetLight(light_);


	Sprite::LoadTexture(1, L"Resources/kuribo-.jpg");
	sprite_ = Sprite::Create(1, { 0.0f,0.0f });

	modelskydome_ = Model::CreateFromOBJ("skydome");
	skydome_ = Object3d::Create();
	//���f�����Z�b�g
	skydome_->SetModel(modelskydome_);

}

void GameScene::Update()
{
	camera_->Update();

	skydome_->Update();

	light_->Update();
}

void GameScene::Draw()
{
#pragma region �w�i�X�v���C�g�`��
	// �w�i�X�v���C�g�`��O����
	Sprite::PreDraw(dxCommon_->GetCommandList());
	// �w�i�X�v���C�g�`��

	/// <summary>
	/// �����ɔw�i�X�v���C�g�̕`�揈����ǉ��ł���
	/// </summary>

	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();
#pragma endregion

#pragma region 3D�I�u�W�F�N�g�`��
	// 3D�I�u�W�F�N�g�`��O����
	Object3d::PreDraw(dxCommon_->GetCommandList());

	// 3D�I�u�W�F�N�g�̕`��
	skydome_->Draw();
	/// <summary>
	/// ������3D�I�u�W�F�N�g�̕`�揈����ǉ��ł���
	/// </summary>

	// 3D�I�u�W�F�N�g�`��㏈��
	Object3d::PostDraw();
#pragma endregion

#pragma region �O�i�X�v���C�g�`��
	// �O�i�X�v���C�g�`��O����
	Sprite::PreDraw(dxCommon_->GetCommandList());

	/// <summary>
	/// �����ɑO�i�X�v���C�g�̕`�揈����ǉ��ł���
	/// </summary>
	sprite_->Draw();
	//
	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();

#pragma endregion
}
