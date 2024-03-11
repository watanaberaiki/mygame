#include "ClearScene.h"

void ClearScene::Initialize(DirectXCommon* dxCommon, ImguiManager* imgui)
{
	dxCommon_ = dxCommon;
	imgui_ = imgui;
	input_ = Input::GetInstance();
	//isBackTransition = true;

	//スプライト共通部の初期化
	spriteCommon->Initialize(dxCommon);
	//テクスチャ読み込み
	spriteCommon->LoadTexture(0, "hit.png");
	spriteCommon->LoadTexture(1, "mario.jpg");
	spriteCommon->LoadTexture(2, "menu.png");
	spriteCommon->LoadTexture(3, "boss.png");
	spriteCommon->LoadTexture(4, "title.png");
	spriteCommon->LoadTexture(5, "start.png");
	spriteCommon->LoadTexture(6, "black1x1.png");
	spriteCommon->LoadTexture(7, "white1x1.png");
	spriteCommon->LoadTexture(8, "gameover.png");
	spriteCommon->LoadTexture(9, "clear.png");
	spriteCommon->LoadTexture(10, "stickR.png");
	spriteCommon->LoadTexture(11, "stickL.dds");
	spriteCommon->LoadTexture(12, "buttonRT.png");
	//スプライトにテクスチャ割り当て
	hitSprite->Initialize(spriteCommon, 0);
	marioSprite->Initialize(spriteCommon, 1);
	menuSprite->Initialize(spriteCommon, 2);
	titleSprite->Initialize(spriteCommon, 4);
	startSprite->Initialize(spriteCommon, 5);
	blackSprite->Initialize(spriteCommon, 6);
	transitionWhiteSprite->Initialize(spriteCommon, 7);
	gameOverSprite->Initialize(spriteCommon, 8);
	clearWhiteSprite->Initialize(spriteCommon, 7);
	clearSprite->Initialize(spriteCommon, 9);
	stickRSprite->Initialize(spriteCommon, 10);
	stickLSprite->Initialize(spriteCommon, 11);
	buttonRTSprite->Initialize(spriteCommon, 12);

	//真ん中
	const XMFLOAT2 center = { 0.5f,0.5f };
	//画面遷移用スプライト
	transitionWhiteSprite->SetAnchorPoint(center);
	transitionWhiteSprite->SetSize(XMFLOAT2(WinApp::window_width, WinApp::window_height));
	transitionWhiteSprite->SetPosition(XMFLOAT2(-WinApp::window_width,- WinApp::window_height));
	transitionWhiteSprite->Update();

	//クリア用スプライト
	clearWhiteSprite->SetAnchorPoint(center);
	clearWhiteSprite->SetPosition(XMFLOAT2(WinApp::window_width / 2, WinApp::window_height / 2));
	clearWhiteSprite->Update();

	//パーティクル
	particles->Initialize("line.png");
	//クリア演出
	isClearBack = true;
}

void ClearScene::Finalize()
{
}

void ClearScene::Update()
{
	
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
	

	particles->Update();
	//戻り演出
	ClearBackTransition();

	
	//遷移演出用
	if (isTransition || isBackTransition) {
		Transition(nextScene);
	}

	//クリアスプライト
	clearSprite->Update();
	//演出白背景
	transitionWhiteSprite->Update();
}

void ClearScene::Draw()
{

	//スプライト描画
	spriteCommon->PreDraw();

	//演出白背景
	clearWhiteSprite->Draw();

	//クリア
	clearSprite->Draw();
	
	//遷移演出
	transitionWhiteSprite->Draw();

	spriteCommon->PostDraw();

	particles->Draw();
}

//クリアバック
void ClearScene::ClearBackTransition()
{
	float sizeX = 0.0f;
	float sizeY = 0.0f;
	//戻り
	if (isClearBack) {
		if (clearBackTime <= clearMaxTime) {
			//イージング
			sizeX = (float)easeOutQuad(clearMaxTime, WinApp::window_width, sizeX - WinApp::window_width, clearBackTime);
			sizeY = (float)easeOutQuad(clearMaxTime, WinApp::window_height, sizeY - WinApp::window_height, clearBackTime);
			//座標代入
			clearWhiteSprite->SetSize(XMFLOAT2(sizeX, sizeY));
			//タイム
			clearBackTime++;
		}
		else {
			isClearBack = false;
			clearBackTime = 0;
		}
	}
	clearWhiteSprite->Update();
}
