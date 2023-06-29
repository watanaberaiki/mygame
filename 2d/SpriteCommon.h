#pragma once
#include "DirectXCommon.h"
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include <array>
#include<string>

#pragma comment(lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;
using namespace DirectX;

//スプライト共通部
class SpriteCommon
{

public:
	//初期化
	void Initialize(DirectXCommon* dxCommon);

	void PreDraw();

	void PostDraw();
	void LoadTexture(uint32_t index, const std::string& fileName);

	void SetTextureCommands(uint32_t index);

	DirectXCommon* GetDirectXCommon() { return dxcommon_; }

	ID3D12Resource* GetTextureBuffer(uint32_t index) { return texBuff[index].Get(); }


private:
	//SRVの最大個数
	static const size_t kMaxSRVCount = 2056;

	static std::string kDefaultTextureDirectoryPath;

private:
	DirectXCommon* dxcommon_=nullptr;

	// パイプランステートの生成
	ComPtr<ID3D12PipelineState> pipelineState;

	// ルートシグネチャは、テクスチャや定数バッファなどシェーダーに渡すリソースの情報をまとめたオブジェクト。
	ComPtr<ID3D12RootSignature> rootSignature;


	//テクスチャバッファの生成
	std::array<ComPtr<ID3D12Resource>, kMaxSRVCount> texBuff;

	ComPtr<ID3D12DescriptorHeap> srvHeap;
};