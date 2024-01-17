//スプライト一つ一つのクラス

#pragma once
#include "SpriteCommon.h"
#include <DirectXMath.h>

using namespace DirectX;
using namespace Microsoft::WRL;

class Sprite
{
	//構造体
public:
	//頂点データ構造体
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 uv;
	};

	//定数バッファ(マテリアル)
	struct ConstBufferDataMaterial {
		XMFLOAT4 color;
	};

	struct ConstBufferDataTransform {
		XMMATRIX mat;
	};

	enum VertexNumber {
		LB,
		LT,
		RB,
		RT
	};

public:

	//初期化
	void Initialize(SpriteCommon* spriteCommon, uint32_t textureIndex = UINT32_MAX);
	//更新処理
	void Update();
	//描画処理
	void Draw();

	//ゲッター
	XMFLOAT4 GetColor() const { return color; }
	const XMFLOAT2 GetPosition() const { return position; }
	const float& GetRotationZ() const { return rotationZ; }
	const XMFLOAT2 GetSize() const { return size; }
	const XMFLOAT2 GetAnchorPoint() const { return anchorPoint; }
	const bool& GetIsFlipX() const { return isFlipX; }
	const bool& GetIsFlipY() const { return isFlipY; }
	const bool& GetIsInvisible() const { return isInvisible; }
	const uint32_t GetTextureIndex() const { return textureIndex; }
	const XMFLOAT2 GetTextureLeftTop() const { return textureLeftTop; }
	const XMFLOAT2 GetTextureSize() const { return textureSize; }

	//セッター
	void SetColor(XMFLOAT4 color_) { this->color = color_; }
	void SetAlpha(float alpha) { color.w = alpha; }
	void SetPosition(const XMFLOAT2& position_) { this->position = position_; }
	void SetRotationZ(const float& rotationZ_) { this->rotationZ = rotationZ_; }
	void SetSize(const XMFLOAT2& size_) { this->size = size_; }
	void SetAnchorPoint(const XMFLOAT2& anchorPoint_) { this->anchorPoint = anchorPoint_; }
	void SetIsFlipX(const bool& isFlipX_) { this->isFlipX = isFlipX_; }
	void SetIsFlipY(const bool& isFlipY_) { this->isFlipY = isFlipY_; }
	void SetIsInvisible(const bool& isInvisible_) { this->isInvisible = isInvisible_; }
	void SetTextureIndex(const uint32_t& textureIndex_) { this->textureIndex = textureIndex_; }
	void SetTextureLeftTop(const XMFLOAT2& textureLeftTop_) { this->textureLeftTop = textureLeftTop_; }
	void SetTextureSize(const XMFLOAT2& textureSize_) { this->textureSize = textureSize_; }

private:
	//初期のテクスチャのサイズを読み込み適用
	void AdjustTextureSize();

private:

	SpriteCommon* spriteCommon = nullptr;

	uint32_t textureIndex = 0;

	XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };

	float rotationZ=0;
	XMFLOAT2 position = {};

	XMFLOAT2 size = { 200.0f,200.0f };

	XMFLOAT2 anchorPoint = { 0.0f,0.0f };

	bool isFlipX = false;
	bool isFlipY = false;

	bool isInvisible = false;

	Vertex vertices[4]= {};

	XMFLOAT2 textureLeftTop = { 0.0f,0.0f };

	XMFLOAT2 textureSize = { 100.0f,100.0f };

	//ワールド変換行列
	XMMATRIX matWorld = {};
	XMMATRIX matRot = {};
	XMMATRIX matTrans = {};

	XMMATRIX matProjection = {};



	ComPtr<ID3D12Resource> vertBuff;

	D3D12_VERTEX_BUFFER_VIEW vbView{};

	ComPtr<ID3D12Resource> constBuffMaterial;
	ConstBufferDataMaterial* constMapMaterial = nullptr;

	ComPtr<ID3D12Resource> constBufferTransform;
	ConstBufferDataTransform* constMapTransform = nullptr;

};