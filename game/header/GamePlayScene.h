#pragma once
#include "BaseScene.h"

class GamePlayScene :public BaseScene
{
public://関数
	//初期化
	void Initialize(DirectXCommon* dxCommon, ImguiManager* imgui) override;
	//終了処理
	void Finalize() override;
	//更新処理
	void Update() override;
	//描画処理
	void Draw() override;
	//判定一覧
	void AllCollision();
	//敵とレティクルの当たり判定
	bool EnemyLineCollision(XMFLOAT3 lineStartPos, XMFLOAT3 lineEndPos, XMFLOAT3 enemyPos, XMFLOAT3 enemyScale);
	//パーティクル処理
	void Particle(XMFLOAT3 pos);
	//敵のパーティクル
	void EnemyParticle(XMFLOAT3 pos);
	//操作ガイド
	void OperationGuide();
	//クリア演出
	void ClearTransition();
private://変数
	//ボス戦かどうか
	bool isBoss = false;

};

