#include "PostEffect.h"
#include<d3dx12.h>

using namespace DirectX;

PostEffect::PostEffect()
{
	
}

void PostEffect::Draw(ID3D12GraphicsCommandList* cmdList)
{
	//�萔�o�b�t�@�r���[�̐ݒ�
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
	//�f�X�N���v�^�q�[�v�̔z��
	ID3D12DescriptorHeap* ppHeaps[] = {};
}
