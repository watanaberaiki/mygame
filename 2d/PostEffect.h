#pragma once
#include "Sprite.h"
#include"SpriteCommon.h"
class PostEffect :
    public SpriteCommon,public Sprite
    
{
public:
    //������
    void Initialize(DirectXCommon* dxcommon, uint32_t textureIndex);
    //�R���X�g���N�^
    PostEffect();
    //�`��
    void Draw(ID3D12GraphicsCommandList*cmdList);
};

