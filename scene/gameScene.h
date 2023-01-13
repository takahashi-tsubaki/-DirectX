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

	//�X�v���C�g�n
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

	//���f���n
	Model* modelskydome_ = nullptr;
	Object3d* skydome_ = nullptr;

	//�V�[���n��
	titleScene* title_ = nullptr;//�^�C�g���V�[��
	resultScene* result_ = nullptr;
	Scene scene_ = Scene::Title;


	float move = 0.0f;//CI�̕ϓ��l

	XMFLOAT2 CIWinPos = { WinApp::window_width / 2 - 400,WinApp::window_height / 2-128 };

	XMFLOAT2 EnemyCIWinPos = { WinApp::window_width / 2 + 150,WinApp::window_height / 2 - 128 };

	int readyTimer = 60;

	//CI�̃^�C�}�[
	int CITimer = 60;
	int timerNum = 3;

	int CoolTimer = 60;

	int isTutorial = true;

	int isReady = false;

#pragma region �G�֘A

	int enemyWin = false;

	float maxCI = 256.0f;

	float enemyCIMove = 0.0f;

	//�G��CI�̂悳
	float CIStrength = 0.0f;

	int enemyCI = false;
	//CI�̃t���O�n��
	int enemyCIStop = false;//
	int enemyGaugeUp = true;//�Q�[�W�A�b�v
	int enemyGaugeDown = false;//�Q�[�W�_�E��
#pragma endregion

#pragma region �v���C���[CI�֘A

	int win = false;

	int winNum = 0;

	int CIMaxFight = 1;//��������ő吔
	int CINum = 0;//���݂̉�

	int CISpeed = 1.0f;//CI�̃X�s�[�h
	int isStop = false;//CI���~�܂��Ă��邩
	//CI�̃A�b�v�_�E��
	int isGaugeUp = true;
	int isGaugeDown = false;
#pragma endregion
};