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
#include "resultScene.h"
#include <random>

class GameScene
{

public:

	GameScene();
	~GameScene();
	
	void Initialize();

	void Update();

	void Draw();

	void Reset();

	void timerReset();
	
	int GetWinNum() { return winNum; }

	static GameScene* GetInstace();

private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Camera* camera_ = nullptr;
	Light* light_ = nullptr;
	Audio* audio_ = nullptr;

	//スプライト系
	Sprite* sprite_ = nullptr;
	Sprite* CIBackSprite_ = nullptr;
	Sprite* enemyCIBack_ = nullptr;
	Sprite* enemyCI_ = nullptr;
	Sprite* CIWin_ = nullptr;
	Sprite* enemyCIWin_ = nullptr;
	Sprite* Num = nullptr;
	Sprite* vsSprite_ = nullptr;
	Sprite* readySp_ = nullptr;
	Sprite* tutorialSp_ = nullptr;
	Sprite* spaceSp_ = nullptr;
	Sprite* thankSp_ = nullptr;

	//モデル系
	Model* modelskydome_ = nullptr;
	Object3d* skydome_ = nullptr;

	//シーン系統
	titleScene* title_ = nullptr;//タイトルシーン
	resultScene* result_ = nullptr;
	Scene scene_ = Scene::Title;


	float move = 0.0f;//CIの変動値

	XMFLOAT2 CIWinPos = { WinApp::window_width / 2 - 400,WinApp::window_height / 2-128 };

	XMFLOAT2 EnemyCIWinPos = { WinApp::window_width / 2 + 150,WinApp::window_height / 2 - 128 };

	int readyTimer = 60;

	//CIのタイマー
	int CITimer = 60;
	int timerNum = 3;

	int CoolTimer = 60;

	int isTutorial = true;

	int isReady = false;

#pragma region 敵関連

	int enemyWin = false;

	float maxCI = 256.0f;

	float enemyCIMove = 0.0f;

	//敵のCIのつよさ
	float CIStrength = 0.0f;

	int enemyCI = false;
	//CIのフラグ系統
	int enemyCIStop = false;//
	int enemyGaugeUp = true;//ゲージアップ
	int enemyGaugeDown = false;//ゲージダウン
#pragma endregion

#pragma region プレイヤーCI関連

	int win = false;

	int winNum = 0;

	int CIMaxFight = 1;//勝負する最大数
	int CINum = 0;//現在の回数

	int CISpeed = 1.0f;//CIのスピード
	int isStop = false;//CIが止まっているか
	//CIのアップダウン
	int isGaugeUp = true;
	int isGaugeDown = false;
#pragma endregion
};