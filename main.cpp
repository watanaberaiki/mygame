#include "Input.h"
#include"WinApp.h"
#include"DirectXCommon.h"
#include "CollisionPrimitive.h"
#include "Collision.h"
#include"Camera.h"
#include"FbxLoader.h"
#include"GameScene.h"
#include"2d/PostEffect.h"
#include"2d/SpriteCommon.h"

#pragma comment (lib,"d3dcompiler.lib")

enum Scene {
	title,
	game,
	clear,
	gameover
};

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//　基盤システムの初期化
	//ポインタ
	Input* input = nullptr;
	WinApp* winApp = nullptr;
	DirectXCommon* dxCommon = nullptr;
	PostEffect* postEffect = nullptr;

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


	//入力の初期化
	input = new Input();
	input->Initialize(winApp);

	//3Dオブジェクト静的初期化
	Object3d::StaticInitialize(dxCommon->GetDevice(), winApp->window_width, winApp->window_height);
	ParticleManager::StaticInitialize(dxCommon, winApp->window_width, winApp->window_height);

	/*GameScene* gamescene = nullptr;
	gamescene = new GameScene();
	gamescene->Initialize(dxCommon,input);*/

	//ポストエフェクト用テクスチャの読み込み
	SpriteCommon* spritecommon = nullptr;
	spritecommon = new SpriteCommon();
	spritecommon->Initialize(dxCommon);
	spritecommon->LoadTexture(0, "white1x1.png");
	spritecommon->LoadTexture(1, "white1x1.png");
	/*SpriteCommon::LoadTexture(0, "hit.png");*/
	//ポストエフェクトの初期化
	postEffect = new PostEffect();
	postEffect->SetSpriteCommon(spritecommon);
	postEffect->SetTexture(spritecommon,0);
	postEffect->Initialize();
	postEffect->SetSize(XMFLOAT2(200.0f, 200.0f));
	postEffect->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
	postEffect->SetPosition(XMFLOAT2((float)winApp->window_width / 2, (float)winApp->window_height / 2));
	/*postEffect->SetSize(XMFLOAT2(200.0f, 200.0f));
	postEffect->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
	postEffect->SetPosition(XMFLOAT2((float)winApp->window_width / 2,(float)winApp->window_height/2));*/

	//最初のシーンの初期化

#ifdef _DEBUG

#endif
	//ゲームループ
	while (true) {
		//メッセージがある？
		if (winApp->ProcessMesseage()) {
			//ゲームループを抜ける
			break;
		}
		//入力の更新
		input->Update();
		////ゲームシーン
		//gamescene->Update();
		//// 4.描画コマンドここから

		dxCommon->PreDraw();
		//ポストエフェクトの描画
		postEffect->Draw(dxCommon->GetCommandlist());

		////ゲームシーン
		//gamescene->Draw();

		dxCommon->PostDraw();
		// 4.描画コマンドここまで
	}
	//入力開放
	delete input;
	//WindowsAPIの終了処理
	winApp->Finalize();
	//WindowsAPIの解放
	delete winApp;
	//DirectX解放
	delete dxCommon;
	////GameScene解放
	//delete gamescene;
	//ポストエフェクト解放
	delete postEffect;
	//カメラ解放
	/*delete camera;*/
	FbxLoader::GetInstance()->Finalize();
	//基盤システムの終了

	return 0;
}

