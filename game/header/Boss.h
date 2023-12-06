//ボスのクラス

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

enum MOVE{
	x,			//横
	y,			//縦
	xy,			//斜め移動
	move_end	//動きの列挙型のサイズ
};//動き

enum TARGETSHOT {
	playerTarget,			//自機狙い弾のみ
	playerTargetHeight,		//自機狙い弾+縦方向
	playerTargetWidth,		//自機狙い弾+横方向
	targetShot_end		//自機狙い弾の列挙型のサイズ
};//自機狙い弾

enum FIXEDSHOT {
	//0,1,2が縦
	leftHeight,			//左の縦
	centerHeight,		//真ん中の縦
	rightHeight,		//右の縦
	//3,4,5が横
	upWidth,			//上の横
	centerWidth,		//真ん中の横
	downWidth,			//下の横
	fixedShot_end		//固定撃ちの列挙型のサイズ
};//固定撃ち

class Boss
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
	//ライン描画
	void LineDraw(ID3D12GraphicsCommandList* cmdList);
	//コールバック関数
	void OnCollision();
	//リセット
	void Reset();
	//動き
	void Move();

public://弾の挙動関係
	//自機狙い弾関連
	void TargetShot();
	//縦の固定撃ち関連
	void FixedShotHeight();
	//横の固定撃ち関連
	void FixedShotWidth();

public://セッター
	void SetPosition(const XMFLOAT3 position_) { this->position = position_; }
	void SetPositionZ(float positionz_) { this->position.z = positionz_; }
	void SetScale(const XMFLOAT3 scale_) { this->scale = scale_; }
	void SetRotation(const XMFLOAT3 rotation_) { this->rotation = rotation_; }
	void SetType(int type_) { this->type = type_; }
	static void SetDxCommon(DirectXCommon* dxcommon_) { Boss::dxcommon = dxcommon_; }
	static void SetPlayer(Player* player_) { Boss::player = player_; }

public://ゲッター
	CubeObject3D* GetCubeObject() { return collisionBox; }
	const std::list<std::unique_ptr<EnemyBullet>>& GetBullet() { return bullets; }
	bool GetisDead() { return isdead; }
	XMFLOAT3 GetPos() { return position; }


private://静的メンバ変数
	static DirectXCommon* dxcommon;
	static Player* player;

private://メンバ変数
	//位置、大きさ、回転
	XMFLOAT3 position = { 0,0,0 };
	XMFLOAT3 scale = { (float)0.1,(float)0.1,(float)0.1 };
	XMFLOAT3 rotation = { 0,0,0 };


	//FBXモデル
	FbxModel* bossFbxModel = nullptr;
	//FBXオブジェクト
	FbxObject3D* bossFbxObj = nullptr;

	//3Dモデル
	Model* bossModel = nullptr;
	//3Dオブジェクト
	WireObject* bossObj = nullptr;

	//判定用
	CubeModel* cubeModel = nullptr;
	CubeObject3D* collisionBox = nullptr;

	//スピード
	float speedY = 0.05f;
	float speedX = 0.05f;

	int time = 0;
	const int MaxTime = 60;

	//弾
	std::list<std::unique_ptr<EnemyBullet>>bullets;
	ResourceManager* resource = nullptr;

	//死亡判定
	bool isdead = false;

	//動き
	int type = 0;
	bool plusX = false;
	bool plusY = false;
	const float MoveX = 1.5f;
	const float MoveY = 1.5f;

	//自機を狙う弾用
	Vector3 velocityVec = {};
	XMFLOAT3 velocity = {};


	//hp
	int life = 10;

	//動き
	MOVE move = x;
	const int moveMaxTime = 60;
	int moveTime = 0;
	//自機狙い弾
	TARGETSHOT targetShot = playerTarget;
	const int targetShotMaxTime = 40;
	int targetShotTime = 0;
	const float space = 0.3f;
	//固定撃ち
	FIXEDSHOT fixedShot = leftHeight;
	const int fixedShotMaxTime = 120;
	int fixedShotTime = 0;
	const float spaceX = 0.5f;
	const float spaceY = 0.5f;

	//敵の位置を横の壁に表示のライン
	LineModel* posLineModel = nullptr;
	LineObject* posLineObject[2] = {};
	const float widthSpace = 4.0f;
};

