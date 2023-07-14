#include "PostEffect.h"
#include<d3dx12.h>
#include"winapp/WinApp.h"

using namespace DirectX;

//静的メンバ変数の実体
const float PostEffect::clearColor[4] = { 0.25f,0.5f,0.1f,0.0f };//緑っぽい色

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
	CD3DX12_CLEAR_VALUE colordesc = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);
	//テクスチャバッファの生成
	result = spriteCommon_->GetDirectXCommon()->GetDevice()->CreateCommittedResource(
		&propertiesdesc,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&colordesc,
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
	//RTV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
	rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescHeapDesc.NumDescriptors = 1;
	//RTV用デスクリプタヒープを設定
	result = spriteCommon_->GetDirectXCommon()->GetDevice()->CreateDescriptorHeap(&rtvDescHeapDesc, IID_PPV_ARGS(&descHeapRTV));
	assert(SUCCEEDED(result));

	//レンダーターゲットビューの設定
	D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
	//シェーダーの計算結果をSRGBに変換して書き込む
	renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	renderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	//デスクリプタヒープにRTV作成
	spriteCommon_->GetDirectXCommon()->GetDevice()->CreateRenderTargetView
	(
		texBuff.Get(),
		&renderTargetViewDesc,
		descHeapRTV->GetCPUDescriptorHandleForHeapStart()
	);

	//深度バッファリソース設定
	CD3DX12_RESOURCE_DESC depthResDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D32_FLOAT,
			WinApp::window_width,
			WinApp::window_height,
			1, 0,
			1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		);
	CD3DX12_HEAP_PROPERTIES propertiedesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_CLEAR_VALUE cleardesc = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);
	//深度バッファの生成
	result = spriteCommon_->GetDirectXCommon()->GetDevice()->CreateCommittedResource(
		&propertiedesc,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&cleardesc,
		IID_PPV_ARGS(&depthBuff));
	assert(SUCCEEDED(result));

	//DSV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC DescHeapDesc{};
	DescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DescHeapDesc.NumDescriptors = 1;
	//DSV用デスクリプタヒープを作成
	result = spriteCommon_->GetDirectXCommon()->GetDevice()->CreateDescriptorHeap(&DescHeapDesc, IID_PPV_ARGS(&descHeapDSV));
	assert(SUCCEEDED(result));
	//デスクリプタヒープにDSV作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;//深度フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	spriteCommon_->GetDirectXCommon()->GetDevice()->CreateDepthStencilView(
		depthBuff.Get(),
		&dsvDesc,
		descHeapDSV->GetCPUDescriptorHandleForHeapStart());

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
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	/*ID3D12DescriptorHeap* ppHeaps[] = { spriteCommon_->Getsrv().Get() };*/
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };
	spriteCommon_->GetDirectXCommon()->GetCommandlist()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	////テクスチャコマンド
	//spriteCommon_->SetTextureCommands(textureIndex_);

	//SRVヒープの先頭にあるSRVをルートパラメータ1番に設定
	spriteCommon_->GetDirectXCommon()->GetCommandlist()->SetGraphicsRootDescriptorTable
	(1, descHeapSRV->GetGPUDescriptorHandleForHeapStart());


	spriteCommon_->GetDirectXCommon()->GetCommandlist()->SetGraphicsRootConstantBufferView(0, constBuffMaterial.Get()->GetGPUVirtualAddress());

	spriteCommon_->GetDirectXCommon()->GetCommandlist()->SetGraphicsRootConstantBufferView(2, constBufferTransform.Get()->GetGPUVirtualAddress());

	//描画
	spriteCommon_->GetDirectXCommon()->GetCommandlist()->DrawInstanced(4, 1, 0, 0);



}

void PostEffect::PreDrawScene(ID3D12GraphicsCommandList* cmdList)
{
	CD3DX12_RESOURCE_BARRIER resourcedesc = CD3DX12_RESOURCE_BARRIER::Transition(texBuff.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	//リソースバリアを変更(シェーダーリソー→描画可能)
	cmdList->ResourceBarrier(
		1,
		&resourcedesc);

	//レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH =
		descHeapRTV->GetCPUDescriptorHandleForHeapStart();
	//深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH =
		descHeapDSV->GetCPUDescriptorHandleForHeapStart();
	//レンダーターゲットをセット
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

	//ビューポートの設定
	CD3DX12_VIEWPORT viewportdesc = CD3DX12_VIEWPORT(0.0f, 0.0f, WinApp::window_width, WinApp::window_height);
	cmdList->RSSetViewports(1, &viewportdesc);
	//シザリング矩形の設定
	CD3DX12_RECT rectdesc = CD3DX12_RECT(0, 0, WinApp::window_width, WinApp::window_height);
	cmdList->RSSetScissorRects(1, &rectdesc);

	//全画面クリア
	cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	//深度バッファのクリア
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void PostEffect::PostDrawScene(ID3D12GraphicsCommandList* cmdList)
{
	//リソースバリアを変更(描画可能→シェーダーリソース)
	CD3DX12_RESOURCE_BARRIER resourcedesc = CD3DX12_RESOURCE_BARRIER::Transition(texBuff.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	cmdList->ResourceBarrier
	(1,&resourcedesc);
}
