#include "titleScene.h"

titleScene::~titleScene()
{
	titleModel_;
	titleObj_;
}

void titleScene::Initialize(Scene scene,Camera* camera,Light* light)
{
	input_ = Input::GetInstance();
	scene_ = scene;
	camera_ = camera;
	light_ = light;

	titleModel_ = Model::CreateFromOBJ("title");
	titleObj_ = Object3d::Create();

	titleObj_->SetModel(titleModel_);

	titleObj_->SetScale({3.0f,3.0f,0.0f});

	titleObj_->SetPosition({-5,5,0.0f});

}

void titleScene::Update()
{
	titleObj_->Update();
}

void titleScene::Draw()
{
	titleObj_->Draw();
}
