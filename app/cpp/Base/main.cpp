#include "Input.h"
#include"WinApp.h"
#include"DirectXCommon.h"
#include"Camera.h"
#include"FbxLoader.h"
#include"GameScene.h"
#include"ImGuiManager.h"

#pragma comment (lib,"d3dcompiler.lib")

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//　基盤システムの初期化
	//ポインタ
	Input* input = Input::GetInstance();
	WinApp* winApp = nullptr;
	DirectXCommon* dxCommon = nullptr;


	//WindowsAPIの初期化
	winApp = new WinApp();
	winApp->Initialize();

#ifdef _DEBUG
	ID3D12Debug1* debugController = nullptr;
	if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		//デバッグレイヤーを有効にする
		debugController->EnableDebugLayer();
		//さらにGPU側でもチェックを行えるようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif
	//DirectXの初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);
#ifdef  _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(dxCommon->GetDevice()->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//ヤバいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		//解放
		infoQueue->Release();
		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
	}
#endif 
	FbxLoader::GetInstance()->Initialize(dxCommon->GetDevice());

	//ImGuiの初期化
	ImguiManager* imgui = new ImguiManager();
	imgui->Initialize(winApp,dxCommon);

	//入力の初期化
	input->Initialize(winApp);

	//3Dオブジェクト静的初期化
	Object3d::StaticInitialize(dxCommon->GetDevice(), winApp->window_width, winApp->window_height);
	WireObject::StaticInitialize(dxCommon->GetDevice(), winApp->window_width, winApp->window_height);

	ParticleManager::StaticInitialize(dxCommon, winApp->window_width, winApp->window_height);

	GameScene* gamescene = nullptr;
	gamescene = new GameScene();
	gamescene->Initialize(dxCommon,imgui);


	//最初のシーンの初期化
	// 
	//ゲームループ
	while (true) {
		//メッセージがある？
		if (winApp->ProcessMesseage()) {
			//ゲームループを抜ける
			break;
		}
		//入力の更新
		input->Update();
		//ゲームシーン更新
		gamescene->Update();
		
		//// 4.描画コマンドここから

		dxCommon->PreDraw();
		
		//ゲームシーン
		gamescene->Draw();
#ifdef _DEBUG

		//imgui
		imgui->Draw();

#endif
		dxCommon->PostDraw();
		// 4.描画コマンドここまで
	}
	//WindowsAPIの終了処理
	winApp->Finalize();
	//imguiの終了処理
	imgui->Finalize();
	//WindowsAPIの解放
	delete winApp;
	//DirectX解放
	delete dxCommon;
	//GameScene解放
	delete gamescene;
	//カメラ解放
	/*delete camera;*/
	FbxLoader::GetInstance()->Finalize();
	//基盤システムの終了

	return 0;
}

