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
	//3Dモデルの当たり判定
	XMFLOAT3 minsphereModel = {}, maxsphereModel = {};
	
	//3Dオブジェクト
	Object3d* sphereobj=nullptr;
	Object3d* blockobj = nullptr;

	//カメラ
	Camera* camera = nullptr;

	//カメラ用
	XMFLOAT3 eye;		//視点座標
	XMFLOAT3 target;	//注視点座標
	XMFLOAT3 up;		//上方向ベクトル
	XMMATRIX matView;

	//FBX
	FbxModel* model1 = nullptr;
	FbxObject3D* object1 = nullptr;
};

