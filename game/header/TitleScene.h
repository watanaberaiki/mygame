#pragma once
#include "BaseScene.h"
using namespace RaikiEngine;
class TitleScene :
    public BaseScene
{
public:
	//初期化
	void Initialize(DirectXCommon* dxCommon, ImguiManager* imgui) override;
	//終了処理
	void Finalize() override;
	//更新処理
	void Update() override;
	//描画処理
	void Draw() override;
	//タイトルシーンのパーティクル処理
	void TitleParticle(XMFLOAT3 pos);
private:
	
};

