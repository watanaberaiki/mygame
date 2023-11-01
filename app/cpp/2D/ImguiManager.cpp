#include "ImguiManager.h"

#include<imgui_impl_win32.h>
#include<imgui_impl_dx12.h>

void ImguiManager::Initialize(WinApp*winApp_, DirectXCommon* dxCommon_)
{
	dxCommon = dxCommon_;
	//ImGuiのコンテキストを生成
	ImGui::CreateContext();
	//ImGuiのスタイルを設定
	ImGui::StyleColorsDark();

	//win32用初期化
	ImGui_ImplWin32_Init(winApp_->GetHwnd());

	//デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	//デスクリプタヒープ生成
	HRESULT result;
	result = dxCommon_->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));
	//DirectX12用初期化
	ImGui_ImplDX12_Init(
		dxCommon_->GetDevice(),
		static_cast<int>(dxCommon_->GetBackBufferCount()),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,srvHeap.Get(),
		srvHeap->GetCPUDescriptorHandleForHeapStart(),
		srvHeap->GetGPUDescriptorHandleForHeapStart()
		);

	ImGuiIO& io = ImGui::GetIO();
	//標準フォントを追加する
	io.Fonts->AddFontDefault();
}

void ImguiManager::Finalize()
{
	//後始末
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//デスクリプタヒープ
	srvHeap.Reset();
}

void ImguiManager::Begin()
{
	//ImGuiフレーム開始
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImguiManager::End()
{
	ImGui::Render();
}

void ImguiManager::Draw()
{
	ID3D12GraphicsCommandList* commandList = dxCommon->GetCommandlist();

	//デスクリプタヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = { srvHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//描画コマンドを発行
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}
