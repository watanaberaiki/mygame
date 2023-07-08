#include "CubeModel.h"

void CubeModel::CreateBuffers(ID3D12Device* device)
{
	HRESULT result;

	//���_�A�C���f�b�N�X�T�C�Y�ݒ�
	vertices.resize(24);
	indices.resize(36);

	//���_�f�[�^����
	CreateVertex();

	//���_�f�[�^�S�̂̃T�C�Y
	size_t sizeVB = static_cast<size_t>(sizeof(VertexPosNormalUv)) * vertices.size();

	//���_�o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp{};	//�q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//CPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB;	//���_�f�[�^�S�̂̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	result = device->CreateCommittedResource(
		&heapProp,	//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc,	//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);
	assert(SUCCEEDED(result));

	//���_�o�b�t�@�ւ̃f�[�^�]��
	//GPU��̃o�b�t�@�ɑΉ��������z�������i���C����������j���擾
	VertexPosNormalUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//�S���_�ɑ΂���
	for (int i = 0; i < vertices.size(); i++)
	{
		vertMap[i] = vertices[i];	//���W���R�s�[
	}
	//�Ȃ��������
	vertBuff->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[�̍쐬
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = (UINT)sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);

	//���_�C���f�b�N�X�S�̂̃T�C�Y
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());

	//�C���f�b�N�X�o�b�t�@�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//CPU�ւ̓]���p
	//���\�[�X�ݒ�
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB;	//�C���f�b�N�X��񂪓��镪�̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//�C���f�b�N�X�o�b�t�@����
	result = device->CreateCommittedResource(
		&heapProp,	//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc,	//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);

	//�C���f�b�N�X�o�b�t�@���}�b�s���O
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	//�S�C���f�b�N�X�ɑ΂���
	for (int i = 0; i < indices.size(); i++)
	{
		indexMap[i] = indices[i];	//�C���f�b�N�X���R�s�[
	}
	//�}�b�s���O����
	indexBuff->Unmap(0, nullptr);

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;


	//�e�N�X�`���ݒ�
	UINT incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);	//�f�X�N���v�^�̃T�C�Y
	imageData = new XMFLOAT4[imageDataCount];
	//�S�s�N�Z����������
	for (size_t i = 0; i < imageDataCount; i++)
	{
		imageData[i].x = 1.0f;
		imageData[i].y = 0.0f;
		imageData[i].z = 0.0f;
		imageData[i].w = 1.0f;
	}

	//�e�N�X�`���o�b�t�@�ݒ�
	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureResourceDesc.Width = textureWidth;	//��
	textureResourceDesc.Height = textureHeight;	//����
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.SampleDesc.Count = 1;
	//�e�N�X�`���o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff)
	);
	//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	result = texBuff->WriteToSubresource(
		0,
		nullptr,
		imageData,
		sizeof(XMFLOAT4) * textureWidth,
		sizeof(XMFLOAT4) * imageDataCount
	);

	//�f�X�N���v�^�q�[�v����
	//SRV�̍ő��
	const size_t kMaxSRVCount = 2056;

	//�f�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_�[���猩����悤��
	srvHeapDesc.NumDescriptors = kMaxSRVCount;

	//�ݒ�����Ƃ�SRV�p�f�X�N���v�^�q�[�v�𐶐�
	result = device->CreateDescriptorHeap(
		&srvHeapDesc,
		IID_PPV_ARGS(&srvHeap)
	);
	assert(SUCCEEDED(result));

	//SRV�q�[�v�̐擪�n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	//�����܂�

	//�e�N�X�`���[�̔ԍ���0�ȍ~�̏ꍇ�n���h����i�߂�
	/*if (texNum > 0)
	{
		srvHandle.ptr += (incrementSize * texNum);
	}*/
	//�V�F�[�_���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//�ݒ�\����
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;
	//�n���h���̎w���ʒu�ɃV�F�[�_���\�[�X�r���[�쐬
	device->CreateShaderResourceView(
		texBuff.Get(),
		&srvDesc,
		srvHeap->GetCPUDescriptorHandleForHeapStart()
	);
}

