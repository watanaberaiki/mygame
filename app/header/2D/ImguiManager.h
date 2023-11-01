#include"imgui.h"
#include"WinApp.h"
#include"DirectXCommon.h"
#pragma once
class ImguiManager
{
public://メンバ関数
	//初期化
	void Initialize(WinApp*winApp,DirectXCommon* dxCommon);
	//終了処理
	void Finalize();
	//ImGui受付開始
	void Begin();
	//ImGui受付終了
	void End();
	//画面への描画
	void Draw();

private://メンバ変数
	//SRV用デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>srvHeap;
	DirectXCommon* dxCommon;

};

