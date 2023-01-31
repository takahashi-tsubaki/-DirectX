#include "../input/Input.h"
#include "Core/WinApp.h"
#include "Core/DirectXCommon.h"
#include "Core/FPS.h"
#include "scene/gameScene.h"

#include "../audio/Audio.h"

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
	Input* input = nullptr;
	DirectXCommon* dxCommon = nullptr;
	Audio* audio = nullptr;
	GameScene* gameScene = nullptr;

	winApp = new WinApp();
	winApp->Initialize();

	input = new Input();
	input->Initialize(winApp);

	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(winApp);

	audio = new Audio();
	audio->Initialize();
	audio->LoadWave("se_amd06.wav");

	//情的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_height);
	// 3Dオブジェクト静的初期化
	Object3d::StaticInitialize(dxCommon->GetDevice());
	//ライト情的初期化
	Light::StaticInitalize(dxCommon->GetDevice());

	gameScene = new GameScene();
	gameScene->Initalize();

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

		input->Update();

		gameScene->Update();

		if (input->TriggerKey(DIK_SPACE)) {
			audio->PlayWave("se_amd06.wav");
		}

		//描画前処理
		dxCommon->preDraw();

		//ゲームシーンの描画
		gameScene->Draw();

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

	audio->Finalize();
	delete audio;

	delete gameScene;


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