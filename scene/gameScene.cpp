#include "gameScene.h"

GameScene::GameScene() {}

GameScene::~GameScene() 
{
	modelskydome_;
	skydome_;
}

void GameScene::Initialize()
{
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	audio_->Initialize();

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

	//�X�v���C�g�̃��[�h
	Sprite::LoadTexture(1, L"Resources/CI.png");
	Sprite::LoadTexture(2, L"Resources/CIBack.png");
	Sprite::LoadTexture(3, L"Resources/CIWin.png");
	Sprite::LoadTexture(4, L"Resources/num3Tex.png");
	Sprite::LoadTexture(5, L"Resources/num2Tex.png");
	Sprite::LoadTexture(6, L"Resources/num1Tex.png");
	Sprite::LoadTexture(7, L"Resources/vs.png");
	Sprite::LoadTexture(8, L"Resources/READY.png");
	Sprite::LoadTexture(9, L"Resources/tutorial.png");
	Sprite::LoadTexture(10,L"Resources/pushSpace.png");
	Sprite::LoadTexture(11, L"Resources/thanks.png");

	sprite_ = Sprite::Create(1, { WinApp::window_width/2 - 400 , WinApp::window_height/2});
	enemyCI_ = Sprite::Create(1, { WinApp::window_width / 2 + 400 , WinApp::window_height / 2 });
	enemyCI_->SetIsFlipX(true);
	//CI�̔w�i
	CIBackSprite_ = Sprite::Create(2, { WinApp::window_width / 2 - 400 , WinApp::window_height / 2 });
	enemyCIBack_ =  Sprite::Create(2, { WinApp::window_width / 2 + 400 , WinApp::window_height / 2 });
	enemyCIBack_->SetIsFlipX(true);
	
	//Win��Sprite����
	CIWin_ = Sprite::Create(3,CIWinPos);
	enemyCIWin_ = Sprite::Create(3, EnemyCIWinPos);

	//VS��Sprite����
	vsSprite_ = Sprite::Create(7, { WinApp::window_width / 2-100, WinApp::window_height / 2 });
	vsSprite_->SetSize({192,128});
	//READY��Sprite����
	readySp_ = Sprite::Create(8, { WinApp::window_width / 2 - 125, WinApp::window_height / 2 -150});
	//��������Sprite����
	tutorialSp_ = Sprite::Create(9,{ WinApp::window_width / 2 - 250, WinApp::window_height / 2 - 300 });

	//pushSpace��Sprite����
	spaceSp_ = Sprite::Create(10, { WinApp::window_width / 2 - 150, WinApp::window_height / 2 + 100 });
	//result�p��Sprite����
	thankSp_ = Sprite::Create(11, { WinApp::window_width / 2 - 218, WinApp::window_height / 2 - 50 });

	modelskydome_ = Model::CreateFromOBJ("skydome");
	skydome_ = Object3d::Create();
	//���f�����Z�b�g
	skydome_->SetModel(modelskydome_);

	title_ = new titleScene();
	title_->Initialize(scene_,camera_,light_);

	result_ = new resultScene();
	result_->Initialize();

	//�G��CI�̋����̍Œ჉�C��
	CIStrength = maxCI* 0.85f;
}

