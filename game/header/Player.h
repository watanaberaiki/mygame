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
	enum Reticle {
		front,		//一番前
		frontBack,	//二番目
		backFront,	//三番目
		back,		//後ろ
	};
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
	//レティクルチェンジ
	void ReticleChange();
	//セッター
	void SetPosition(const XMFLOAT3 position_) { this->position = position_; }
	void SetPositionZ(const float positionz_) { this->position.z = positionz_; }
	void SetScale(const XMFLOAT3 scale_) { this->scale = scale_; }
	void SetRotation(const XMFLOAT3 rotation_) { this->rotation = rotation_; }
	void SetIsTitle(bool isTitle_) { this->isTitle = isTitle_; }
	static void SetDxCommon(DirectXCommon* dxcommon_) { Player::dxcommon = dxcommon_; }
	void SetIsEnemyReticleCol(bool isEnemyReticleCol_) { isEnemyReticleCol = isEnemyReticleCol_; }
	//ゲッター
	CubeObject3D* GetCubeObject() { return collisionBox; }
	XMFLOAT3 GetPosition() { return position; }
	XMFLOAT3 GetEndPosition() { return backReticlepos; }
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
	XMFLOAT3 frontBackReticlepos = { 0,0,0 };
	XMFLOAT3 backFrontReticlepos = { 0,0,0 };
	XMFLOAT3 backReticlepos = { 0,0,0 };

	//レティクルチェンジ
	int reticle = front;

	float frontdepth = 0.0f;
	float backdepth = 0.0f;
	//レティクル制限
	//前
	const float frontReticleUpLimit = 2.0f;
	const float frontReticleDownLimit = 1.0f;
	const float frontReticleLeftLimit = 2.0f;
	const float frontReticleRightLimit = 2.0f;
	//二番目
	const float frontBackReticleUpLimit = 3.0f;
	const float frontBackReticleDownLimit = 1.5f;
	const float frontBackReticleLeftLimit = 3.0f;
	const float frontBackReticleRightLimit = 3.0f;
	//三番目
	const float backFrontReticleUpLimit = 4.0f;
	const float backFrontReticleDownLimit = 2.0f;
	const float backFrontReticleLeftLimit = 3.5f;
	const float backFrontReticleRightLimit = 3.5f;
	//後ろ
	const float backReticleUpLimit = 5.0f;
	const float backReticleDownLimit = 2.0f;
	const float backReticleLeftLimit = 4.0f;
	const float backReticleRightLimit = 4.0f;



	//FBXモデル
	FbxModel* playerFbxModel = nullptr;
	//FBXオブジェクト
	FbxObject3D* playerFbxObj = nullptr;

	//3Dモデル
	Model* playerModel_hp[4] = {};
	//3Dオブジェクト
	WireObject* playerObj = nullptr;

	//判定用
	CubeModel* cubeModel = nullptr;
	CubeObject3D* collisionBox = nullptr;

	//スピード
	float speed = 0.05f;

	//弾
	std::list<std::unique_ptr<PlayerBullet>>bullets;
	const int bulletDireyMaxTime = 10;
	int bulletDireyTime = 0;
	//弾の回転用
	Vector3 bulletFrontVec = { 0,0,0 };
	float lengthFrontVec = 0.0f;
	float lengthreticleVec = 0.0f;
	float bulletRotation = 0.0f;


	//リソース
	ResourceManager* resource = nullptr;

	//レティクル
	LineModel* reticleModel = nullptr;
	LineModel* reticleSelectModel = nullptr;

	static const int maxLine = 8;
	const float linePos = 0.2f;
	LineObject* reticleObject[maxLine] = {};
	bool isEnemyReticleCol = false;

	//レティクルの位置を横の壁に表示のライン
	LineModel* reticleLineModel = nullptr;
	static const int maxWidthLine = 8;
	LineObject* reticleLineObject[maxWidthLine] = {};
	const float widthSpace = 4.0f;
	LineModel* reticleLineModelStraight = nullptr;
	LineObject* reticleLineObjectStraight;

	//レティクル影
	static const int lineShadowNum = 5;
	LineObject* reticleLineShadowObject[lineShadowNum] = {};
	float downLinePosY = 1.7f;
	LineModel* reticleShadowModel = nullptr;
	LineModel* reticleShadowSelectModel = nullptr;
	//レティクル用ベクトル
	Vector3 reticleVec = {};
	Vector3 frontVec = {};
	Vector3 frontBackVec = {};
	Vector3 backFrontVec = {};
	Vector3 backVec = {};
	XMFLOAT3 velocity = {};

	//タイトル画面に動かせないようにする
	bool isTitle = false;

	//hp
	const int maxLife = 4;
	int life = 4;

	//死んでる判定
	bool isDead = false;

	//当たった後の無敵時間
	bool isInvincible = false;
	int maxInvincibleTime = 20;
	int invincibleTime = 0;
};

