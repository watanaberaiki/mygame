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


	////�e�N�X�`���̐ݒ�
	//SetTextureCommands(textureIndex_);

	//�萔�o�b�t�@�r���[�̐ݒ�
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
	/*cmdList->SetGraphicsRootConstantBufferView(2, constBufferTransform->GetGPUVirtualAddress());*/

	////�f�X�N���v�^�q�[�v�̔z��
	//ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };

	//�p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(pipelineState.Get());
	//���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(rootSignature.Get());
	//�v���~�e�B�u�`���ݒ�
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);



}
