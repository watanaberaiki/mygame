#pragma once
#include"FbxObject3D.h"
#include"FbxModel.h"
#include"PlayerBullet.h"

class Player
{
public://メンバ関数
	//初期化
	void Initialize();
	//更新処理
	void Update();
	//描画処理
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//動き
	void Move();
	//発射
	void Fire();
	//セッター
	void SetPosition(XMFLOAT3 position) { this->position = position; }
	void SetScale(XMFLOAT3 scale) { this->position = scale; }
	void SetRotation(XMFLOAT3 rotation) { this->position = rotation; }
	static void SetInput(Input* input) { Player::input = input; }

private://静的メンバ変数
	//キーボード
	static Input* input;

private://メンバ変数
	//位置、大きさ、回転
	XMFLOAT3 position = { 0,0,0 };
	XMFLOAT3 scale = { (float)0.2,(float)0.03,(float)0.1 };
	XMFLOAT3 rotation = { 0,0,0 };


	//FBXモデル
	FbxModel* playermodel = nullptr;
	//FBXオブジェクト
	FbxObject3D* playerobj = nullptr;

	//スピード
	float speed = 0.2f;

	//弾
	size_t bulletsize = 100;
	std::list<std::unique_ptr<PlayerBullet>>bullets;
};

