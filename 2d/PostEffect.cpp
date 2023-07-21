#include "PostEffect.h"
#include<d3dx12.h>
#include"WinApp.h"

#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX;

//�ÓI�����o�ϐ��̎���
const float PostEffect::clearColor[4] = { 0.25f,0.5f,0.1f,0.0f };//�΂��ۂ��F

void PostEffect::Initialize()
{
	CreateGraphicsPipelineState();

	HRESULT result;

	rotationZ = 0.0f;
	position_ = { 0.0f,0.0f };

	CD3DX12_HEAP_PROPERTIES heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resource = CD3DX12_RESOURCE_DESC::Buffer(sizeof(Vertex) * vertNum);

	// ���_�o�b�t�@�̐���
	result = spriteCommon_->GetDirectXCommon()->GetDevice()->CreateCommittedResource(
		&heap, // �q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resource, // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//���_�f�[�^
	Vertex vertices[vertNum] = {
		{{-0.5f,-0.5f,0.0f},{0.0f,1.0f}},//����
		{{-0.5f,+0.5f,0.0f},{0.0f,0.0f}},//����
		{{+0.5f,-0.5f,0.0f},{1.0f,1.0f}},//�E��
		{{+0.5f,+0.5f,0.0f},{1.0f,0.0f}},//�E��
	};

	//���_�o�b�t�@�ւ̃f�[�^�]��
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices, sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}

	//���_�o�b�t�@�r���[�̍쐬
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(Vertex) * 4;
	vbView.StrideInBytes = sizeof(Vertex);


	resource = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataTransform) + 0xff) & ~0xff);

	//�萔�o�b�t�@�̐���
	result = spriteCommon_->GetDirectXCommon()->GetDevice()->CreateCommittedResource(
		&heap, // �q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resource, // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBufferTransform));
	assert(SUCCEEDED(result));


	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;


	////�萔�o�b�t�@�̐���
	//result = spriteCommon_->GetDirectXCommon()->GetDevice()->CreateCommittedResource(&cbHeapProp, D3D12_HEAP_FLAG_NONE, &cbResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&constBuffMaterial));
	//assert(SUCCEEDED(result));

	//result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);
	//assert(SUCCEEDED(result));

	//constMapMaterial->color = color;


	//�e�N�X�`�����\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texresDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
			WinApp::window_width,
			(UINT)WinApp::window_height,
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

	CD3DX12_HEAP_PROPERTIES propertiesdesc = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	CD3DX12_CLEAR_VALUE colordesc = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);
	for (int i = 0; i < 2; i++) {
		//�e�N�X�`���o�b�t�@�̐���
		result = spriteCommon_->GetDirectXCommon()->GetDevice()->CreateCommittedResource(
			&propertiesdesc,
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&colordesc,
			IID_PPV_ARGS(&texBuff[i]));
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
			for (int j = 0; j < pixelCount; j++) { img[j] = 0xff0000ff; }
			//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
			result = texBuff[i]->WriteToSubresource(0, nullptr, img, rowPitch, depthPitch);
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
			texBuff[0].Get(),
			&srvDesc,
			descHeapSRV->GetCPUDescriptorHandleForHeapStart()
		);
		//RTV�p�f�X�N���v�^�q�[�v�ݒ�
		D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
		rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvDescHeapDesc.NumDescriptors = 2;
		//RTV�p�f�X�N���v�^�q�[�v��ݒ�
		result = spriteCommon_->GetDirectXCommon()->GetDevice()->CreateDescriptorHeap(&rtvDescHeapDesc, IID_PPV_ARGS(&descHeapRTV));
		assert(SUCCEEDED(result));

		//�����_�[�^�[�Q�b�g�r���[�̐ݒ�
		D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
		//�V�F�[�_�[�̌v�Z���ʂ�SRGB�ɕϊ����ď�������
		renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		renderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		//�f�X�N���v�^�q�[�v��RTV�쐬
		for (int i = 0; i < 2; i++) {
			spriteCommon_->GetDirectXCommon()->GetDevice()->CreateRenderTargetView
			(
				texBuff[i].Get(),
				&renderTargetViewDesc,
				CD3DX12_CPU_DESCRIPTOR_HANDLE(
					descHeapRTV->GetCPUDescriptorHandleForHeapStart(),
					i,
					spriteCommon_->GetDirectXCommon()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
				)
			);
		}

		//�[�x�o�b�t�@���\�[�X�ݒ�
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
		//�[�x�o�b�t�@�̐���
		result = spriteCommon_->GetDirectXCommon()->GetDevice()->CreateCommittedResource(
			&propertiedesc,
			D3D12_HEAP_FLAG_NONE,
			&depthResDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&cleardesc,
			IID_PPV_ARGS(&depthBuff));
		assert(SUCCEEDED(result));

		//DSV�p�f�X�N���v�^�q�[�v�ݒ�
		D3D12_DESCRIPTOR_HEAP_DESC DescHeapDesc{};
		DescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		DescHeapDesc.NumDescriptors = 1;
		//DSV�p�f�X�N���v�^�q�[�v���쐬
		result = spriteCommon_->GetDirectXCommon()->GetDevice()->CreateDescriptorHeap(&DescHeapDesc, IID_PPV_ARGS(&descHeapDSV));
		assert(SUCCEEDED(result));
		//�f�X�N���v�^�q�[�v��DSV�쐬
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;//�[�x�t�H�[�}�b�g
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		spriteCommon_->GetDirectXCommon()->GetDevice()->CreateDepthStencilView(
			depthBuff.Get(),
			&dsvDesc,
			descHeapDSV->GetCPUDescriptorHandleForHeapStart());
	}
}