void CubeModel::CreateVertex()
{
	//���̈�̊�b�T�C�Y
	XMFLOAT3 size = { 1.0f,1.0f,1.0f };
	//���_�f�[�^
	VertexPosNormalUv v[] = {
		//�O
		{{-size.x / 2,-size.y / 2,-size.z / 2},{},{0.0f,1.0f} },	//0
		{{-size.x / 2, size.y / 2,-size.z / 2},{},{0.0f,0.0f} },	//1 
		{{ size.x / 2,-size.y / 2,-size.z / 2},{},{1.0f,1.0f} },	//2 
		{{ size.x / 2, size.y / 2,-size.z / 2},{},{1.0f,0.0f} },	//3
		//��				 	   
		{{ size.x / 2,-size.y / 2, size.z / 2},{},{0.0f,1.0f} },	//4
		{{ size.x / 2, size.y / 2, size.z / 2},{},{0.0f,0.0f} },	//5
		{{-size.x / 2,-size.y / 2, size.z / 2},{},{1.0f,1.0f} },	//6
		{{-size.x / 2, size.y / 2, size.z / 2},{},{1.0f,0.0f} },	//7
		//��				 	    
		{{-size.x / 2,-size.y / 2,-size.z / 2},{},{0.0f,1.0f} },	//8
		{{-size.x / 2,-size.y / 2, size.z / 2},{},{0.0f,0.0f} },	//9
		{{-size.x / 2, size.y / 2,-size.z / 2},{},{1.0f,1.0f} },	//10
		{{-size.x / 2, size.y / 2, size.z / 2},{},{1.0f,0.0f} },	//11
		//�E				 	    
		{{ size.x / 2,-size.y / 2,-size.z / 2},{},{0.0f,1.0f} },	//12
		{{ size.x / 2,-size.y / 2, size.z / 2},{},{0.0f,0.0f} },	//13
		{{ size.x / 2, size.y / 2,-size.z / 2},{},{1.0f,1.0f} },	//14
		{{ size.x / 2, size.y / 2, size.z / 2},{},{1.0f,0.0f} },	//15
		//��					  	
		{{-size.x / 2,-size.y / 2, size.z / 2},{},{0.0f,1.0f} },	//16
		{{-size.x / 2,-size.y / 2,-size.z / 2},{},{0.0f,0.0f} },	//17
		{{ size.x / 2,-size.y / 2, size.z / 2},{},{1.0f,1.0f} },	//18
		{{ size.x / 2,-size.y / 2,-size.z / 2},{},{1.0f,0.0f} },	//19
		//��				 	    
		{{-size.x / 2, size.y / 2,-size.z / 2},{},{0.0f,1.0f} },	//20
		{{-size.x / 2, size.y / 2, size.z / 2},{},{0.0f,0.0f} },	//21
		{{ size.x / 2, size.y / 2,-size.z / 2},{},{1.0f,1.0f} },	//22
		{{ size.x / 2, size.y / 2, size.z / 2},{},{1.0f,0.0f} },	//23
	};
	//�C���f�b�N�X�f�[�^
	unsigned short in[] =
	{

		//�O
		0,1,2,	//�O�p�`1��
		2,1,3,	//�O�p�`2��
		//��
		4,5,6,
		6,5,7,
		//��
		8,9,10,
		10,9,11,
		//�E
		12,13,14,
		14,13,15,
		//��
		16,17,18,
		18,17,19,
		//��
		20,21,22,
		22,21,23,
	};

	//���_���W�Auv���W�A�C���f�b�N�X�f�[�^����
	for (int i = 0; i < 24; i++)
	{
		vertices[i] = v[i];
	}

	for (int i = 0; i < 36; i++)
	{
		indices[i] = in[i];
	}

	//�@���̌v�Z
	for (int i = 0; i < indices.size() / 3; i++)
	{//�O�p�`1���ƂɌv�Z���Ă���
		//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
		unsigned short indices0 = indices[i * 3 + 0];
		unsigned short indices1 = indices[i * 3 + 1];
		unsigned short indices2 = indices[i * 3 + 2];
		//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
		XMVECTOR p0 = XMLoadFloat3(&vertices[indices0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[indices1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[indices2].pos);
		//p0��p1�x�N�g���Ap0��p2�x�N�g�����v�Z�@(�x�N�g���̌��Z)
		XMVECTOR v1 = DirectX::XMVectorSubtract(p1, p0);
		XMVECTOR v2 = DirectX::XMVectorSubtract(p2, p0);
		//�O�ς͗������琂���ȃx�N�g��
		XMVECTOR normal = DirectX::XMVector3Cross(v1, v2);
		//���K��
		normal = DirectX::XMVector3Normalize(normal);
		//���߂��@���𒸓_�f�[�^�ɑ��
		DirectX::XMStoreFloat3(&vertices[indices0].normal, normal);
		DirectX::XMStoreFloat3(&vertices[indices1].normal, normal);
		DirectX::XMStoreFloat3(&vertices[indices2].normal, normal);
	}

	//����
	for (int i = 0; i < 24; i++)
	{
		//������
		if (colpositionmin.x >= vertices[i].pos.x) {
			colpositionmin.x = vertices[i].pos.x;
		}
		if (colpositionmin.y >= vertices[i].pos.y) {
			colpositionmin.y = vertices[i].pos.y;
		}
		if (colpositionmin.z >= vertices[i].pos.z) {
			colpositionmin.z = vertices[i].pos.z;
		}
		
		//�傫��
		if (colpositionmax.x >= vertices[i].pos.x) {
			colpositionmax.x = vertices[i].pos.x;
		}
		if (colpositionmax.y >= vertices[i].pos.y) {
			colpositionmax.y = vertices[i].pos.y;
		}
		if (colpositionmax.z >= vertices[i].pos.z) {
			colpositionmax.z = vertices[i].pos.z;
		}

	}
}

void CubeModel::SetImageData(XMFLOAT4 color)
{
	HRESULT result;
	for (size_t i = 0; i < imageDataCount; i++)
	{
		imageData[i].x = color.x;
		imageData[i].y = color.y;
		imageData[i].z = color.z;
		imageData[i].w = color.w;
	}
	//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	result = texBuff->WriteToSubresource(
		0,
		nullptr,
		imageData,
		sizeof(XMFLOAT4) * textureWidth,
		sizeof(XMFLOAT4) * imageDataCount
	);
}

void CubeModel::Update()
{
	//-----���̏�ɒ��_�̍X�V����������-----

	HRESULT result;
	//���_�f�[�^�S�̂̃T�C�Y
	size_t sizeVB = static_cast<size_t>(sizeof(VertexPosNormalUv)) * vertices.size();

	//���_�o�b�t�@�ւ̃f�[�^�]��
	//GPU��̃o�b�t�@�ɑΉ��������z�������i���C����������j���擾
	VertexPosNormalUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//�S���_�ɑ΂���
	for (int i = 0; i < vertices.size(); i++)
	{
		vertMap[i] = vertices[i];	//���W���R�s�[
	}
	//�Ȃ��������
	vertBuff->Unmap(0, nullptr);
}

void CubeModel::Draw(ID3D12GraphicsCommandList* cmdList)
{
	//���_�o�b�t�@���Z�b�g
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	//�C���f�b�N�X�o�b�t�@���Z�b�g
	cmdList->IASetIndexBuffer(&ibView);

	//�f�X�N���v�^�q�[�v�̃Z�b�g
	ID3D12DescriptorHeap* ppHeaps[] = { srvHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//�V�F�[�_���\�[�X�r���[���Z�b�g
	cmdList->SetGraphicsRootDescriptorTable(1, srvHeap->GetGPUDescriptorHandleForHeapStart());

	//�`��R�}���h
	cmdList->DrawIndexedInstanced((UINT)indices.size(), 1, 0, 0, 0);
}
