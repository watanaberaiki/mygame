#include "PostEffect.h"
#include<d3dx12.h>
#include"winapp/WinApp.h"

using namespace DirectX;

void PostEffect::Initialize()
{
	HRESULT result;

	//基底クラスとしての初期化
	Sprite::Initialize();

	//テクスチャリソース設定
	CD3DX12_RESOURCE_DESC texresDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
			WinApp::window_width,
			(UINT)WinApp::window_height,
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

	CD3DX12_HEAP_PROPERTIES propertiesdesc = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	//テクスチャバッファの生成
	result = spriteCommon_->GetDirectXCommon()->GetDevice()->CreateCommittedResource(
		&propertiesdesc,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&texBuff));
	assert(SUCCEEDED(result));

	{//テクスチャを赤クリア
		//画素数(1280x720=921600ピクセル)
		const UINT pixelCount = WinApp::window_width * WinApp::window_height;
		//画像1行分のデータサイズ
		const UINT rowPitch = sizeof(UINT) * WinApp::window_width;
		//画像全体のデータサイズ
		const UINT depthPitch = rowPitch * WinApp::window_height;
		//画像イメージ
		UINT* img = new UINT[pixelCount];
		for (int i = 0; i < pixelCount; i++) { img[i] = 0xff0000ff; }
		//テクスチャバッファにデータ転送
		result = texBuff->WriteToSubresource(0, nullptr, img, rowPitch, depthPitch);
		assert(SUCCEEDED(result));
		delete[]img;
	}

	//SRV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = 1;
	//SRV用デスクリプタヒープを生成
	result = spriteCommon_->GetDirectXCommon()->GetDevice()->CreateDescriptorHeap(&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV));
	assert(SUCCEEDED(result));
	//SRV設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;
	//デスクリプタヒープにSRV作成
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



	//パイプラインステートの設定
	cmdList->SetPipelineState(spriteCommon_->GetPipeLineState().Get());
	//ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(spriteCommon_->GetRootSignature().Get());
	//プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/*ID3D12DescriptorHeap* ppHeaps[] = { spriteCommon_->Getsrv().Get() };*/
	ID3D12DescriptorHeap* ppHeaps[] = {descHeapSRV.Get()};
	spriteCommon_->GetDirectXCommon()->GetCommandlist()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	////テクスチャコマンド
	//spriteCommon_->SetTextureCommands(textureIndex_);

	//SRVヒープの先頭にあるSRVをルートパラメータ1番に設定
	spriteCommon_->GetDirectXCommon()->GetCommandlist()->SetGraphicsRootDescriptorTable
	(1,descHeapSRV->GetGPUDescriptorHandleForHeapStart());


	spriteCommon_->GetDirectXCommon()->GetCommandlist()->SetGraphicsRootConstantBufferView(0, constBuffMaterial.Get()->GetGPUVirtualAddress());

	spriteCommon_->GetDirectXCommon()->GetCommandlist()->SetGraphicsRootConstantBufferView(2, constBufferTransform.Get()->GetGPUVirtualAddress());

	//描画
	spriteCommon_->GetDirectXCommon()->GetCommandlist()->DrawInstanced(4, 1, 0, 0);



}
