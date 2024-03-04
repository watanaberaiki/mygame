#include "GameOverScene.h"

void GameOverScene::Initialize(DirectXCommon* dxCommon, ImguiManager* imgui)
{
	dxCommon_ = dxCommon;
	imgui_ = imgui;
	input_ = Input::GetInstance();
}

void GameOverScene::Finalize()
{
}

void GameOverScene::Update()
{
	//遷移演出用
	if (isTransition || isBackTransition) {
		Transition(nextScene);
	}

	//スプライト
	gameOverSprite->Update();
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
}

void GameOverScene::Draw()
{

	
	//スプライト描画
	spriteCommon->PreDraw();

	gameOverSprite->Draw();

	//遷移演出
	transitionWhiteSprite->Draw();

	spriteCommon->PostDraw();
}
