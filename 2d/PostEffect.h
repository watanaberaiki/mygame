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
    //シーン描画前処理
    void PreDrawScene(ID3D12GraphicsCommandList* cmdList);
    //シーン描画後処理
    void PostDrawScene(ID3D12GraphicsCommandList* cmdList);
    //パイプライン生成
    void CreateGraphicsPipelineState();
private://静的メンバ変数
    static const float clearColor[4];

private://メンバ変数
    //テクスチャバッファ
    ComPtr<ID3D12Resource>texBuff;
    //SRV用デスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap>descHeapSRV;
    //深度バッファ
    ComPtr<ID3D12Resource> depthBuff;
    //RTV用デスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap>descHeapRTV;
    //DSV用デスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap>descHeapDSV;
    //グラフィックスパイプライン
    ComPtr<ID3D12PipelineState>pipelineState;
    //ルートシグネチャ
    ComPtr<ID3D12RootSignature>rootSignature;

};