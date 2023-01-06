#include "Input.h"
#include "Core/WinApp.h"
#include "Core/DirectXCommon.h"
#include "Core/FPS.h"

#include "2D/Sprite.h"
#include "2D/SpriteManager.h"

#include "3D/Object3d.h"

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
	spManager = new SpriteManager;
	spManager->Initialize(dxCommon);

	Object3d::StaticInitialize(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_height);

	Object3d* obj3d = nullptr;

	obj3d = Object3d::Create();

	Object3d* obj3d2 = nullptr;
	obj3d2 = Object3d::Create();
	obj3d2->SetPosition({ 30,0,0 });

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

		obj3d->Update();
		obj3d2->Update();
		//描画前処理
		dxCommon->preDraw();

		//描画処理
		spManager->Draw();

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
	winApp = nullptr;
	delete dxCommon;
	delete obj3d;
	delete obj3d2;
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