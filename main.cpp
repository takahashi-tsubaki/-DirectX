
#include "Input.h"
#include "Core/WinApp.h"
#include "Core/DirectXCommon.h"
#include "Core/FPS.h"

#include "2D/Sprite.h"
#include "2D/SpriteManager.h"

#include"3D/Object3d.h"

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
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	// 3Dオブジェクト静的初期化
	Object3d::StaticInitialize(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_height);

	SpriteManager* spManager = nullptr;
	spManager = new SpriteManager;
	spManager->Initialize(dxCommon);
	
	Sprite* sprite = new Sprite();
	sprite->Initialize(spManager);

	Object3d* obj3d = nullptr;
	
	obj3d = Object3d::Create();

	Object3d* obj3d2 = nullptr;
	obj3d2 = Object3d::Create();
	obj3d2->SetPosition({30,0,0});
	
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


	/*	spManager->Draw();*/
#pragma region 3Dオブジェクト描画
// 3Dオブジェクト描画前処理
		Object3d::PreDraw(dxCommon->GetCommandList());

		// 3Dオブクジェクトの描画
		obj3d->Draw();
		obj3d2->Draw();
		/// <summary>
		/// ここに3Dオブジェクトの描画処理を追加できる
		/// </summary>

		// 3Dオブジェクト描画後処理
		Object3d::PostDraw();
#pragma endregion

		//　４．ここまで描画コマンド
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
