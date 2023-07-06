#include "PostEffect.h"
#include<d3dx12.h>

using namespace DirectX;

void PostEffect::Initialize(SpriteCommon* spriteCommon, uint32_t textureIndex)
{
	spriteCommon_ = spriteCommon;
	Sprite::Initialize(spriteCommon_, textureIndex);
}

PostEffect::PostEffect()
{
	
}

void PostEffect::Draw(ID3D12GraphicsCommandList* cmdList)
{
	Update();


	////テクスチャの設定
	//SetTextureCommands(textureIndex_);

	//定数バッファビューの設定
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
	/*cmdList->SetGraphicsRootConstantBufferView(2, constBufferTransform->GetGPUVirtualAddress());*/

	////デスクリプタヒープの配列
	//ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };

	//パイプラインステートの設定
	cmdList->SetPipelineState(pipelineState.Get());
	//ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(rootSignature.Get());
	//プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);



}
