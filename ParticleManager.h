#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <forward_list>
#include "DirectXCommon.h"

using namespace Microsoft::WRL;

class ParticleManager
{
private: // エイリアス
	// Microsoft::WRL::を省略
	//template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // サブクラス
	// 頂点データ構造体
	struct VertexPos
	{
		XMFLOAT3 pos; // xyz座標
		float scale;
	};

	// 定数バッファ用データ構造体
	struct ConstBufferData
	{

		XMMATRIX mat;	// ３Ｄ変換行列
		XMMATRIX matBillboard;
	};

	//パーティクル一粒
	struct Particle {
		//DirectXを省略
		using XMFLOAT3 = DirectX::XMFLOAT3;

		//座標
		XMFLOAT3 position = {};
		//速度
		XMFLOAT3 velocity = {};
		//加速度
		XMFLOAT3 accel = {};
		//現在フレーム
		int frame = 0;
		//過去フレーム
		int num_frame = 0;

		//スケール
		float scale = 1.0f;
		//初期値
		float s_scale = 1.0f;
		//最終値
		float e_scale = 0.0f;
		//色
		XMFLOAT4 color = {};
	};

private: // 定数
	const int division = 50;					// 分割数
	const float radius = 5.0f;				// 底面の半径
	const float prizmHeight = 8.0f;			// 柱の高さ
	const int planeCount = division * 2 + division * 2;		// 面の数

	static const int vertexCount = 1024;		// 頂点数

public: // 静的メンバ関数


	ParticleManager();

	~ParticleManager();

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="window_width">画面幅</param>
	/// <param name="window_height">画面高さ</param>
	static void StaticInitialize(DirectXCommon* dxcommon, int window_width, int window_height);

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// 3Dオブジェクト生成
	/// </summary>
	/// <returns></returns>
	ParticleManager* Create();

	/// <summary>
	/// 視点座標の取得
	/// </summary>
	/// <returns>座標</returns>
	const XMFLOAT3& GetEye() { return eye; }

	/// <summary>
	/// 視点座標の設定
	/// </summary>
	/// <param name="position">座標</param>
	void SetEye(XMFLOAT3 eye);

	/// <summary>
	/// 注視点座標の取得
	/// </summary>
	/// <returns>座標</returns>
	const XMFLOAT3& GetTarget() { return target; }

	/// <summary>
	/// 注視点座標の設定
	/// </summary>
	/// <param name="position">座標</param>
	void SetTarget(XMFLOAT3 target);

	/// <summary>
	/// ベクトルによる移動
	/// </summary>
	/// <param name="move">移動量</param>
	void CameraMoveVector(XMFLOAT3 move);

	void CameraMoveEyeVector(XMFLOAT3 move);

private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* device;
	// デスクリプタサイズ
	static UINT descriptorHandleIncrementSize;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestate;
	// ビュー行列
	static XMMATRIX matView;
	// 射影行列
	static XMMATRIX matProjection;
	// 視点座標
	static XMFLOAT3 eye;
	// 注視点座標
	static XMFLOAT3 target;
	// 上方向ベクトル
	static XMFLOAT3 up;	
	// 頂点データ配列
	static VertexPos vertices[vertexCount];
	//ビューボード行列
	static XMMATRIX matBillboard;
	//Y軸周りのビューボード行列
	static XMMATRIX matBillboardY;
	//パス
	static std::string kDefaultTextureDirectoryPath;

	static DirectXCommon* dxcommon;



private:// 静的メンバ関数
	/// <summary>
	/// デスクリプタヒープの初期化
	/// </summary>
	void InitializeDescriptorHeap();

	/// <summary>
	/// カメラ初期化
	/// </summary>
	/// <param name="window_width">画面横幅</param>
	/// <param name="window_height">画面縦幅</param>
	static void InitializeCamera(int window_width, int window_height);

	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	/// <returns>成否</returns>
	static void InitializeGraphicsPipeline();

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	void LoadTexture();

	void LoadTexture(const std::string& fileName);

	/// <summary>
	/// モデル作成
	/// </summary>
	void CreateModel();

	/// <summary>
	/// ビュー行列を更新
	/// </summary>
	static void UpdateViewMatrix();

public: // メンバ関数
	bool Initialize(const std::string& fileName);
	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	//パーティクルの追加
	void Add(int life, XMFLOAT3 position, XMFLOAT3 velociy, XMFLOAT3 accel, float start_scale, float end_scale, XMFLOAT4 color);

private: // メンバ変数

	ComPtr<ID3D12Resource> constBuff; // 定数バッファ
	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff;
	// ローカルスケール
	XMFLOAT3 scale = { 1,1,1 };
	//パーティクル配列
	std::forward_list<Particle> particles;
	// コマンドリスト
	ID3D12GraphicsCommandList* cmdList;
	// シェーダリソースビューのハンドル(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	// シェーダリソースビューのハンドル(CPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView;
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView;
	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap;
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
};