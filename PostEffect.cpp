#include "PostEffect.h"
#include<d3dx12.h>
#include"winapp/WinApp.h"

using namespace DirectX;

void PostEffect::Initialize()
{
	HRESULT result;

	//���N���X�Ƃ��Ă̏�����
	Sprite::Initialize();

	//�e�N�X�`�����\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texresDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
			WinApp::window_width,
			(UINT)WinApp::window_height,
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

	CD3DX12_HEAP_PROPERTIES propertiesdesc = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	//�e�N�X�`���o�b�t�@�̐���
	result = spriteCommon_->GetDirectXCommon()->GetDevice()->CreateCommittedResource(
		&propertiesdesc,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&texBuff));
	assert(SUCCEEDED(result));

	{//�e�N�X�`����ԃN���A
		//��f��(1280x720=921600�s�N�Z��)
		const UINT pixelCount = WinApp::window_width * WinApp::window_height;
		//�摜1�s���̃f�[�^�T�C�Y
		const UINT rowPitch = sizeof(UINT) * WinApp::window_width;
		//�摜�S�̂̃f�[�^�T�C�Y
		const UINT depthPitch = rowPitch * WinApp::window_height;
		//�摜�C���[�W
		UINT* img = new UINT[pixelCount];
		for (int i = 0; i < pixelCount; i++) { img[i] = 0xff0000ff; }
		//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		result = texBuff->WriteToSubresource(0, nullptr, img, rowPitch, depthPitch);
		assert(SUCCEEDED(result));
		delete[]img;
	}

	//SRV�p�f�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = 1;
	//SRV�p�f�X�N���v�^�q�[�v�𐶐�
	result = spriteCommon_->GetDirectXCommon()->GetDevice()->CreateDescriptorHeap(&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV));
	assert(SUCCEEDED(result));
	//SRV�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;
	//�f�X�N���v�^�q�[�v��SRV�쐬
	spriteCommon_->GetDirectXCommon()->GetDevice()->CreateShaderResourceView
	(
		texBuff.Get(),
		&srvDesc,
		descHeapSRV->GetCPUDescriptorHandleForHeapStart()
	);
}

PostEffect::PostEffect()
{

}

void PostEffect::Draw(ID3D12GraphicsCommandList* cmdList)
{

	if (isInvisible_) {
		return;
	}

	Update();

	spriteCommon_->GetDirectXCommon()->GetCommandlist()->IASetVertexBuffers(0, 1, &vbView);



	//�p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(spriteCommon_->GetPipeLineState().Get());
	//���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(spriteCommon_->GetRootSignature().Get());
	//�v���~�e�B�u�`���ݒ�
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/*ID3D12DescriptorHeap* ppHeaps[] = { spriteCommon_->Getsrv().Get() };*/
	ID3D12DescriptorHeap* ppHeaps[] = {descHeapSRV.Get()};
	spriteCommon_->GetDirectXCommon()->GetCommandlist()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	////�e�N�X�`���R�}���h
	//spriteCommon_->SetTextureCommands(textureIndex_);

	//SRV�q�[�v�̐擪�ɂ���SRV�����[�g�p�����[�^1�Ԃɐݒ�
	spriteCommon_->GetDirectXCommon()->GetCommandlist()->SetGraphicsRootDescriptorTable
	(1,descHeapSRV->GetGPUDescriptorHandleForHeapStart());


	spriteCommon_->GetDirectXCommon()->GetCommandlist()->SetGraphicsRootConstantBufferView(0, constBuffMaterial.Get()->GetGPUVirtualAddress());

	spriteCommon_->GetDirectXCommon()->GetCommandlist()->SetGraphicsRootConstantBufferView(2, constBufferTransform.Get()->GetGPUVirtualAddress());

	//�`��
	spriteCommon_->GetDirectXCommon()->GetCommandlist()->DrawInstanced(4, 1, 0, 0);



}
