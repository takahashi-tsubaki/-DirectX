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
	// カメラ注視点をセット
	camera_->SetTarget({ 0, 1, 0 });
	// 3Dオブジェクトにカメラをセット
	Object3d::SetCamera(camera_);
	//ライト生成
	light_ = Light::Create();
	//ライト色を設定
	light_->SetLightColor({ 1,1,1 });
	//3Dオブジェクトにライトをセット
	Object3d::SetLight(light_);


	Sprite::LoadTexture(1, L"Resources/kuribo-.jpg");
	sprite_ = Sprite::Create(1, { WinApp::window_width/2 - 500 , WinApp::window_height/2});

	
	
	

	modelskydome_ = Model::CreateFromOBJ("skydome");
	skydome_ = Object3d::Create();
	//モデルをセット
	skydome_->SetModel(modelskydome_);

	title_ = new titleScene();
	title_->Initialize(scene_);

}

void GameScene::Update()
{
	//何のシーンなのか
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

		//CIの処理
		if (isStop == false)//ボタンを押してない時
		{
			//CIの大きさの処理
			if (isGaugeUp == true)
			{
				move+= 8.0f;
			}
			else if (isGaugeDown == true)
			{
				move-= 8.0f;
			}
			//CIのアップダウンの切り替え
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
		//CIを止める処理
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

	

	sprite_->SetSize({ move,128.0f });//   ピクセル分/元のテクスチャサイズ
	
	sprite_->SetTextureRect({0.0f,0.0f},{ move,256});
}

void GameScene::Draw()
{
#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(dxCommon_->GetCommandList());
	// 背景スプライト描画

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(dxCommon_->GetCommandList());

	switch (scene_)
	{
	case Scene::Title:
		skydome_->Draw();
		break;
	case Scene::Game:
		
		break;
	}

	// 3Dオブジェクトの描画
	
	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(dxCommon_->GetCommandList());

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
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
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
