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
#include"LoadFile.h"
#include<map>

struct LevelData;

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
	Model* testmodel = nullptr;
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

	//ファイル読み込み
	LevelData* leveldata = nullptr;
	//配置オブジェクト情報
	std::map<std::string, Model*> models;
	std::vector<Object3d*> objects;
};

