#pragma once
#include "../Input.h"
#include "../Global.h"

#include "../3D/Model.h"
#include "../3D/Object3d.h"
#include "../3D/Camera.h"
#include "../3D/Light.h"
class titleScene
{
public:

	~titleScene();

	void Initialize(Scene scene, Camera* camera, Light* light);

	void Update();

	void Draw();

private:
	Input* input_ = nullptr;

	Scene scene_;

	Model* titleModel_ = nullptr;
	Object3d* titleObj_ = nullptr;

	Camera* camera_ = nullptr;
	Light* light_ = nullptr;

};