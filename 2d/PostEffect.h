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
    //�V�[���`��O����
    void PreDrawScene(ID3D12GraphicsCommandList* cmdList);
    //�V�[���`��㏈��
    void PostDrawScene(ID3D12GraphicsCommandList* cmdList);
    //�p�C�v���C������
    void CreateGraphicsPipelineState();
private://�ÓI�����o�ϐ�
    static const float clearColor[4];

private://�����o�ϐ�
    //�e�N�X�`���o�b�t�@
    ComPtr<ID3D12Resource>texBuff;
    //SRV�p�f�X�N���v�^�q�[�v
    ComPtr<ID3D12DescriptorHeap>descHeapSRV;
    //�[�x�o�b�t�@
    ComPtr<ID3D12Resource> depthBuff;
    //RTV�p�f�X�N���v�^�q�[�v
    ComPtr<ID3D12DescriptorHeap>descHeapRTV;
    //DSV�p�f�X�N���v�^�q�[�v
    ComPtr<ID3D12DescriptorHeap>descHeapDSV;
    //�O���t�B�b�N�X�p�C�v���C��
    ComPtr<ID3D12PipelineState>pipelineState;
    //���[�g�V�O�l�`��
    ComPtr<ID3D12RootSignature>rootSignature;

};