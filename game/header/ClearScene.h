#pragma once
#include "BaseScene.h"
class ClearScene :
    public BaseScene
{
	//初期化
	void Initialize(DirectXCommon* dxCommon, ImguiManager* imgui) override;
	//終了処理
	void Finalize() override;
	//更新処理
	void Update() override;
	//描画処理
	void Draw() override;
	//クリアバック
	void ClearBackTransition();
};

