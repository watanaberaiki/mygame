#pragma once
#include "Sprite.h"
#include"SpriteCommon.h"
class PostEffect :
   public Sprite
    
{
public:
    //初期化
    void Initialize();
    //コンストラクタ
    PostEffect();
    //描画
    void Draw(ID3D12GraphicsCommandList*cmdList);

private:
    //テクスチャバッファ
    ComPtr<ID3D12Resource>texBuff;
    //SRV用デスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap>descHeapSRV;
};