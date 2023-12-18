//ライン描画のためのオブジェクトクラス

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
#include"LineModel.h"
class LineObject
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
			XMMATRIX world2;
			XMFLOAT3 cameraPos;
			XMFLOAT2 scale;
		};
	public:	//静的メンバ関数
		//セッター
		static void SetDevice(ID3D12Device* device_) { LineObject::device = device_; }
		static void SetCamera(Camera* camera_) { LineObject::camera = camera_; }

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
		void SetModel(LineModel* model_) { this->model = model_; }
		//グラフィックスパイプラインの生成
		static void CreateGraphicsPipeline();
		//セッター
		void SetStartPosition(XMFLOAT3 pos) { startPosition = pos; }
		void SetEndPosition(XMFLOAT3 pos) { endPosition = pos; }
		void SetStartPositionX(float posX) { startPosition.x = posX; }
		void SetEndPositionX(float posX) { endPosition.x = posX; }
		void SetStartPositionY(float posY) { startPosition.y = posY; }
		void SetEndPositionY(float posY) { endPosition.y = posY; }
		void SetScale(XMFLOAT3 sca) { scale = sca; }
		void SetRotation(XMFLOAT3 rot) { rotation = rot; }
		//ゲッター
		XMFLOAT3 GetStartPosition() { return startPosition; }
		XMFLOAT3 GetEndPosition() { return endPosition; }
		XMFLOAT3 GetRotation() { return rotation; }
		XMFLOAT3 GetScale() { return scale; }
		LineModel* GetModel() { return model; }

	private://メンバ変数
		//定数バッファ
		ComPtr<ID3D12Resource>constBuffTransform;
		//ルートシグネチャ
		static ComPtr<ID3D12RootSignature>rootsignature;
		//パイプラインステートオブジェクト
		static ComPtr<ID3D12PipelineState>pipelinestate;

	private:
		//ローカルスケール
		XMFLOAT3 scale = { 0.01f,1.0f,1.0f };
		//X,Y,Z軸回りのローカル行列
		XMFLOAT3 rotation = { 0,0,0 };
		//ローカル座標
		XMFLOAT3 startPosition = { 0,0,0 };
		//ローカル座標
		XMFLOAT3 endPosition = { 0,0,0 };
		//ローカルワールド変換行列
		XMMATRIX matWorld = {};
		XMMATRIX matWorld2 = {};
		//モデル
		LineModel* model = nullptr;
		//判定用
		XMFLOAT3 playerPos = { 0,0,0 };
		//定数バッファ用
		XMFLOAT2 scale2 = { 1.0,1.0 };

};

