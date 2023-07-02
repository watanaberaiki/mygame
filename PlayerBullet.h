#pragma once
#include"FbxObject3D.h"
#include"FbxModel.h"
class PlayerBullet
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
	//セッター
	void SetPosition(XMFLOAT3 position) { this->position = position; }
	void SetScale(XMFLOAT3 scale) { this->position = scale; }
	void SetRotation(XMFLOAT3 rotation) { this->position = rotation; }
	void SetIsFire(bool isfire) { this->isfire = isfire; }

	//ゲッター
	bool GetIsFIre() { return isfire; }
private://静的メンバ変数
	//キーボード
	static Input* input;

private://メンバ変数
	//位置、大きさ、回転
	XMFLOAT3 position = { 0,0,0 };
	XMFLOAT3 scale = { (float)0.05,(float)0.01,(float)0.05 };
	XMFLOAT3 rotation = { 0,0,0 };


	//FBXモデル
	FbxModel* bulletmodel = nullptr;
	//FBXオブジェクト
	FbxObject3D* bulletobj = nullptr;

	//スピード
	float speed = 0.2f;
	//フラグ
	bool isfire = false;
};

