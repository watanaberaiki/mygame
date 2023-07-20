#pragma once
#include"FbxObject3D.h"
#include"FbxModel.h"
#include"Object3d.h"
#include"Model.h"
#include"CubeModel.h"
#include"CubeObject3D.h"

class PlayerBullet
{
public://メンバ関数
	//初期化
	void Initialize(DirectXCommon* dxcommon);
	//更新処理
	void Update();
	//描画処理
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//動き
	void Move();
	//判定
	void OnCollision();
	//セッター
	void SetPosition(XMFLOAT3 position) { this->position = position; }
	void SetScale(XMFLOAT3 scale) { this->position = scale; }
	void SetRotation(XMFLOAT3 rotation) { this->position = rotation; }
	void SetIsFire(bool isfire) { this->isfire = isfire; }

	//ゲッター
	bool GetIsFIre() { return isfire; }
	bool GetIsDeath()const { return isdeath; }
private://静的メンバ変数
	//キーボード
	static Input* input;

private://メンバ変数
	//位置、大きさ、回転
	XMFLOAT3 position = { 0,0,0 };
	XMFLOAT3 scale = { (float)0.1,(float)0.1,(float)0.1 };
	XMFLOAT3 rotation = { 0,0,0 };


	//FBXモデル
	FbxModel* bulletfbxmodel = nullptr;
	//FBXオブジェクト
	FbxObject3D* bulletfbxobj = nullptr;
	//3Dモデル
	Model* bulletmodel = nullptr;
	//3Dオブジェクト
	Object3d* bulletobj = nullptr;


	//スピード
	float speed = 0.2f;
	//フラグ
	bool isfire = false;
	bool isdeath = false;

	//判定用
	CubeModel* cubeModel = nullptr;
	CubeObject3D* collisionBox = nullptr;
	
};

