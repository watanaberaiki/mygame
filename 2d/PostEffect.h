#pragma once
#include "Sprite.h"
#include"SpriteCommon.h"
class PostEffect :
   public Sprite
    
{
public:
    //������
    void Initialize();
    //�R���X�g���N�^
    PostEffect();
    //�`��
    void Draw(ID3D12GraphicsCommandList*cmdList);

private:
    //�e�N�X�`���o�b�t�@
    ComPtr<ID3D12Resource>texBuff;
    //SRV�p�f�X�N���v�^�q�[�v
    ComPtr<ID3D12DescriptorHeap>descHeapSRV;
};