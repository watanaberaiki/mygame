//プレイヤーのクラス

#pragma once
#include"FbxObject3D.h"
#include"FbxModel.h"
#include"PlayerBullet.h"
#include"Object3d.h"
#include"Model.h"
#include"CubeModel.h"
#include"CubeObject3D.h"
#include"DirectXCommon.h"
#include <memory>
#include"ResourceManager.h"
#include"LineModel.h"
#include"LineObject.h"
#include<DirectXMath.h>
#include"Vector3.h"
#include"WireObject.h"
class Player
{
public://メンバ関数
	//初期化
	void Initialize();
	//更新処理
	void Update();
	//描画処理
	void Draw();
	//ワイヤーフレーム
	void WireDraw();
	//当たり判定
	void DebugDraw(ID3D12GraphicsCommandList* cmdList);
	//動き
	void Move();
	//レティクルを動かす
	void MoveReticle();
	//発射
	void Fire();
	//コールバック関数
	void OnCollision();
	//リセット
	void Reset();
	//セッター
	void SetPosition(XMFLOAT3 position_) { this->position = position_; }
	void SetPositionZ(float positionz_) { this->position.z = positionz_; }
	void SetScale(XMFLOAT3 scale_) { this->scale = scale_; }
	void SetRotation(XMFLOAT3 rotation_) { this->rotation = rotation_; }
	void SetIsTitle(bool isTitle_) { this->isTitle = isTitle_; }
	static void SetDxCommon(DirectXCommon* dxcommon_) { Player::dxcommon = dxcommon_; }
	//ゲッター
	CubeObject3D* GetCubeObject() { return collisionBox; }
	XMFLOAT3 GetPosition() { return position; }
	const std::list<std::unique_ptr<PlayerBullet>>& GetBullet() { return bullets; }
	bool GetIsDead() { return isDead; }
private://静的メンバ変数
	//キーボード
	static Input* input;
	static DirectXCommon* dxcommon;
private://メンバ変数
	//位置、大きさ、回転
	XMFLOAT3 position = { 0,0,0 };
	XMFLOAT3 scale = { (float)0.1,(float)0.1,(float)0.1 };
	XMFLOAT3 rotation = { 0,0,0 };

	//レティクルの位置
	XMFLOAT3 frontReticlepos = { 0,0,0 };
	XMFLOAT3 backReticlepos = { 0,0,0 };
	float frontdepth = 0.0f;
	float backdepth = 0.0f;


	//FBXモデル
	FbxModel* playerFbxModel = nullptr;
	//FBXオブジェクト
	FbxObject3D* playerFbxObj = nullptr;

	//3Dモデル
	Model* playerModel = nullptr;
	//3Dオブジェクト
	WireObject* playerObj = nullptr;

	//判定用
	CubeModel* cubeModel = nullptr;
	CubeObject3D* collisionBox = nullptr;

	//スピード
	float speed = 0.05f;

	//弾
	std::list<std::unique_ptr<PlayerBullet>>bullets;

	//リソース
	ResourceManager* resource = nullptr;

	//ライン
	LineModel* lineModel = nullptr;
	LineObject* lineObject[4] = {};

	//レティクル用ベクトル
	Vector3 reticleVec = {};
	Vector3 frontVec = {};
	Vector3 backVec = {};
	XMFLOAT3 velocity = {};

	//タイトル画面に動かせないようにする
	bool isTitle = false;

	//hp
	int life = 5;

	//死んでる判定
	bool isDead = false;
};

