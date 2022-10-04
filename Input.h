#pragma once
#include <cassert>

#define DIRECTINPUT_VERSION  0x0800
#include <dinput.h>
#include <windows.h>
#include <wrl.h>

using namespace Microsoft::WRL;

#pragma comment(lib,"dinput8.lib")//directInput�̃o�[�W����
#pragma comment(lib,"dxguid.lib")

class Input
{
	//�����o�֐�
public:
	/// <summary>
	/// ������
	/// </summary>
	void Initialize(HINSTANCE hInstance, HWND hwnd);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	//�����o�ϐ�
private:

};