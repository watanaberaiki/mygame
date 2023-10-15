//プレイヤーの弾のクラス

#pragma once
#include"FbxObject3D.h"
#include"FbxModel.h"
#include"Object3d.h"
#include"Model.h"
#include"CubeModel.h"
#include"CubeObject3D.h"
#include"ResourceManager.h"
class PlayerBullet
{
public://メンバ関数
	//初期化
	void Initialize(DirectXCommon* dxcommon,ResourceManager* resource,XMFLOAT3 velocity);
	//更新処理
	void Update();
	//描画処理
	void Draw();
	//当たり判定
	void DebugDraw(ID3D12GraphicsCommandList* cmdList);
	//動き
	void Move();
	//判定
	void OnCollision();
	//セッター
	void SetPosition(XMFLOAT3 position_) { this->position = position_; }
	void SetScale(XMFLOAT3 scale_) { this->position = scale_; }
	void SetRotation(XMFLOAT3 rotation_) { this->position = rotation_; }
	void SetIsFire(bool isfire_) { this->isfire = isfire_; }

	//ゲッター
	bool GetIsFIre() { return isfire; }
	bool GetIsDeath()const { return isdeath; }
	CubeObject3D* GetCubeObject() { return collisionBox; }
private://静的メンバ変数
	//キーボード
	static Input* input;

private://メンバ変数
	//位置、大きさ、回転
	XMFLOAT3 position = { 0,0,0 };
	XMFLOAT3 scale = { (float)0.1,(float)0.1,(float)0.1 };
	XMFLOAT3 rotation = { 0,0,0 };


	//FBXモデル
	FbxModel* bulletfbxmodel = nullptr;
	//FBXオブジェクト
	FbxObject3D* bulletfbxobj = nullptr;
	//3Dモデル
	Model* bulletmodel = nullptr;
	//3Dオブジェクト
	Object3d* bulletobj = nullptr;


	//スピード
	float speed = 0.4f;
	XMFLOAT3 velocity = {};
	//フラグ
	bool isfire = false;
	bool isdeath = false;

	//寿命
	static const int32_t kLifeTime = 60 * 5;
	//デスタイマー
	int32_t deathTimer_ = kLifeTime;

	//判定用
	CubeModel* cubeModel = nullptr;
	CubeObject3D* collisionBox = nullptr;
};

