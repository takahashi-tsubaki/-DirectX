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

	titleScene* title_ = nullptr;//�^�C�g���V�[��

	Scene scene_ = Scene::Title;

	float move = 0.0f;//CI�̕ϓ��l
	
	int CISpeed = 1.0f;//CI�̃X�s�[�h

	int isStop = false;//CI���~�܂��Ă��邩

	//CI�̃A�b�v�_�E��
	int isGaugeUp = true;
	int isGaugeDown = false;

};