#include "gameScene.h"

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initalize()
{
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	audio_->Initialize();
	audio_->LoadWave("se_amd06.wav");

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
	sprite_ = Sprite::Create(1, { WinApp::window_width/2 - 500 , WinApp::window_height/2});

	
	
	

	modelskydome_ = Model::CreateFromOBJ("skydome");
	skydome_ = Object3d::Create();
	//���f�����Z�b�g
	skydome_->SetModel(modelskydome_);

	title_ = new titleScene();
	title_->Initialize(scene_);

}

void GameScene::Update()
{
	//���̃V�[���Ȃ̂�
	switch (scene_)
	{
	case Scene::Title:
		if (input_->TriggerKey(DIK_RETURN) && scene_ == Scene::Title)
		{
			scene_ = Scene::Game;

		}
		break;
	case Scene::Game:
		if (input_->TriggerKey(DIK_RETURN) && scene_ == Scene::Game)
		{
			move = 0.0f;
			isStop = false;
			scene_ = Scene::Title;

		}

		//CI�̏���
		if (isStop == false)//�{�^���������ĂȂ���
		{
			//CI�̑傫���̏���
			if (isGaugeUp == true)
			{
				move+= 8.0f;
			}
			else if (isGaugeDown == true)
			{
				move-= 8.0f;
			}
			//CI�̃A�b�v�_�E���̐؂�ւ�
			if (move >= 256)
			{
				isGaugeUp = false;
				isGaugeDown = true;
			}
			else if (move <= 0)
			{
				isGaugeDown = false;
				isGaugeUp = true;
			}
		}
		else
		{
			move = move;
		}
		//CI���~�߂鏈��
		if (input_->TriggerKey(DIK_SPACE))
		{
			if (isStop == false)
			{
				isStop = true;
			}
			else
			{
				isStop = false;
			}
		}

		break;
	}

	camera_->Update();

	skydome_->Update();

	light_->Update();

	

	sprite_->SetSize({ move,128.0f });//   �s�N�Z����/���̃e�N�X�`���T�C�Y
	
	sprite_->SetTextureRect({0.0f,0.0f},{ move,256});
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

	switch (scene_)
	{
	case Scene::Title:
		skydome_->Draw();
		break;
	case Scene::Game:
		
		break;
	}

	// 3D�I�u�W�F�N�g�̕`��
	
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
	switch (scene_)
	{
	case Scene::Title:
		break;
	case Scene::Game:
		sprite_->Draw();
		break;
	}

	//
	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();

#pragma endregion
}
