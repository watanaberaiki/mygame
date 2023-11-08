#pragma once
#include <dwrite.h>
#include <wchar.h>
#include <wrl/client.h>
#include <random>
#include <xinput.h>
#include "WinApp.h"

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")
#pragma comment (lib, "xinput.lib")

class DxInput
{
public://メンバ関数
	//更新処理
	void Update();


private:
	XINPUT_STATE state; // コントローラーの状態の取得

};

