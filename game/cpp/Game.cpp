#include "Game.h"
Game::Game()
{
}

Game::~Game()
{
	
}

void Game::Initialize(DirectXCommon* dxCommon, ImguiManager* imgui)
{
	sceneManager_ = new SceneManager();
	BaseScene* scene = new TitleScene();
	sceneManager_->Initialize(dxCommon, imgui);
	sceneManager_->SetNextScene(scene);
}

void Game::Update()
{
	sceneManager_->Update();

	//imgui更新
	imgui_->Begin();
	//表示項目
	//ボタンを押したら
	if (ImGui::Button("Save")) {

	}
	//デモウィンドウ
	ImGui::ShowDemoWindow();

	imgui_->End();
}

void Game::Draw()
{
	sceneManager_->Draw();

}