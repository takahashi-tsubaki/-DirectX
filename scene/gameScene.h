#pragma once
#include "../Core/DirectXCommon.h"
#include "../input/Input.h"

#include "../2D/Sprite.h"

#include "../3D/Model.h"
#include "../3D/Object3d.h"
#include "../3D/Camera.h"
#include "../3D/Light.h"

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

	Sprite* sprite_ = nullptr;


	Model* modelskydome_ = nullptr;
	Object3d* skydome_ = nullptr;

	Model* modelPlayer_ = nullptr;
	Object3d* player_ = nullptr;

};