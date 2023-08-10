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

enum EnemyMove {
	Move_z,
	Move_xz,
	Move_yz,
	Move_xyz
};

class Enemy
{
public://メンバ関数
	//初期化
	void Initialize();
	//更新処理
	void Update();
	//描画処理
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//当たり判定
	void DebugDraw(ID3D12GraphicsCommandList* cmdList);
	//動き
	void Move();
	//発射
	void Fire();
	//コールバック関数
	void OnCollision();
	
	//セッター
	void SetPosition(XMFLOAT3 position) { this->position = position; }
	void SetScale(XMFLOAT3 scale) { this->scale = scale; }
	void SetRotation(XMFLOAT3 rotation) { this->rotation = rotation; }
	void SetType(int type) { this->type = type; }
	static void SetDxCommon(DirectXCommon* dxcommon) { Enemy::dxcommon = dxcommon; }
	//ゲッター
	CubeObject3D* GetCubeObject() { return collisionBox; }
	const std::list<std::unique_ptr<EnemyBullet>>& GetBullet() { return bullets; }
	bool GetisDead() { return isdead; }
	XMFLOAT3 GetPos() { return position; }


private://静的メンバ変数
	static DirectXCommon* dxcommon;

private://メンバ変数
	//位置、大きさ、回転
	XMFLOAT3 position = { 0,0,0 };
	XMFLOAT3 scale = { (float)0.1,(float)0.1,(float)0.1 };
	XMFLOAT3 rotation = { 0,0,0 };


	//FBXモデル
	FbxModel* enemyfbxmodel = nullptr;
	//FBXオブジェクト
	FbxObject3D* enemyfbxobj = nullptr;

	//3Dモデル
	Model* enemymodel = nullptr;
	//3Dオブジェクト
	WireObject* enemyobj = nullptr;

	//判定用
	CubeModel* cubeModel = nullptr;
	CubeObject3D* collisionBox = nullptr;

	//スピード
	float speedZ = 0.2f;
	float speedY = 0.1f;
	float speedX = 0.1f;

	int time = 0;
	const int MaxTime = 60;

	//弾
	std::list<std::unique_ptr<EnemyBullet>>bullets;
	ResourceManager* resource = nullptr;

	//死亡判定
	bool isdead = false;

	//動き
	int type=0;
	bool plusX = false;
	bool plusY = false;
	const float MoveX = 5.0f;
	const float MoveY = 5.0f;


};

