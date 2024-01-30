//雑魚敵のクラス

#pragma once
#include"FbxObject3D.h"
#include"FbxModel.h"
#include"Object3d.h"
#include"Model.h"
#include"CubeModel.h"
#include"CubeObject3D.h"
#include"DirectXCommon.h"
#include"EnemyBullet.h"
#include"ResourceManager.h"
#include"Player.h"

//敵の動き
enum EnemyMove {
	Move_z,		//動かない
	Move_xz,	//横のみ
	Move_yz,	//縦のみ
	Move_xyz	//横と縦
};
//敵の弾の撃ち方
enum Shot {
	Target,		//ターゲット
	Straight,	//まっすぐ
	Random		//ランダム
};

class Enemy
{
public://メンバ関数
	//初期化
	void Initialize();
	//更新処理
	void Update();
	//描画処理
	void WireDraw();
	//描画
	void Draw();
	//当たり判定
	void DebugDraw(ID3D12GraphicsCommandList* cmdList);
	//動き
	void Move();
	//発射
	void Fire();
	//コールバック関数
	void OnCollision();
	//イージング
	double easeOutQuad(double time_, double start_, double difference, double totaltime_);

	//セッター
	void SetPosition(const XMFLOAT3 position_) { this->position = position_; }
	void SetScale(const XMFLOAT3 scale_) { this->scale = scale_; }
	void SetRotation(const XMFLOAT3 rotation_) { this->rotation = rotation_; }
	void SetType(int type_) { this->type = type_; }
	void SetShotType(int shotType_) { shotType = static_cast<Shot>(shotType_); }
	static void SetDxCommon(DirectXCommon* dxcommon_) { Enemy::dxcommon = dxcommon_; }
	static void SetPlayer(Player* player_) { Enemy::player = player_; }

	//ゲッター
	CubeObject3D* GetCubeObject() { return collisionBox; }
	const std::list<std::unique_ptr<EnemyBullet>>& GetBullet() { return bullets; }
	bool GetisDead() { return isdead; }
	XMFLOAT3 GetPos() { return position; }
	XMFLOAT3 GetRotation() { return rotation; }
	XMFLOAT3 GetScale() { return scale; }


private://静的メンバ変数
	static DirectXCommon* dxcommon;
	static Player* player;

private://メンバ変数
	//位置、大きさ、回転
	XMFLOAT3 position = { 0,0,0 };
	XMFLOAT3 scale = { (float)0.2,(float)0.2,(float)0.2 };
	XMFLOAT3 rotation = { 0,0,0 };


	//FBXモデル
	FbxModel* enemyFbxModel = nullptr;
	//FBXオブジェクト
	FbxObject3D* enemyFbxObj = nullptr;

	//3Dモデル
	Model* enemyModel = nullptr;
	//3Dオブジェクト
	WireObject* enemyObj = nullptr;

	//判定用
	CubeModel* cubeModel = nullptr;
	CubeObject3D* collisionBox = nullptr;

	//スピード
	float speedZ = 0.2f;
	float speedY = 0.02f;
	float speedX = 0.02f;

	//弾を撃つ間隔
	int time = 0;
	int MaxTime = 0;
	//それぞれの種類ごとに間隔を変更
	int targetShot = 30;
	int straightShot = 25;
	int ramdomShot = 15;



	//弾
	std::list<std::unique_ptr<EnemyBullet>>bullets;
	ResourceManager* resource = nullptr;

	//死亡判定
	bool isdead = false;

	//動き
	int type=0;
	bool plusX = false;
	bool plusY = false;
	const float MoveX = 1.5f;
	const float MoveY = 1.5f;

	//自機を狙う弾用
	Vector3 velocityVec = {};
	XMFLOAT3 velocity = {};

	//登場演出
	//登場演出中かの判定
	bool isAppearanceDirection = false;
	//登場演出に一度入ったかの判定
	bool isAppearance = false;
	const int directionMaxTime = 20;
	float startScale = 0.0f;
	float endScale = 0.2f;
	float directionScale = 0.0f;
	int directionTime = 0;

	//敵の位置を横の壁に表示のライン
	LineModel* posLineModel = nullptr;
	LineObject* posLineObject[2] = {};
	const float widthSpace = 4.0f;
	XMFLOAT3 lineScale = { 0.0f,0.0f,0.0f };
	XMFLOAT3 startLineScale = { 0.0f,0.0f,0.0f };
	XMFLOAT3 endLineScale= { 0.01f,1.0f,1.0f };
	float linePosY = 0;
	float startLinePosY = 0.0f;
	float endLinePosY = 0.2f;

	//ショット
	Shot shotType = Shot::Target;

};

