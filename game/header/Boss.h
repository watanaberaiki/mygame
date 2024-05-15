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
	//イージング
	double easeOutQuad(double time_, double start_, double difference, double totaltime_);

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
	XMFLOAT3 GetScale() { return scale; }


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
	const int maxLife = 30;
	int life = 30;

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
	LineObject* posLineObject[4] = {};
	const float widthSpace = 4.0f;
	const float linePosY = 0.2f;
	const float downLinePosY = 1.7f;
	float spaceDownX = 0.0f;
	float endSpaceX = 0.2f;
	float spaceDownZ = 0.0f;
	float endSpaceZ = 1.0f;

	//最初だけの演出
	bool firstDirection = false;
	//Z方向に定期的に移動
	int moveZTime = 0;
	const int moveZMaxTime = 300;
	bool isMoveZ = false;
	bool isDirectionZ = false;
	//演出
	const int directionMaxTime = 30;
	XMFLOAT3 startScale = { 0.5f, 0.6f, 0.5f };
	float endScale = 0.0f;
	XMFLOAT3 directionScale = {0.0f,0.0f,0.0f};
	int directionTime = 0;
	//演出縮小拡大
	bool isReduction = false;
	//Z軸の移動先
	float distanceZ = 7.0f;
	float front = 0.0f;		//一番前
	float frontBack=0.0f;	//二番目
	float backFront=0.0f;	//三番目
	float back=0.0f;		//後ろ
};

