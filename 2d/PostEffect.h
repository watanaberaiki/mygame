#pragma once
#include "Sprite.h"
class PostEffect :
    public Sprite
{
public:
    //コンストラクタ
    PostEffect();
    //描画
    void Draw(ID3D12GraphicsCommandList*cmdList);
};

