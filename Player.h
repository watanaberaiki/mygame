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

class Player
{
public://メンバ関数
	//初期化
	void Initialize();
	//更新処理
	void Update();
	//描画処理
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//動き
	void Move();
	//発射
	void Fire();
	//セッター
	void SetPosition(XMFLOAT3 position) { this->position = position; }
	void SetPositionZ(float positionz) { this->position.z = positionz; }
	void SetScale(XMFLOAT3 scale) { this->position = scale; }
	void SetRotation(XMFLOAT3 rotation) { this->position = rotation; }
	static void SetInput(Input* input) { Player::input = input; }
	static void SetDxCommon(DirectXCommon* dxcommon) { Player::dxcommon = dxcommon; }
	//ゲッター
	CubeObject3D* GetCubeObject() { return collisionBox; }
	XMFLOAT3 GetPosition() { return position; }
private://静的メンバ変数
	//キーボード
	static Input* input;
	static DirectXCommon* dxcommon;
private://メンバ変数
	//位置、大きさ、回転
	XMFLOAT3 position = { 0,0,0 };
	XMFLOAT3 scale = { (float)0.1,(float)0.1,(float)0.1 };
	XMFLOAT3 rotation = { 0,0,0 };


	//FBXモデル
	FbxModel* playerfbxmodel = nullptr;
	//FBXオブジェクト
	FbxObject3D* playerfbxobj = nullptr;

	//3Dモデル
	Model* playermodel = nullptr;
	//3Dオブジェクト
	Object3d* playerobj = nullptr;

	//判定用
	CubeModel* cubeModel = nullptr;
	CubeObject3D* collisionBox = nullptr;

	//スピード
	float speed = 0.1f;

	//弾
	std::list<std::unique_ptr<PlayerBullet>>bullets;
};

