#include "Input.h"

Input::Input() {

}

Input::~Input()
{
}

void Input::Initialize(WinApp* winApp_)
{
	//借りてきたWinAppのインスタンスを記録
	this->winApp = winApp_;

	HRESULT result;

	// DirectInputの初期化
	ComPtr<IDirectInput8> directInput = nullptr;
	result = DirectInput8Create(
		winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	// キーボードデバイスの生成
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	// 入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
		winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	//パッド


}

void Input::Update() 
{
	HRESULT result;

	//前回のキー入力を保存
	memcpy(keyPre, key, sizeof(key));

	// キーボード情報の取得開始
	result=keyboard->Acquire();
	// 全キーの入力状態を取得する
	result=keyboard->GetDeviceState(sizeof(key), key);

	//パッドの接続確認
	oldPadState = padState;
	DWORD flag =XInputGetState(0, &padState);

	//接続成功
	if (flag == ERROR_SUCCESS) {

	}
	//接続失敗
	else {
	
	}



}

bool Input::PushKey(BYTE keyNumber) 
{
	//指定キーを押していればtrueを返す
	if (key[keyNumber]) {
		return true;
	}

	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	if (keyPre[keyNumber] == false && key[keyNumber]) {
		return true;
	}

	return false;
}

bool Input::ReleasedKey(BYTE keyNumber)
{
	if (keyPre[keyNumber] && key[keyNumber] == false) {
		return true; 
	}
	return false;
}

bool Input::PushPadButton(WORD Button)
{
	return (padState.Gamepad.wButtons & Button);
}

bool Input::TriggerPadButton(WORD Button)
{

	return (padState.Gamepad.wButtons & Button) && ((oldPadState.Gamepad.wButtons & Button) != Button);
}

bool Input::ReleasedPadButton(WORD Button)
{
	return (oldPadState.Gamepad.wButtons & Button) && ((padState.Gamepad.wButtons & Button) != Button);
}

bool Input::TriggerLButton()
{
	return (padState.Gamepad.bLeftTrigger>0)&&(oldPadState.Gamepad.bLeftTrigger==0);
}

bool Input::TriggerRButton()
{
	return  (padState.Gamepad.bRightTrigger > 0) && (oldPadState.Gamepad.bRightTrigger == 0);
}

bool Input::LStickLeft(int deadZone)
{
	return padState.Gamepad.sThumbLX < -deadZone;
}

bool Input::LStickRight(int deadZone)
{
	return padState.Gamepad.sThumbLX > deadZone;
}

bool Input::LStickUp(int deadZone)
{
	return padState.Gamepad.sThumbLY > deadZone;
}

bool Input::LStickDown(int deadZone)
{
	return padState.Gamepad.sThumbLY <- deadZone;
}

bool Input::RStickLeft(int deadZone)
{
	return padState.Gamepad.sThumbRX < -deadZone;
}

bool Input::RStickRight(int deadZone)
{
	return padState.Gamepad.sThumbRX > deadZone;
}

bool Input::RStickUp(int deadZone)
{
	return padState.Gamepad.sThumbRY > deadZone;
}

bool Input::RStickDown(int deadZone)
{
	return padState.Gamepad.sThumbRY < -deadZone;
}

Input* Input::GetInstance()
{
	static Input instance;
	return &instance;
}
