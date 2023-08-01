#include "Sprite.h"

void Sprite::Initialize(SpriteCommon* spriteCommon, uint32_t textureIndex)
{
	HRESULT result{};
	assert(spriteCommon);
	spriteCommon_ = spriteCommon;

	if (textureIndex != UINT32_MAX) {
		textureIndex_ = textureIndex;
		AdjustTextureSize();
		size_ = textureSize_;
	}

	ID3D12Resource* textureBuffer = spriteCommon_->GetTextureBuffer(textureIndex_);
	if (textureBuffer)
	{
		//テクスチャ情報取得
		D3D12_RESOURCE_DESC resDesc = textureBuffer->GetDesc();

		float tex_left = textureLeftTop_.x / resDesc.Width;
		float tex_right = (textureLeftTop_.x + textureSize_.x) / resDesc.Width;
		float tex_top = textureLeftTop_.y / resDesc.Height;
		float tex_bottom = (textureLeftTop_.y + textureSize_.y) / resDesc.Height;

		vertices[LB].uv = { tex_left, tex_bottom };
		vertices[LT].uv = { tex_left, tex_top };
		vertices[RB].uv = { tex_right, tex_bottom };
		vertices[RT].uv = { tex_right, tex_top };

	}

#pragma region 頂点データ(3点分の座標)

	float left = (0.0f - anchorPoint_.x) * size_.x;
	float right = (1.0f - anchorPoint_.x) * size_.x;
	float top = (0.0f - anchorPoint_.y) * size_.y;
	float bottom = (1.0f - anchorPoint_.y) * size_.y;
	//頂点データ
					//   x      y      z  
	vertices[LB].pos = { left,bottom, 0.0f };
	vertices[LT].pos = { left,   top, 0.0f };
	vertices[RB].pos = { right,bottom, 0.0f };
	vertices[RT].pos = { right,   top, 0.0f };

	if (isFlipX) {
		left = -left;
		right = -right;
	}
	if (isFlipY) {
		top = -top;
		bottom = -bottom;
	}

	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

#pragma endregion 

#pragma region 頂点バッファの確保

	// 頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{}; // ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用
	// リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; // 頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 頂点バッファの生成
	result = spriteCommon_->GetDirectXCommon()->GetDevice()->CreateCommittedResource(
		&heapProp, // ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

#pragma endregion 

#pragma region 頂点バッファへのデータ転送

	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	// 全頂点に対して
	for (int i = 0; i < _countof(vertices); i++) {
		vertMap[i] = vertices[i]; // 座標をコピー
	}
	// 繋がりを解除
	vertBuff->Unmap(0, nullptr);

#pragma endregion 

#pragma region 頂点バッファビューの作成



	// GPU仮想アドレス
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vbView.SizeInBytes = sizeVB;
	// 頂点1つ分のデータサイズ
	vbView.StrideInBytes = sizeof(vertices[0]);


#pragma endregion 

#pragma region 色の定数バッファ
	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;


	//定数バッファの生成
	result = spriteCommon_->GetDirectXCommon()->GetDevice()->CreateCommittedResource(&cbHeapProp, D3D12_HEAP_FLAG_NONE, &cbResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&constBuffMaterial));
	assert(SUCCEEDED(result));

	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);
	assert(SUCCEEDED(result));

	constMapMaterial->color = color;

#pragma endregion

#pragma region 3D変換行列の定数バッファ

	{


		//ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		//リソース設定
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;
		cbResourceDesc.Height = 1;
		cbResourceDesc.DepthOrArraySize = 1;
		cbResourceDesc.MipLevels = 1;
		cbResourceDesc.SampleDesc.Count = 1;
		cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;


		//定数バッファの生成
		result = spriteCommon_->GetDirectXCommon()->GetDevice()->CreateCommittedResource(&cbHeapProp, D3D12_HEAP_FLAG_NONE, &cbResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&constBufferTransform));
		assert(SUCCEEDED(result));

		//定数バッファのマッピング
		result = constBufferTransform->Map(0, nullptr, (void**)&constMapTransform);
		assert(SUCCEEDED(result));



		rotationZ = 0.0f;
		position_ = { 0.0f,0.0f };



		//透視投影行列の計算
		matProjection =
			XMMatrixOrthographicOffCenterLH(
				0.0f, WinApp::window_width,
				WinApp::window_height, 0.0f,
				0.0f, 1.0f
			);


		//カメラを動かさない場合
		constMapTransform->mat = matProjection;


		/*constMapTransform->mat.r[0].m128_f32[0] = 2.0f / WinApp::window_width;
		constMapTransform->mat.r[1].m128_f32[1] = -2.0f / WinApp::window_height;
		constMapTransform->mat.r[3].m128_f32[0] = -1.0f;
		constMapTransform->mat.r[3].m128_f32[1] = 1.0f;*/
	}

