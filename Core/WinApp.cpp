#include "WinApp.h"
LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//メッセージ分岐
	switch (msg)
	{
	case WM_DESTROY://ウィンドウが破棄された

		PostQuitMessage(0);// OSに対して、アプリの終了を伝える
		return 0;

	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//標準の処理を行う
}
void WinApp::Initialize()
{

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;//コールバック関数の指定
	w.lpszClassName = _T("DirectXTest");//アプリケーションクラス名(適当でいいです)
	w.hInstance = GetModuleHandle(0);//ハンドルの取得
	w.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&w);//アプリケーションクラス(こういうの作るからよろしくってOSに予告する)
	
	RECT wrc = { 0,0,window_width, window_height };
	//関数を使ってウィンドウのサイズを補正する
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウオブジェクトの生成
	hwnd = CreateWindow(w.lpszClassName,//クラス名指定
	_T("DX12Test"),//タイトルバーの文字
	WS_OVERLAPPEDWINDOW,//タイトルバーと境界線があるウィンドウ
	CW_USEDEFAULT,//表示x座標はOSにお任せ
	CW_USEDEFAULT,//表示y座標はOSにお任せ
	wrc.right - wrc.left,//ウィンドウ幅
	wrc.bottom - wrc.top,//ウィンドウ高
	nullptr,//親ウィンドウハンドル
	nullptr,//メニューハンドル
	w.hInstance,//呼び出しアプリケーションハンドル
	nullptr);//追加パラメーター

	ShowWindow(hwnd, SW_SHOW);

}

void WinApp::Update()
{

}

void WinApp::Finalize()
{
	//ウィンドウクラスの登録解除
	UnregisterClass(w.lpszClassName, w.hInstance);
}

bool WinApp::ProcessMessage()
{
	MSG msg{};
	//ウィンドウメッセージ処理
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}

	if (msg.message == WM_QUIT)
	{
		return true;
	}

	return false;
}