#pragma once
#include <cassert>

#define DIRECTINPUT_VERSION  0x0800
#include <dinput.h>
#include <windows.h>
#include <wrl.h>

using namespace Microsoft::WRL;

#pragma comment(lib,"dinput8.lib")//directInputのバージョン
#pragma comment(lib,"dxguid.lib")

class Input
{
	//メンバ関数
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(HINSTANCE hInstance, HWND hwnd);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	//メンバ変数
private:

};