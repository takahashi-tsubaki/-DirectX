#include "Input.h"
#include "Core/WinApp.h"
#include "Core/DirectXCommon.h"
#include "Core/FPS.h"

#include "2D/Sprite.h"
#include "2D/SpriteManager.h"

#include "3D/Object3d.h"
#include "3D/Model.h"
#include "3D/Camera.h"

void DebugOutputFormatString(const char* format, ...) {
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	vprintf(format, valist);
	va_end(valist);
#endif
}

//関数のプロトタイプ宣言
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	OutputDebugStringA("Hello DirectX!!\n");


	//FPS
	FPS* fps = new FPS;

	WinApp* winApp = nullptr;
	winApp = new WinApp();
	winApp->Initialize();

	Input* input = nullptr;
	input = new Input();
	input->Initialize(winApp);

	DirectXCommon* dxCommon = nullptr;
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(winApp);

	SpriteManager* spManager = nullptr;
	spManager = SpriteManager::GetInstance();
	spManager->Initialize(dxCommon);



	//情的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(),WinApp::window_width,WinApp::window_height);

	// 3Dオブジェクト静的初期化
	Object3d::StaticInitialize(dxCommon->GetDevice());

	//ライト情的初期化
	Light::StaticInitalize(dxCommon->GetDevice());

	Sprite* sprite = nullptr;

	Sprite::LoadTexture(1, L"Resources/kuribo-.jpg");

	Sprite::LoadTexture(2, L"Resources/mario.jpg");

	sprite = Sprite::Create(1,{0.0f,0.0f});

	Model* model = nullptr;
	Object3d* objModel = nullptr;

	Model* enemymModel = nullptr;
	Object3d* objEnemy = nullptr;

	objModel = Object3d::Create();
	model = Model::CreateFromOBJ("skydome");

	objEnemy = Object3d::Create();
	enemymModel = Model::CreateFromOBJ("enemy");

	objModel->SetModel(model);

	objEnemy->SetModel(enemymModel);

	Light* light = nullptr;
	//ライト生成
	light = Light::Create();
	//ライト色を設定
	light->SetLightColor({ 1,1,1 });
	//3Dオブジェクトにライトをセット
	Object3d::SetLight(light);

	Camera* camera = nullptr;

	camera = new Camera(WinApp::window_width,WinApp::window_height);
	// カメラ注視点をセット
	camera->SetTarget({ 0, 1, 0 });

	// 3Dオブジェクトにカメラをセット
	Object3d::SetCamera(camera);
	/*sprite->SetSize({ 1.0f,1.0f });*/


	/*sprite->SetPosition({0.1f,0.1f});*/


	//ここからDirectX初期化処理


	//ここから描画初期化処理


	//ここまで描画初期化処理


	//ゲームループ
	while (true)
	{

		fps->FpsControlBegin();
		if (winApp->ProcessMessage())
		{
			//ゲームループを抜ける
			break;
		}
		//ここからDirectX毎フレーム処理

		spManager->Update();

		camera->Update();

		objModel->Update();
		objEnemy->Update();
		light->Update();

		//描画前処理
		dxCommon->preDraw();

		//描画処理
#pragma region 背景スプライト描画
// 背景スプライト描画前処理
		Sprite::PreDraw(dxCommon->GetCommandList());
		// 背景スプライト描画
		/*spriteBG->Draw();*/
		sprite->Draw();
		/// <summary>
		/// ここに背景スプライトの描画処理を追加できる
		/// </summary>

		// スプライト描画後処理
		Sprite::PostDraw();
#pragma endregion
		
#pragma region 3Dオブジェクト描画
		// 3Dオブジェクト描画前処理
		Object3d::PreDraw(dxCommon->GetCommandList());

		// 3Dオブクジェクトの描画
		objModel->Draw();
		objEnemy->Draw();
		/// <summary>
		/// ここに3Dオブジェクトの描画処理を追加できる
		/// </summary>

		// 3Dオブジェクト描画後処理
		Object3d::PostDraw();
#pragma endregion
		//描画後処理
		dxCommon->postDraw();


		fps->FpsControlEnd();
		//ここまでDirectX毎フレーム処理
	}
	//windowAPIの終了処理
	winApp->Finalize();
	//入力解放
	delete input;
	delete winApp;
	/*delete dxCommon;*/
	/*delete spManager;
	delete sprite;*/

	//input = nullptr;
	//winApp = nullptr;
	/*dxCommon = nullptr;*/
	//spManager = nullptr;
	//sprite = nullptr;
	///*delete obj3d;
	//delete obj3d2;*/
	return 0;
}

LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して,アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}