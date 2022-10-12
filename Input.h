#pragma once
#include <cassert>

#define DIRECTINPUT_VERSION  0x0800
#include <dinput.h>
#include <windows.h>
#include <wrl.h>

#pragma comment(lib,"dinput8.lib")//directInput�̃o�[�W����
#pragma comment(lib,"dxguid.lib")

class Input
{
	//�����o�֐�
public:

	template<class Type> using ComPtr = Microsoft::WRL::ComPtr<Type>;

	/// <summary>
	/// ������
	/// </summary>
	void Initialize(HINSTANCE hInstance, HWND hwnd);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// 
	/// </summary>
	/// <returns>������Ă��邩</returns>
	bool PushKey(BYTE keyNum);

	/// <summary>
	/// 
	/// </summary>
	/// <returns>��������</returns>
	bool TriggerKey(BYTE keyNum);

	

	//�����o�ϐ�
private:

	//DirectInput�̏�����
	ComPtr<IDirectInput8> directInput = nullptr;

	//�L�[�{�[�h�f�o�C�X�̐���
	ComPtr<IDirectInputDevice8> keyboard = nullptr;

	BYTE keys[256] = {};
	BYTE oldkeys[256] = {};


};