PostEffect::PostEffect()
{

}

void PostEffect::Draw(ID3D12GraphicsCommandList* cmdList, Input* input)
{

	if (isInvisible_) {
		return;
	}
	if (input->TriggerKey(DIK_0)) {
		//�f�X�N���v�^�q�[�v��SRV�쐬
		static int tex = 0;
		//�e�N�X�`���ԍ���0��1�Ő؂�ւ�
		tex = (tex + 1) % 2;

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		spriteCommon_->GetDirectXCommon()->GetDevice()->CreateShaderResourceView(texBuff[tex].Get(),
			&srvDesc,
			descHeapSRV->GetCPUDescriptorHandleForHeapStart());


	}
	/*constMapMaterial->color = color;*/

	//�萔�o�b�t�@�Ƀf�[�^�]��
	ConstBufferDataTransform* constMap = nullptr;
	HRESULT result = this->constBufferTransform->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result)) {
		constMap->mat = XMMatrixIdentity();
		this->constBufferTransform->Unmap(0, nullptr);
	}

	spriteCommon_->GetDirectXCommon()->GetCommandlist()->IASetVertexBuffers(0, 1, &vbView);



	//�p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(pipelineState.Get());
	//���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(rootSignature.Get());
	//�v���~�e�B�u�`���ݒ�
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	/*ID3D12DescriptorHeap* ppHeaps[] = { spriteCommon_->Getsrv().Get() };*/
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };
	spriteCommon_->GetDirectXCommon()->GetCommandlist()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	////�e�N�X�`���R�}���h
	//spriteCommon_->SetTextureCommands(textureIndex_);

	//SRV�q�[�v�̐擪�ɂ���SRV�����[�g�p�����[�^1�Ԃɐݒ�
	spriteCommon_->GetDirectXCommon()->GetCommandlist()->SetGraphicsRootDescriptorTable
	(1, descHeapSRV->GetGPUDescriptorHandleForHeapStart());


	/*spriteCommon_->GetDirectXCommon()->GetCommandlist()->SetGraphicsRootConstantBufferView(0, constBuffMaterial.Get()->GetGPUVirtualAddress());*/

	spriteCommon_->GetDirectXCommon()->GetCommandlist()->SetGraphicsRootConstantBufferView(2, constBufferTransform.Get()->GetGPUVirtualAddress());

	//�`��
	spriteCommon_->GetDirectXCommon()->GetCommandlist()->DrawInstanced(4, 1, 0, 0);



}

