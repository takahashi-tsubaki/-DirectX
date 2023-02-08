#include "gameScene.h"

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initalize()
{
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
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
	sprite_ = Sprite::Create(1, { 0.0f,0.0f });

	modelskydome_ = Model::CreateFromOBJ("sphere");
	skydome_ = Object3d::Create();
	//モデルをセット
	skydome_->SetModel(modelskydome_);
	skydome_->SetPosition({ -5,0,-10 });



	modelPlayer_ = Model::CreateFromOBJ("sphere");
	player_ = Object3d::Create();
	//モデルをセット
	player_->SetModel(modelPlayer_);
	player_->SetPosition({ 6,0,0 });

	modelGround_ = Model::CreateFromOBJ("Wall");
	ground_ = Object3d::Create();
	ground_->SetModel(modelGround_);
	ground_->SetPosition({0,-5,0});
	ground_->SetScale({ 1,0.1f,0.1f });

	changeSphere = Model::CreateFromOBJ("skydome");
	
	//球の座標をプレイヤーに置き換え
	sphere.center = XMVectorSet(0,5,0,1);
	sphere.radius = 1.0f;

	//平面の座標の置き換え
	plane.normal = XMVectorSet(0,1,0,0);
	plane.distance = 0.0f;
}

void GameScene::Update()
{
	moveY = XMVectorSet(0,0.1f,0,0);
	if (isDown == true)
	{
		Down();
	}
	if (isUp == true)
	{
		Up();
	}

	bool isHit = Collision::CheckSphere2Plane(sphere, plane);
	if (isHit)
	{
		player_->SetColor({1,0,0,1});
	}
	else
	{
		player_->SetColor({1,1,1,1});
	}
	
	camera_->Update();

	skydome_->Update();

	player_->Update();

	ground_->Update();

	light_->Update();
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

	// 3Dオブジェクトの描画
	/*skydome_->Draw();*/

	player_->Draw();

	ground_->Draw();

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
	/*sprite_->Draw();*/
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::Down()
{
	sphere.center -= moveY;
	posY -= 0.1f;
	player_->SetPosition({ player_->GetPosition().x,posY,player_->GetPosition().z });
	if (posY < -5)
	{
		isUp = true;
		isDown = false;
	}
}

void GameScene::Up()
{
	sphere.center += moveY;
	posY += 0.1f;
	player_->SetPosition({ player_->GetPosition().x,posY,player_->GetPosition().z });
	if (posY > 5)
	{
		isUp = false;
		isDown = true;
	}
}

