#pragma once
#include "Sprite.h"
class PostEffect :
    public Sprite
{
public:
    //�R���X�g���N�^
    PostEffect();
    //�`��
    void Draw(ID3D12GraphicsCommandList*cmdList);
};

