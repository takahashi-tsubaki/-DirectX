#pragma once

#include "../Global.h"

#include "../Core/DirectXCommon.h"
#include "../Input.h"
#include "../Audio.h"

#include "../2D/Sprite.h"

#include "../3D/Model.h"
#include "../3D/Object3d.h"
#include "../3D/Camera.h"
#include "../3D/Light.h"

#include "titleScene.h"

class GameScene
{

public:

	GameScene();
	~GameScene();

	void Initalize();

	void Update();

	void Draw();

private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Camera* camera_ = nullptr;
	Light* light_ = nullptr;
	Audio* audio_ = nullptr;

	Sprite* sprite_ = nullptr;

	Model* modelskydome_ = nullptr;
	Object3d* skydome_ = nullptr;

	titleScene* title_ = nullptr;//タイトルシーン

	Scene scene_ = Scene::Title;

	float move = 0.0f;//CIの変動値
	
	int CISpeed = 1.0f;//CIのスピード

	int isStop = false;//CIが止まっているか

	//CIのアップダウン
	int isGaugeUp = true;
	int isGaugeDown = false;

};