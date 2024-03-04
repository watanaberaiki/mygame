#pragma once
#include "BaseScene.h"
class BossScene :
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
	//操作ガイド
	void OperationGuide();
	//パーティクル処理
	void Particle(XMFLOAT3 pos);
	//判定一覧
	void AllCollision();
	//敵とレティクルの当たり判定
	bool EnemyLineCollision(XMFLOAT3 lineStartPos, XMFLOAT3 lineEndPos, XMFLOAT3 enemyPos, XMFLOAT3 enemyScale);
	//敵のパーティクル
	void EnemyParticle(XMFLOAT3 pos);
	//クリア演出
	void ClearTransition();
};

