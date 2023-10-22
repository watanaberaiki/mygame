//当たり判定の可視化するための3Dモデルクラス

#pragma once
#include "vector"
#include "DirectXMath.h"
#include "DirectXCommon.h"
#include "DirectXTex.h"
#include "windows.h"
#include "wrl.h"
#include "d3d12.h"
#include "d3dx12.h"

class CubeModel
{
private:	//エイリアス
//Microsoft::WRL::を省略
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;
	using TexMetadata = DirectX::TexMetadata;
	using ScracthImage = DirectX::ScratchImage;
	//std::を省略
	using string = std::string;
	template<class T>using vector = std::vector<T>;

public:
	//バッファ生成
	void CreateBuffers(ID3D12Device* device);
	//頂点生成
	void CreateVertex();
	//色設定
	void SetImageData(XMFLOAT4 color);
	//更新
	void Update();
	//描画
	void Draw(ID3D12GraphicsCommandList* cmdList);
	//ゲッター
	XMFLOAT3 GetColPositionMin() { return colPositionMin; }
	XMFLOAT3 GetColPositionMax() { return colPositionMax; };
public:
	//頂点データ配列
	struct VertexPosNormalUv
	{
		XMFLOAT3 pos = {};	//座標
		XMFLOAT3 normal = {};	//法線ベクトル
		XMFLOAT2 uv = {};	//uv座標
		VertexPosNormalUv* parent = nullptr;	//uv座標
	};
	//頂点データ配列
	vector<VertexPosNormalUv>vertices;
	//頂点インデックス配列
	vector<unsigned short>indices;
private:
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	//インデックスバッファ
	ComPtr<ID3D12Resource>indexBuff;
	//テクスチャバッファ
	ComPtr<ID3D12Resource>texBuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	//SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap>descHeapSRV;
	//アンビエント係数
	XMFLOAT3 ambient = { 1,1,1 };
	//ディフューズ係数
	XMFLOAT3 diffuse = { 1,1,1 };
	//テクスチャメタデータ
	TexMetadata metadata = {};
	//スクラッチイメージ
	ScracthImage scratchImg = {};
	//画像用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> srvHeap;
	const UINT textureWidth = 256;
	const UINT textureHeight = 256;
	const UINT imageDataCount = textureWidth * textureHeight;
	XMFLOAT4* imageData = new XMFLOAT4[imageDataCount];
	//テクスチャーのGPUのハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = {};
	//判定用
	XMFLOAT3 colPositionMin = { 0,0,0 };
	XMFLOAT3 colPositionMax = { 0,0,0 };
};

