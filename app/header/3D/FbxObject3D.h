#pragma once

#include"FbxModel.h"
#include"Camera.h"

#include<Windows.h>
#include<wrl.h>
#include<d3d12.h>
#include<d3dx12.h>
#include<DirectXMath.h>
#include<string>
#include"FbxLoader.h"

class FbxObject3D
{

protected:	//エイリアス
	//Microsoft::WRL::を省略
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	//定数
	//ボーンの最大数
	static const int MAX_BONES = 32;

public:	//サブクラス
	//定数バッファ用データ構造体
	struct ConstBufferDataTransform
	{
		XMMATRIX viewproj;		//ビュープロジェクション行列
		XMMATRIX world;			//ワールド行列
		XMFLOAT3 cameraPos;		//カメラ座標(ワールド座標)
	};
	//定数バッファ用構造体(スキニング)
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[MAX_BONES];
	};

public:	//静的メンバ関数
	//setter
	static void SetDevice(ID3D12Device* device_) { FbxObject3D::device = device_; }
	static void SetCamera(Camera* camera_) { FbxObject3D::camera = camera_; }
	//グラフィックスパイプラインの生成
	static void CreateGraphicsPipeline();
private://静的メンバ変数
	//デバイス
	static ID3D12Device* device;
	//カメラ
	static Camera* camera;
	//ルートシグネチャ
	static ComPtr<ID3D12RootSignature>rootsignature;
	//パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState>pipelinestate;
public://メンバ関数
	//初期化
	void Initialize();
	//毎フレーム処理
	void Update();
	//描画
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//アニメーション開始
	void PlayAnimation();

	///セッター
	//モデルのセット
	void SetModel(FbxModel* fbxModel_) { this->fbxModel = fbxModel_; }
	void SetPosition(XMFLOAT3 position_) { this->position = position_; }
	void SetScale(XMFLOAT3 scale_) { this->scale = scale_; }
	void SetRotation(XMFLOAT3 rotation_) { this->rotation = rotation_; }


	//ゲッター

protected:	//メンバ変数
	//定数バッファ
	ComPtr<ID3D12Resource>constBuffTransform;
	//定数バッファ(スキン)
	ComPtr<ID3D12Resource>constBuffSkin;
	//ローカルスケール
	XMFLOAT3 scale = { (float)1,(float)1,(float)1 };
	//X,Y,Z軸周りのローカル回転角
	XMFLOAT3 rotation = { 0,45,0 };
	//ローカル座標
	XMFLOAT3 position = { 0,0,0 };
	//ローカルワールド変換行列
	XMMATRIX matWorld = {};
	//モデル
	FbxModel* fbxModel = nullptr;
	//1フレームの時間
	FbxTime frameTime;
	//アニメーション開始時間
	FbxTime startTime;
	//アニメーション終了時間
	FbxTime endTime;
	//現在時間
	FbxTime currentTime;
	//アニメーション再生中
	bool isPlay = false;
};
