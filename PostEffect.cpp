#include "PostEffect.h"
#include<d3dx12.h>

using namespace DirectX;

PostEffect::PostEffect()
{
	
}

void PostEffect::Draw(ID3D12GraphicsCommandList* cmdList)
{
	//定数バッファビューの設定
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
	//デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = {};
}
