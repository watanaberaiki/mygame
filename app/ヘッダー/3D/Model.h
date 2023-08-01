#pragma once
#include <d3dx12.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include <Windows.h>
#include<wrl.h>



using namespace DirectX;
using namespace Microsoft::WRL;

class Model
{

public:
	//OBJファイルから3Dモデルを読み込む
	static Model* LoadFromObj(const std::string& modelname);
	//デバイスセッター
	static void SetDevice(ID3D12Device* device) { Model::device = device; }

private:
	//OBJファイルから3Dモデルを読み込む
	void LoadFromOBJInternal(const std::string& modelname);
	//バッファ生成
	void CreateBuff();
	//マテリアル読み込み
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);
	//テクスチャ読み込み
	void LoadTexture(const std::string& directoryPath, const std::string& filename);
	//デスクリプタヒープの初期化
	void InitializeDescriptorHeap();
public://メンバ関数
	//描画
	void Draw(ID3D12GraphicsCommandList*cmdList,UINT rootParamIndexMaterial);
	//ゲッター
	XMFLOAT3 GetminModel()const { return minModel; }
	XMFLOAT3 GetmaxModel()const { return maxModel; }
public:
	//サブクラス
	// 頂点データ構造体
	struct VertexPosNormalUv
	{
		XMFLOAT3 pos; // xyz座標
		XMFLOAT3 normal; // 法線ベクトル
		XMFLOAT2 uv;  // uv座標
	};
	//マテリアル
	struct Material
	{
		std::string name; //マテリアル名
		XMFLOAT3 ambient; //アンビエント影響度
		XMFLOAT3 diffuse; //ディフューズ影響度
		XMFLOAT3 specular; //スペキュラー影響度
		float alpha; //アルファ
		std::string textureFilename; //テクスチャファイル名
		//コンストラクタ
		Material() {
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.0f,0.0f,0.0f };
			specular = { 0.0f,0.0f,0.0f };
			alpha = 1.0f;
		}
	};
	//定数バッファ用
	struct ConstBufferDataB1 {
		XMFLOAT3 ambient;	//アンビエント係数
		float pad1;			//パティング
		XMFLOAT3 diffuse;	//ディフーズ係数
		float pad2;			//パティング
		XMFLOAT3 specular;	//スペキュラー係数
		float alpha;		//アルファ
	};

private://メンバ変数
	// 頂点データ配列
	std::vector<VertexPosNormalUv>vertices;
	// 頂点インデックス配列
	std::vector<unsigned short>indices;
	//マテリアル
	Material material;
	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap;
	// デスクリプタサイズ
	UINT descriptorHandleIncrementSize;
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;
	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView;
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView;
	// シェーダリソースビューのハンドル(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	// シェーダリソースビューのハンドル(CPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	// 定数バッファ
	ComPtr<ID3D12Resource> constBuffB1;
	//最小、最大
	XMFLOAT3 minModel = {}, maxModel = {};
private://静的メンバ変数
	// デバイス
	static ID3D12Device* device;
};

