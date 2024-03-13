#include "GameOverScene.h"

void GameOverScene::Initialize(DirectXCommon* dxCommon, ImguiManager* imgui)
{
	dxCommon_ = dxCommon;
	imgui_ = imgui;
	input_ = Input::GetInstance();
	eye = XMFLOAT3(0, 0, 0);	//視点座標
	target = XMFLOAT3(0, 0, 10);	//注視点座標
	up = XMFLOAT3(0, 1, 0);		//上方向ベクトル
	//カメラ
	camera->Initialize();
	camera->SetEye(eye);
	camera->SetTarget(target);
	camera->SetUp(up);
	camera->Update();
	//パーティクルマネージャカメラセット
	ParticleManager::SetCamera(camera);
	//スプライト共通部の初期化
	spriteCommon->Initialize(dxCommon);
	//テクスチャ読み込み
	spriteCommon->LoadTexture(1, "white1x1.png");
	spriteCommon->LoadTexture(2, "gameover.png");
	//スプライトにテクスチャ割り当て
	transitionWhiteSprite->Initialize(spriteCommon, 1);
	gameOverSprite->Initialize(spriteCommon, 2);

	//真ん中
	const XMFLOAT2 center = { 0.5f,0.5f };
	//画面遷移用スプライト
	transitionWhiteSprite->SetAnchorPoint(center);
	transitionWhiteSprite->SetSize(XMFLOAT2(WinApp::window_width, WinApp::window_height));
	transitionWhiteSprite->SetPosition(XMFLOAT2(WinApp::window_width/2, WinApp::window_height/2));
	transitionWhiteSprite->Update();

	//パーティクル
	particles->Initialize("line.png");

	isBackTransition = true;
}

void GameOverScene::Finalize()
{
}

void GameOverScene::Update()
{
	//カメラ更新
	camera->Update();
	//戻っている最中は押しても反応しない
	if (isBackTransition) {

	}
	else
	{
		//タイトルシーンチェンジ
		if (input_->TriggerKey(DIK_SPACE) || input_->TriggerPadButton(XINPUT_GAMEPAD_A)) {
			isTransition = true;
			nextScene = Title;
		}
	}
	
	//パーティクル
	particles->Update();
	//遷移演出用
	if (isTransition || isBackTransition) {
		Transition(nextScene);
	}
	//スプライト
	gameOverSprite->Update();
}

void GameOverScene::Draw()
{
	//遷移時のパーティクル
	particles->Draw();

	//スプライト描画
	spriteCommon->PreDraw();

	gameOverSprite->Draw();

	//遷移演出
	transitionWhiteSprite->Draw();

	spriteCommon->PostDraw();

	
}
