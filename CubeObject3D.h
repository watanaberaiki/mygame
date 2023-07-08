#pragma once

#include "CubeModel.h"
#include "Camera.h"

#include "windows.h"
#include "wrl.h"
#include "d3d12.h"
#include "d3dx12.h"
#include "DirectXMath.h"
#include "string.h"
#include "input.h"
class CubeObject3D
{
private:	//エイリアス
	//Microsoft::WRL::を省略
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
public://サブクラス
	//定数バッファ用データ構造体
	struct ConstBufferDataTransform
	{
		XMMATRIX viewproj;
		XMMATRIX world;
		XMFLOAT3 cameraPos;
	};
public:	//静的メンバ関数
	//セッター
	static void SetDevice(ID3D12Device* device) { CubeObject3D::device = device; }
	static void SetCamera(Camera* camera) { CubeObject3D::camera = camera; }

private://静的メンバ変数
	static ID3D12Device* device;
	static Camera* camera;

public://メンバ関数
	//初期化
	void Initialize();
	//更新
	void Update();
	//移動
	void Move();
	//描画
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//モデルのセット
	void SetModel(CubeModel* model) { this->model = model; }
	//グラフィックスパイプラインの生成
	static void CreateGraphicsPipeline();
	//セッター
	void SetPosition(XMFLOAT3 pos) { position = pos; }
	void SetScale(XMFLOAT3 sca) { scale = sca; }
	void SetRotation(XMFLOAT3 rot) { rotation = rot; }
	//ゲッター
	XMFLOAT3 GetPosition() { return position; }
	XMFLOAT3 GetRotation() { return rotation; }
	XMFLOAT3 GetScale() { return scale; }
	CubeModel *GetModel() { return model; }
	//判定
	bool CheakCollision(CubeObject3D* cubeObject3D);

private://メンバ変数
//定数バッファ
	ComPtr<ID3D12Resource>constBuffTransform;
	//ルートシグネチャ
	static ComPtr<ID3D12RootSignature>rootsignature;
	//パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState>pipelinestate;

private:
	//ローカルスケール
	XMFLOAT3 scale = { 1,1,1 };
	//X,Y,Z軸回りのローカル行列
	XMFLOAT3 rotation = { 0,0,0 };
	//ローカル座標
	XMFLOAT3 position = { 0,0,0 };
	//ローカルワールド変換行列
	XMMATRIX matWorld;
	//モデル
	CubeModel* model = nullptr;
	//判定用
	XMFLOAT3 playerPos = { 0,0,0 };
	
};