#pragma endregion

}

void Sprite::Update() {

	ID3D12Resource* textureBuffer = spriteCommon_->GetTextureBuffer(textureIndex_);
	if (textureBuffer)
	{
		//テクスチャ情報取得
		D3D12_RESOURCE_DESC resDesc = textureBuffer->GetDesc();

		float tex_left = textureLeftTop_.x / resDesc.Width;
		float tex_right = (textureLeftTop_.x + textureSize_.x) / resDesc.Width;
		float tex_top = textureLeftTop_.y / resDesc.Height;
		float tex_bottom = (textureLeftTop_.y + textureSize_.y) / resDesc.Height;

		vertices[LB].uv = { tex_left, tex_bottom };
		vertices[LT].uv = { tex_left, tex_top };
		vertices[RB].uv = { tex_right, tex_bottom };
		vertices[RT].uv = { tex_right, tex_top };

	}

	float left = (0.0f - anchorPoint_.x) * size_.x;
	float right = (1.0f - anchorPoint_.x) * size_.x;
	float top = (0.0f - anchorPoint_.y) * size_.y;
	float bottom = (1.0f - anchorPoint_.y) * size_.y;

	if (isFlipX) {
		left = -left;
		right = -right;
	}
	if (isFlipY) {
		top = -top;
		bottom = -bottom;
	}

	//頂点データ
					  //   x      y      z
	vertices[LB].pos = { left,bottom, 0.0f };
	vertices[LT].pos = { left,   top, 0.0f };
	vertices[RB].pos = { right,bottom, 0.0f };
	vertices[RT].pos = { right,   top, 0.0f };

	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	Vertex* vertMap = nullptr;
	HRESULT result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	// 全頂点に対して
	for (int i = 0; i < _countof(vertices); i++) {
		vertMap[i] = vertices[i]; // 座標をコピー
	}

	// 繋がりを解除
	vertBuff->Unmap(0, nullptr);

	constMapMaterial->color = color;

	matWorld = XMMatrixIdentity();

	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotationZ));

	matWorld *= matRot;

	matTrans = XMMatrixTranslation(position_.x, position_.y, 0.0f);
	matWorld *= matTrans;

	constMapTransform->mat = matWorld * matProjection;

}
void Sprite::Draw() {

	if (isInvisible_) {
		return;
	}

	//テクスチャコマンド
	spriteCommon_->SetTextureCommands(textureIndex_);


	spriteCommon_->GetDirectXCommon()->GetCommandlist()->IASetVertexBuffers(0, 1, &vbView);

	spriteCommon_->GetDirectXCommon()->GetCommandlist()->SetGraphicsRootConstantBufferView(0, constBuffMaterial.Get()->GetGPUVirtualAddress());

	spriteCommon_->GetDirectXCommon()->GetCommandlist()->SetGraphicsRootConstantBufferView(2, constBufferTransform.Get()->GetGPUVirtualAddress());


	spriteCommon_->GetDirectXCommon()->GetCommandlist()->DrawInstanced(4, 1, 0, 0);


}

void Sprite::AdjustTextureSize() {
	ID3D12Resource* textureBuffer = spriteCommon_->GetTextureBuffer(textureIndex_);

	D3D12_RESOURCE_DESC resDesc = textureBuffer->GetDesc();

	textureSize_.x = static_cast<float>(resDesc.Width);
	textureSize_.y = static_cast<float>(resDesc.Height);
}
