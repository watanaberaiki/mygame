#pragma once
#include "Sprite.h"
#include"SpriteCommon.h"
class PostEffect :
    public SpriteCommon,public Sprite
    
{
public:
    //初期化
    void Initialize(SpriteCommon* spriteCommon, uint32_t textureIndex);
    //コンストラクタ
    PostEffect();
    //描画
    void Draw(ID3D12GraphicsCommandList*cmdList);
};

