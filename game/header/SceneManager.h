#pragma once
#include"BaseScene.h"
class SceneManager
{
public:
	//デストラクタ
	~SceneManager();
	//次シーン予約
	void SetNextScene(BaseScene* nextScene) { nextScene_ = nextScene; }
	//初期化
	void Initialize(DirectXCommon* dxCommon, ImguiManager* imgui);
	//更新
	void Update();
	//描画
	void Draw();
private:
	//次のシーン
	BaseScene* nextScene_ = nullptr;
	//今のシーン(実行中シーン)
	BaseScene* scene_ = nullptr;
	DirectXCommon* dxCommon_;
	ImguiManager* imgui_;
};

