#include "ClearScene.h"

void ClearScene::Initialize(DirectXCommon* dxCommon, ImguiManager* imgui)
{
	dxCommon_ = dxCommon;
	imgui_ = imgui;
	input_ = Input::GetInstance();
}

void ClearScene::Finalize()
{
}

void ClearScene::Update()
{
	//遷移演出用
	if (isTransition || isBackTransition) {
		Transition(nextScene);
	}

	particles->Update();
	//戻っている最中は押しても反応しない
	if (isBackTransition) {

	}
	else
	{
		//タイトルシーンチェンジ
		if (input_->TriggerKey(DIK_SPACE) || input_->TriggerPadButton(XINPUT_GAMEPAD_A)) {
			isTransition = true;
			nextScene = Title;
			player->Reset();
		}
	}
	//戻り演出
	ClearBackTransition();

	clearSprite->Update();
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