void GameScene::Update()
{
	//���̃V�[���Ȃ̂�
	switch (scene_)
	{
	case Scene::Title:
		title_->Update();
		if (input_->TriggerKey(DIK_SPACE) && scene_ == Scene::Title)
		{
			scene_ = Scene::Game;

		}
		break;
	case Scene::Game:
		srand(time(nullptr));

		
		if (isTutorial == true)
		{
			if (input_->TriggerKey(DIK_SPACE))
			{
				isTutorial = false;

			}
		}
		if (isTutorial == false)
		{
			isReady = true;
			if (isReady == true)
			{
				readyTimer--;
			}
		
			if (readyTimer <= 0)
			{
				isReady = false;
				//�����~�߂��Ă����琧�����Ԃ��~�߂�
				if (enemyCIStop == true && isStop == true)
				{
					CITimer = 60;
					timerNum = timerNum;
#pragma region CI���s����

					if (move >= enemyCIMove)
					{
						win = true;
						winNum++;
						enemyWin = false;
					}
					else
					{
						enemyWin = true;
						win = false;

					}
					CINum++;
#pragma endregion
				}
				else
				{
					CITimer--;
					if (CITimer <= 0)
					{
						CITimer = 60;
						timerNum--;
					}
				}



				if (CINum < CIMaxFight)
				{
					if (timerNum > 0)
					{
#pragma region �v���C���[��CI����
						//CI�̏���
						if (isStop == false)//�{�^���������ĂȂ���
						{
							//CI�̑傫���̏���
							if (isGaugeUp == true)
							{
								move += 16.0f;
							}
							else if (isGaugeDown == true)
							{
								move -= 16.0f;
							}
							//CI�̃A�b�v�_�E���̐؂�ւ�
							if (move >= 256)
							{
								move = 256;
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
#pragma endregion

#pragma region �G��CI����

						//CI�̏���
						if (enemyCIStop == false)//�{�^���������ĂȂ���
						{
							//CI�̑傫���̏���
							if (enemyGaugeUp == true)
							{
								enemyCIMove += 24.0f * CISpeed;
							}
							else if (enemyGaugeDown == true)
							{
								enemyCIMove -= 24.0f * CISpeed;
							}
							//CI�̃A�b�v�_�E���̐؂�ւ�
							if (enemyCIMove >= 256.0f)
							{
								enemyCIMove = 256;
								enemyGaugeUp = false;
								enemyGaugeDown = true;
							}
							else if (enemyCIMove <= 0.0f)
							{
								enemyGaugeDown = false;
								enemyGaugeUp = true;
							}
						}
						else
						{
							enemyCIMove = enemyCIMove;
						}
						if (enemyCIMove >= CIStrength)
						{

							enemyCI = rand() % 10 + 1;

							if (enemyCI <= 3)
							{
								if (enemyCIStop == false)
								{
									enemyCIStop = true;
								}
							}


						}
#pragma endregion


					}
					else
					{
						if (enemyCIStop == false)
						{
							enemyCIMove = 0.0f;
						}
						if (isStop == false)
						{
							move = 0.0f;
						}

						CINum++;
					}

				}
				else if (CINum >= CIMaxFight && input_->TriggerKey(DIK_SPACE) && scene_ == Scene::Game)
				{
					timerReset();
					Reset();
					scene_ = Scene::Result;
				}
			}
			
		}
		
		break;
	case Scene::Result:
		result_->Update();
		if (input_->TriggerKey(DIK_SPACE) && scene_ == Scene::Result)
		{
			scene_ = Scene::Title;
		}
		break;
	}

	camera_->Update();

	skydome_->Update();

	light_->Update();

	sprite_->SetSize({ move,128.0f });//   �s�N�Z����/���̃e�N�X�`���T�C�Y
	
	sprite_->SetTextureRect({0.0f,0.0f},{ move,256});

	enemyCI_->SetSize({ enemyCIMove,128.0f });//   �s�N�Z����/���̃e�N�X�`���T�C�Y
	enemyCI_->SetTextureRect({ 0.0f,0.0f }, { enemyCIMove,256 });

	switch (scene_)
	{
	case Scene::Game:
		if (timerNum == 3) {
			Num = Sprite::Create(4, { 1100.0f,500.0f });
		}
		if (timerNum == 2)
		{
			Num = Sprite::Create(5, { 1100.0f,500.0f });
		}
		if (timerNum == 1)
		{
			Num = Sprite::Create(6, { 1100.0f,500.0f });
		}
		
		break;
	}

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
		title_->Draw();
		break;
	case Scene::Game:
		
		break;
	
	}
	skydome_->Draw();
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
		spaceSp_->Draw();
		break;
	case Scene::Game:
		if (isTutorial == false)
		{
			CIBackSprite_->Draw();
			enemyCIBack_->Draw();
			sprite_->Draw();
			vsSprite_->Draw();
			enemyCI_->Draw();
			if (timerNum >= 1)
			{
				Num->Draw();
			}
		}
		if (isTutorial == true)
		{
			tutorialSp_->Draw();
		}
		
		
		if (enemyCIStop == true && isStop == true||timerNum<=0)
		{
			if (move >= enemyCIMove)
			{
				CIWin_->Draw();
			}
			else
			{
				enemyCIWin_->Draw();
			}
		}
		if (isReady == true)
		{
			readySp_->Draw();
		}

		break;
	case Scene::Result:
		result_->Draw();
		thankSp_->Draw();
		spaceSp_->Draw();
		break;
	}

	//
	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::Reset()
{
	move = 0.0f;
	isStop = false;
	enemyCIMove = 0.0f;
	enemyCIStop = false;
	CINum = 0;
	
	isTutorial = true;
	isReady = false;
}

void GameScene::timerReset()
{
	//CI�̃^�C�}�[
	CITimer = 60;
	timerNum = 3;
	CoolTimer = 60;
	readyTimer = 60;

}

GameScene* GameScene::GetInstace()
{
	static GameScene instance;
	return &instance;
}
