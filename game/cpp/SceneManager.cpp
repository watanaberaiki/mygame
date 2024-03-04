#include "SceneManager.h"

SceneManager::~SceneManager()
{
	//最後のシーンの終了と解放
	scene_->Finalize();
	delete scene_;
}

void SceneManager::Initialize(DirectXCommon* dxCommon, ImguiManager* imgui)
{
	dxCommon_ = dxCommon;
	imgui_ = imgui;
}

void SceneManager::Update()
{
	//次シーンの予約があるなら
	if (nextScene_) {
		//旧シーンの終了
		if (scene_) {
			scene_->Finalize();
			delete scene_;
		}

		//シーン切り替え
		scene_ = nextScene_;
		nextScene_ = nullptr;
		//シーンマネージャをセット
		scene_->SetSceneManager(this);
		//次シーンを初期化する
		scene_->Initialize(dxCommon_,imgui_);
	}
	
	//実行中シーンを更新する
	scene_->Update();
}

void SceneManager::Draw()
{
	scene_->Draw();
}