void PostEffect::PreDrawScene(ID3D12GraphicsCommandList* cmdList)
{
	for (int i = 0; i < 2; i++) {
		CD3DX12_RESOURCE_BARRIER resourcedesc = CD3DX12_RESOURCE_BARRIER::Transition(texBuff[i].Get(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET);
		//���\�[�X�o���A��ύX(�V�F�[�_�[���\�[���`��\)
		cmdList->ResourceBarrier(
			1,
			&resourcedesc);
	}
	//�����_�[�^�[�Q�b�g�r���[�p�f�B�X�N���v�^�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHs[2];
	for (int i = 0; i < 2; i++) {
		rtvHs[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeapRTV->GetCPUDescriptorHandleForHeapStart(), i,
			spriteCommon_->GetDirectXCommon()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	}
	//�[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH =
		descHeapDSV->GetCPUDescriptorHandleForHeapStart();
	//�����_�[�^�[�Q�b�g���Z�b�g
	cmdList->OMSetRenderTargets(2, rtvHs, false, &dsvH);

	//�r���[�|�[�g�̐ݒ�
	CD3DX12_VIEWPORT viewportdesc[2];
	//�V�U�����O��`�̐ݒ�
	CD3DX12_RECT rectdesc[2];
	for (int i = 0; i < 2; i++) {
		viewportdesc[i] = CD3DX12_VIEWPORT(0.0f, 0.0f, WinApp::window_width, WinApp::window_height);
		rectdesc[i] = CD3DX12_RECT(0, 0, WinApp::window_width, WinApp::window_height);
	}
	cmdList->RSSetViewports(2, viewportdesc);

	cmdList->RSSetScissorRects(2, rectdesc);

	//�S��ʃN���A
	for (int i = 0; i < 2; i++) {
		cmdList->ClearRenderTargetView(rtvHs[i], clearColor, 0, nullptr);
	}
	//�[�x�o�b�t�@�̃N���A
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void PostEffect::PostDrawScene(ID3D12GraphicsCommandList* cmdList)
{
	//���\�[�X�o���A��ύX(�`��\���V�F�[�_�[���\�[�X)
	for (int i = 0; i < 2; i++) {
		CD3DX12_RESOURCE_BARRIER resourcedesc = CD3DX12_RESOURCE_BARRIER::Transition(texBuff[i].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		cmdList->ResourceBarrier(1, &resourcedesc);
	}

}

void PostEffect::CreateGraphicsPipelineState()
{
	HRESULT result = S_FALSE;
	ID3DBlob* vsBlob = nullptr; // ���_�V�F�[�_�I�u�W�F�N�g
	ID3DBlob* psBlob = nullptr; // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ID3DBlob* errorBlob = nullptr; // �G���[�I�u�W�F�N�g

	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shaders/PostEffectTestVS.hlsl", // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);
	// �G���[�Ȃ�
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shaders/PostEffectTestPS.hlsl", // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&psBlob, &errorBlob);


#pragma endregion


#pragma region �V�F�[�_�[�R�[�h�̃G���[


	// �G���[�Ȃ�
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	{
	"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	D3D12_APPEND_ALIGNED_ELEMENT,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}, // (1�s�ŏ������ق������₷��)
	{
	"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	D3D12_APPEND_ALIGNED_ELEMENT,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}, // (1�s�ŏ������ق������₷��)
	};

	// �O���t�B�b�N�X�p�C�v���C���ݒ� �O���t�B�b�N�X�p�C�v���C���̊e�X�e�[�W�̐ݒ������\���̂�p�ӂ���B
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	// �V�F�[�_�[�̐ݒ� ���_�V�F�[�_�A�s�N�Z���V�F�[�_���p�C�v���C���ɐݒ�
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// �T���v���}�X�N�̐ݒ�
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�

	// ���X�^���C�U�̐ݒ�
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // �J�����O���Ȃ�
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // �|���S�����h��Ԃ�
	pipelineDesc.RasterizerState.DepthClipEnable = true; // �[�x�N���b�s���O��L����
	////�f�X�X�e���V���X�e�[�g
	//pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	//pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;//��ɏ㏑��

	// �u�����h�X�e�[�g
	// �u�����h�X�e�[�g
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA�S�Ẵ`�����l����`��

	//���ʐݒ�(�A���t�@�l)
	blenddesc.BlendEnable = true;                   // �u�����h��L���ɂ���
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;    // ���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;      // �\�[�X�̒l��100% �g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;    // �f�X�g�̒l��  0% �g��

	// ���Z����
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD; // ���Z
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;   // �\�[�X�̒l��100% �g��
	//blenddesc.DestBlend = D3D12_BLEND_ONE;  // �f�X�g�̒l��100% �g��

	// ���Z����
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;    // �f�X�g����\�[�X�����Z
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;               // �\�[�X�̒l��100% �g��
	//blenddesc.DestBlend = D3D12_BLEND_ONE;              // �f�X�g�̒l��100% �g��

	// �F���]
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;             // ���Z
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;    // 1.0f-�f�X�g�J���[�̒l
	//blenddesc.DestBlend = D3D12_BLEND_ZERO;             // �g��Ȃ�

	// ����������
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;             // ���Z
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;         // �\�[�X�̃A���t�@�l
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;    // 1.0f-�\�[�X�̃A���t�@�l

	pipelineDesc.BlendState.RenderTarget[0] = blenddesc;

	//�[�x�o�b�t�@�̃t�H�[�}�b�g
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// ���_���C�A�E�g�̐ݒ�
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	// �}�`�̌`��ݒ�
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// ���̑��̐ݒ�
	pipelineDesc.NumRenderTargets = 1; // �`��Ώۂ�1��
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255�w���RGBA
	pipelineDesc.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	//�f�X�N���v�^�����W�̐ݒ�
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1; //��x�̕`��Ɏg���e�N�X�`����1���Ȃ̂�1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	////�e�N�X�`���T���v���[�̐ݒ�
	//D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	//samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	//samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	//samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	//samplerDesc.MinLOD = 0.0f;
	//samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


	//���[�g�p�����[�^�̐ݒ�
	D3D12_ROOT_PARAMETER rootParams[3] = {};

	//�萔�o�b�t�@0��
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //���
	rootParams[0].Descriptor.ShaderRegister = 0; //�萔�o�b�t�@�ԍ�
	rootParams[0].Descriptor.RegisterSpace = 0; //�f�t�H���g�l
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�ẴV�F�[�_���猩����

	//�e�N�X�`�����W�X�^0��
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //���
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange; //�f�X�N���v�^�����W
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1; //�f�X�N���v�^�����W��
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�ẴV�F�[�_���猩����


	//�萔�o�b�t�@1��
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //���
	rootParams[2].Descriptor.ShaderRegister = 1; //�萔�o�b�t�@�ԍ�
	rootParams[2].Descriptor.RegisterSpace = 0; //�f�t�H���g�l
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //�S�ẴV�F�[�_���猩����

	//�X�^�e�B�b�N�T���v���[
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0,
		D3D12_FILTER_MIN_MAG_MIP_POINT);
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;


	// ���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;
	rootSignatureDesc.NumParameters = _countof(rootParams);
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	// ���[�g�V�O�l�`���̃V���A���C�Y
	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = spriteCommon_->GetDirectXCommon()->GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();
	// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	pipelineDesc.pRootSignature = rootSignature.Get();



	result = spriteCommon_->GetDirectXCommon()->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));

}
