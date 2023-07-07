#include "PostEffect.h"
#include<d3dx12.h>

using namespace DirectX;

void PostEffect::Initialize(DirectXCommon* dxcommon, uint32_t textureIndex)
{
	SpriteCommon* spritecommon = new SpriteCommon();
	spritecommon->Initialize(dxcommon);
	spritecommon->LoadTexture(textureIndex, "white1x1.png");
	spriteCommon_ = spritecommon;
	textureIndex_ = textureIndex;
	Sprite::Initialize(spriteCommon_, textureIndex_);
}

PostEffect::PostEffect()
{
	
}

void PostEffect::Draw(ID3D12GraphicsCommandList* cmdList)
{

	if (isInvisible_) {
		return;
	}

	Update();

	//テクスチャコマンド
	spriteCommon_->SetTextureCommands(textureIndex_);

	ID3D12DescriptorHeap* ppHeaps[] = { srvHeap.Get() };
	dxcommon_->GetCommandlist()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//パイプラインステートの設定
	cmdList->SetPipelineState(pipelineState.Get());
	//ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(rootSignature.Get());
	//プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	spriteCommon_->GetDirectXCommon()->GetCommandlist()->IASetVertexBuffers(0, 1, &vbView);

	spriteCommon_->GetDirectXCommon()->GetCommandlist()->SetGraphicsRootConstantBufferView(0, constBuffMaterial.Get()->GetGPUVirtualAddress());

	spriteCommon_->GetDirectXCommon()->GetCommandlist()->SetGraphicsRootConstantBufferView(2, constBufferTransform.Get()->GetGPUVirtualAddress());


	

	spriteCommon_->GetDirectXCommon()->GetCommandlist()->DrawInstanced(4, 1, 0, 0);



}
