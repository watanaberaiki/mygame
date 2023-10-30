//メインカメラクラス

#pragma once
#include <DirectXTex.h>
#include"Input.h"

using namespace DirectX;

class Camera
{

public:
	//初期化処理
	void Initialize();
	//更新処理
	void Update();

	/*static void SetInput(Input* input) { Camera::input = input; }*/

public:


	//セッター
	void SetEye(XMFLOAT3 eye_) { this->eye = eye_; };
	void SetTarget(XMFLOAT3 target_) { this->target = target_; };
	void SetUp(XMFLOAT3 up_) { this->up = up_; };

	//ゲッター
	const XMFLOAT3& GetTarget() { return target; }
	const XMFLOAT3& GetUp() { return up; }
	const XMFLOAT3& GetEye() { return eye; }
	const XMMATRIX& GetmatView() { return matView; }
	XMMATRIX GetmatViewProjection() { return (matView * matProjection); }

private:
	XMMATRIX matView = {};
	XMFLOAT3 eye = {};		//視点座標
	XMFLOAT3 target = {};	//注視点座標
	XMFLOAT3 up = {};		//上方向ベクトル
	XMMATRIX matProjection = {};

	//キーボード
	Input* input = nullptr;
};

