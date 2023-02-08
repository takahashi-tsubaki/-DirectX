#pragma once
#include "../Core/DirectXCommon.h"
#include "../input/Input.h"

#include "../2D/Sprite.h"

#include "../3D/Model.h"
#include "../3D/Object3d.h"
#include "../3D/Camera.h"
#include "../3D/Light.h"

#include "../collision/collisionPrimitive.h"
#include "../collision/Collision.h"

class GameScene
{

public:

	GameScene();
	~GameScene();

	void Initalize();

	void Update();

	void Draw();

	void Down();

	void Up();

	

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

	Model* modelGround_ = nullptr;
	Object3d* ground_ = nullptr;

	Model* changeSphere = nullptr;
	Object3d* changeModel = nullptr;

	Sphere sphere;
	Plane plane;

	XMVECTOR moveY;

	bool isDown = true;
	bool isUp = false;

	float posY = 0.0f;
};