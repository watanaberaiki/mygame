//敵の弾のクラス


#pragma once
#include"FbxObject3D.h"
#include"FbxModel.h"
#include"Object3d.h"
#include"Model.h"
#include"CubeModel.h"
#include"CubeObject3D.h"
#include"ResourceManager.h"
#include"WireObject.h"
class EnemyBullet
{
public://メンバ関数
	//初期化
	void Initialize(DirectXCommon* dxcommon, ResourceManager*resource,XMFLOAT3 velocity);
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
	void SetIsFire(bool isfire_) { this->isFire = isfire_; }

	//ゲッター
	bool GetIsFIre() { return isFire; }
	bool GetIsDeath()const { return isDeath; }
	CubeObject3D* GetCubeObject() { return collisionBox; }
private://静的メンバ変数

private://メンバ変数
	//位置、大きさ、回転
	XMFLOAT3 position = { 0,0,0 };
	XMFLOAT3 scale = { (float)0.1,(float)0.1,(float)0.1 };
	XMFLOAT3 rotation = { 0,0,0 };


	//FBXモデル
	FbxModel* bulletFbxModel = nullptr;
	//FBXオブジェクト
	FbxObject3D* bulletFbxObj = nullptr;
	//3Dモデル
	Model* bulletModel = nullptr;
	//3Dオブジェクト
	WireObject* bulletObj = nullptr;


	//スピード
	float speed = 0.5f;
	//フラグ
	bool isFire = false;
	bool isDeath = false;

	//寿命
	static const int32_t kLifeTime = 60 * 5;
	//デスタイマー
	int32_t deathTimer_ = kLifeTime;

	//判定用
	CubeModel* cubeModel = nullptr;
	CubeObject3D* collisionBox = nullptr;

	//自機に向けてのロックオン用
	XMFLOAT3 velocity = {};

};

