#include "DxInput.h"

void DxInput::Update()
{
	//状態確保
	XInputGetState(0, &state);
	int pad_A = 0, pad_B = 0, pad_X = 0, pad_Y = 0;
	int pad_leftShoulder = 0, pad_rightShoulder = 0;
	int pad_left = 0, pad_right = 0, pad_up = 0, pad_down = 0;

	/////////////////////////
	//ゲームパッドボタン関係///
	/////////////////////////
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) pad_left = 1;               //ゲームパッド十字キー左
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) pad_right = 1;             //ゲームパッド十字キー右
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) pad_up = 1;                   //ゲームパッド十字キー上
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) pad_down = 1;               //ゲームパッド十字キー下
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) pad_A = 1;                          //ゲームパッドA
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) pad_B = 1;                          //ゲームパッドB
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) pad_X = 1;                          //ゲームパッドX
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) pad_Y = 1;                          //ゲームパッドY
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) pad_leftShoulder = 1;   //ゲームパッドL
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) pad_rightShoulder = 1; //ゲームパッドR


	/////////////////////////////
	//ゲームパッドスティック関係///
	////////////////////////////
	// ゲームパッドデッドゾーン処理
	if ((state.Gamepad.sThumbLX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
		state.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
		(state.Gamepad.sThumbLY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
			state.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
	{
		state.Gamepad.sThumbLX = 0;
		state.Gamepad.sThumbLY = 0;
	}
	if ((state.Gamepad.sThumbRX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
		state.Gamepad.sThumbRX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
		(state.Gamepad.sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
			state.Gamepad.sThumbRY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
	{
		state.Gamepad.sThumbRX = 0;
		state.Gamepad.sThumbRY = 0;
	}
}
