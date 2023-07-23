#pragma once
#include"Vector3.h"
#include "Input.h"
#include"WinApp.h"
#include"DirectXCommon.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include"Object3d.h"
#include <DirectXMath.h>
#include<math.h>
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include "CollisionPrimitive.h"
#include "Collision.h"
#include"Camera.h"
#include <math.h>
#include"ParticleManager.h"
#include"FbxLoader.h"
#include"FbxObject3D.h"
#include"Railcamera.h"

#include"LineModel.h"
#include"LineObject.h"
#include"Enemy.h"
#include"Player.h"
#include"CubeModel.h"
#include"CubeObject3D.h"
#include"ResourceManager.h"
#include"EnemyBullet.h"
#pragma warning(push)
#pragma warning(disable:4267)
#include<map>
#include<utility>
#pragma warning(pop)
class GameScene
{
//メンバ関数
public:
	GameScene();
	~GameScene();
	//初期化
	void Initialize(DirectXCommon* dxCommon, Input* input);
	//更新
	void Update();
	//描画
	void Draw();
	//判定一覧
	void AllCollision();
private:
	//ポインタ
	Input* input_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;

	//パーティクル
	ParticleManager* particleManager = new ParticleManager();

	//スプライト共通部
	SpriteCommon* spriteCommon = nullptr;
	//スプライト
	Sprite* hitsprite = new Sprite();
	Sprite* mariosprite = new Sprite();
	
	/*OBJからモデルデータを読み込む*/
	//3Dモデル
	Model* spheremodel=nullptr;
	Model* blockmodel = nullptr;
	//3Dモデルの当たり判定
	XMFLOAT3 minsphereModel = {}, maxsphereModel = {};
	
	//3Dオブジェクト
	Object3d* sphereobj=nullptr;
	Object3d* blockobj = nullptr;

	//カメラ
	Camera* camera = nullptr;

	//カメラ用
	XMFLOAT3 eye = {0,0,0};		//視点座標
	XMFLOAT3 target = {0,0,0};	//注視点座標
	XMFLOAT3 up = {0,0,0};		//上方向ベクトル
	XMMATRIX matView = {};

	//FBXModel
	FbxModel* boneTestModel = nullptr;
	FbxModel* cube = nullptr;

	static const int bonetestsize = 5;
	FbxObject3D* bonetest[bonetestsize] = {};

	//レールカメラ
	Railcamera* railCamera = nullptr;

	//敵
	static const int enemysize = 5;
	std::list<std::unique_ptr<Enemy>>enemys;
	/*const std::list<std::unique_ptr<EnemyBullet>>& enemyBullets;*/
	//プレイヤー
	Player* player = nullptr;
	////マップ用
	int enemycount = 0;

	//ヒット確認
	bool isHit = false;

	//ライン
	LineModel* linemodel = nullptr;
	LineObject* lineobject = nullptr;

	ResourceManager* resorcemanager = nullptr;

	//判定をまとめて管理
	std::list<std::unique_ptr<CubeObject3D>>collisionBoxs;
};

