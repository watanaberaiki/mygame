#pragma once
#include "Camera.h"
#include"Input.h"
class Railcamera
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
public://メンバ関数
	//初期化
	void Initialize(Camera*camera);
	//更新
	void Update();

	//ゲッター
	const XMMATRIX& GetMatWorld()const { return matWorld; }
private://メンバ変数
	// ローカルスケール
	XMFLOAT3 scale = XMFLOAT3(1,1,1);
	// X,Y,Z軸回りのローカル回転角
	XMFLOAT3 rotation = XMFLOAT3( 0,0,0 );
	// ローカル座標
	XMFLOAT3 position = XMFLOAT3(0,0,0 );
	// ローカルワールド変換行列
	XMMATRIX matWorld;

private://静的メンバ変数
	//カメラ
	static Camera* camera;
};

