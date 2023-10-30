//キーボード入力のクラス
#pragma once
#include"WinApp.h"
#include <Windows.h>
#include<cassert>
#define DIRECTINPUT_VERSION     0x0800   // DirectInputのバージョン指定
#include <dinput.h>
#include <wrl.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

//入力
class Input final
{
private:
	//コンストラクタをprivate
	Input();
	//デストラクタをprivate
	~Input();
public:
	//コピーコンストラクタを無効
	Input(const Input& obj) = delete;
	//代入演算子を無効にする
	Input& operator=(const Input& obj) = delete;


public:	
	//namespace省略
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

public://メンバ関数
	//初期化
	void Initialize(WinApp* winApp);
	//更新
	void Update();
	//キーチェック
	bool PushKey(BYTE keyNumber);
	//キートリガーチェック
	bool TriggerKey(BYTE keyNumber);
	//キーリリースチェック
	bool released(BYTE keyNumber);
	//インスタンス取得
	static Input* GetInstance();

private://メンバ変数
	//キー
	BYTE key[256] = {};
	//前回の全キーの状態
	BYTE keyPre[256] = {};
	ComPtr<IDirectInputDevice8> keyboard=nullptr;
	//WindowsAPI
	WinApp* winApp = nullptr;
};

