#include "CubeModel.h"

void CubeModel::CreateBuffers(ID3D12Device* device)
{
	HRESULT result;

	//頂点、インデックスサイズ設定
	vertices.resize(24);
	indices.resize(36);

	//頂点データ生成
	CreateVertex();

	//頂点データ全体のサイズ
	size_t sizeVB = static_cast<size_t>(sizeof(VertexPosNormalUv)) * vertices.size();

	//頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{};	//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//CPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB;	//頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	result = device->CreateCommittedResource(
		&heapProp,	//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,	//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);
	assert(SUCCEEDED(result));

	//頂点バッファへのデータ転送
	//GPU上のバッファに対応した仮想メモリ（メインメモリ上）を取得
	VertexPosNormalUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//全頂点に対して
	for (int i = 0; i < vertices.size(); i++)
	{
		vertMap[i] = vertices[i];	//座標をコピー
	}
	//つながりを解除
	vertBuff->Unmap(0, nullptr);

	//頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = (UINT)sizeVB;
	vbView.StrideInBytes = sizeof(vertices[0]);

	//頂点インデックス全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());

	//インデックスバッファ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//CPUへの転送用
	//リソース設定
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB;	//インデックス情報が入る分のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//インデックスバッファ生成
	result = device->CreateCommittedResource(
		&heapProp,	//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,	//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);

	//インデックスバッファをマッピング
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	//全インデックスに対して
	for (int i = 0; i < indices.size(); i++)
	{
		indexMap[i] = indices[i];	//インデックスをコピー
	}
	//マッピング解除
	indexBuff->Unmap(0, nullptr);

	//インデックスバッファビューの作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;


	//テクスチャ設定
	//UINT incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);	//デスクリプタのサイズ
	imageData = new XMFLOAT4[imageDataCount];
	//全ピクセルを初期化
	for (size_t i = 0; i < imageDataCount; i++)
	{
		imageData[i].x = 1.0f;
		imageData[i].y = 0.0f;
		imageData[i].z = 0.0f;
		imageData[i].w = 1.0f;
	}

	//テクスチャバッファ設定
	//ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureResourceDesc.Width = textureWidth;	//幅
	textureResourceDesc.Height = textureHeight;	//高さ
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.SampleDesc.Count = 1;
	//テクスチャバッファの生成
	result = device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff)
	);
	//テクスチャバッファにデータ転送
	result = texBuff->WriteToSubresource(
		0,
		nullptr,
		imageData,
		sizeof(XMFLOAT4) * textureWidth,
		sizeof(XMFLOAT4) * imageDataCount
	);

	//デスクリプタヒープ生成
	//SRVの最大個数
	const size_t kMaxSRVCount = 2056;

	//デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダーから見えるように
	srvHeapDesc.NumDescriptors = kMaxSRVCount;

	//設定をもとにSRV用デスクリプタヒープを生成
	result = device->CreateDescriptorHeap(
		&srvHeapDesc,
		IID_PPV_ARGS(&srvHeap)
	);
	assert(SUCCEEDED(result));

	//SRVヒープの先頭ハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	//ここまで

	//テクスチャーの番号が0以降の場合ハンドルを進める
	/*if (texNum > 0)
	{
		srvHandle.ptr += (incrementSize * texNum);
	}*/
	//シェーダリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//設定構造体
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;
	//ハンドルの指す位置にシェーダリソースビュー作成
	device->CreateShaderResourceView(
		texBuff.Get(),
		&srvDesc,
		srvHeap->GetCPUDescriptorHandleForHeapStart()
	);
}

void CubeModel::CreateVertex()
{
	//球体一つの基礎サイズ
	XMFLOAT3 size = { 1.0f,1.0f,1.0f };
	//頂点データ
	VertexPosNormalUv v[] = {
		//前
		{{-size.x / 2,-size.y / 2,-size.z / 2},{},{0.0f,1.0f} },	//0
		{{-size.x / 2, size.y / 2,-size.z / 2},{},{0.0f,0.0f} },	//1 
		{{ size.x / 2,-size.y / 2,-size.z / 2},{},{1.0f,1.0f} },	//2 
		{{ size.x / 2, size.y / 2,-size.z / 2},{},{1.0f,0.0f} },	//3
		//後				 	   
		{{ size.x / 2,-size.y / 2, size.z / 2},{},{0.0f,1.0f} },	//4
		{{ size.x / 2, size.y / 2, size.z / 2},{},{0.0f,0.0f} },	//5
		{{-size.x / 2,-size.y / 2, size.z / 2},{},{1.0f,1.0f} },	//6
		{{-size.x / 2, size.y / 2, size.z / 2},{},{1.0f,0.0f} },	//7
		//左				 	    
		{{-size.x / 2,-size.y / 2,-size.z / 2},{},{0.0f,1.0f} },	//8
		{{-size.x / 2,-size.y / 2, size.z / 2},{},{0.0f,0.0f} },	//9
		{{-size.x / 2, size.y / 2,-size.z / 2},{},{1.0f,1.0f} },	//10
		{{-size.x / 2, size.y / 2, size.z / 2},{},{1.0f,0.0f} },	//11
		//右				 	    
		{{ size.x / 2,-size.y / 2,-size.z / 2},{},{0.0f,1.0f} },	//12
		{{ size.x / 2,-size.y / 2, size.z / 2},{},{0.0f,0.0f} },	//13
		{{ size.x / 2, size.y / 2,-size.z / 2},{},{1.0f,1.0f} },	//14
		{{ size.x / 2, size.y / 2, size.z / 2},{},{1.0f,0.0f} },	//15
		//下					  	
		{{-size.x / 2,-size.y / 2, size.z / 2},{},{0.0f,1.0f} },	//16
		{{-size.x / 2,-size.y / 2,-size.z / 2},{},{0.0f,0.0f} },	//17
		{{ size.x / 2,-size.y / 2, size.z / 2},{},{1.0f,1.0f} },	//18
		{{ size.x / 2,-size.y / 2,-size.z / 2},{},{1.0f,0.0f} },	//19
		//上				 	    
		{{-size.x / 2, size.y / 2,-size.z / 2},{},{0.0f,1.0f} },	//20
		{{-size.x / 2, size.y / 2, size.z / 2},{},{0.0f,0.0f} },	//21
		{{ size.x / 2, size.y / 2,-size.z / 2},{},{1.0f,1.0f} },	//22
		{{ size.x / 2, size.y / 2, size.z / 2},{},{1.0f,0.0f} },	//23
	};
	//インデックスデータ
	unsigned short in[] =
	{

		//前
		0,1,2,	//三角形1つ目
		2,1,3,	//三角形2つ目
		//後
		4,5,6,
		6,5,7,
		//左
		8,9,10,
		10,9,11,
		//右
		12,13,14,
		14,13,15,
		//下
		16,17,18,
		18,17,19,
		//上
		20,21,22,
		22,21,23,
	};

	//頂点座標、uv座標、インデックスデータを代入
	for (int i = 0; i < 24; i++)
	{
		vertices[i] = v[i];
	}

	for (int i = 0; i < 36; i++)
	{
		indices[i] = in[i];
	}

	//法線の計算
	for (int i = 0; i < indices.size() / 3; i++)
	{//三角形1つごとに計算していく
		//三角形のインデックスを取り出して、一時的な変数に入れる
		unsigned short indices0 = indices[i * 3 + 0];
		unsigned short indices1 = indices[i * 3 + 1];
		unsigned short indices2 = indices[i * 3 + 2];
		//三角形を構成する頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&vertices[indices0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[indices1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[indices2].pos);
		//p0→p1ベクトル、p0→p2ベクトルを計算　(ベクトルの減算)
		XMVECTOR v1 = DirectX::XMVectorSubtract(p1, p0);
		XMVECTOR v2 = DirectX::XMVectorSubtract(p2, p0);
		//外積は両方から垂直なベクトル
		XMVECTOR normal = DirectX::XMVector3Cross(v1, v2);
		//正規化
		normal = DirectX::XMVector3Normalize(normal);
		//求めた法線を頂点データに代入
		DirectX::XMStoreFloat3(&vertices[indices0].normal, normal);
		DirectX::XMStoreFloat3(&vertices[indices1].normal, normal);
		DirectX::XMStoreFloat3(&vertices[indices2].normal, normal);
	}

	//判定
	for (int i = 0; i < 24; i++)
	{
		//小さい
		if (colPositionMin.x >= vertices[i].pos.x) {
			colPositionMin.x = vertices[i].pos.x;
		}
		if (colPositionMin.y >= vertices[i].pos.y) {
			colPositionMin.y = vertices[i].pos.y;
		}
		if (colPositionMin.z >= vertices[i].pos.z) {
			colPositionMin.z = vertices[i].pos.z;
		}
		
		//大きい
		if (colPositionMax.x >= vertices[i].pos.x) {
			colPositionMax.x = vertices[i].pos.x;
		}
		if (colPositionMax.y >= vertices[i].pos.y) {
			colPositionMax.y = vertices[i].pos.y;
		}
		if (colPositionMax.z >= vertices[i].pos.z) {
			colPositionMax.z = vertices[i].pos.z;
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
	//テクスチャバッファにデータ転送
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
	//-----この上に頂点の更新処理を書く-----

	HRESULT result;
	//頂点データ全体のサイズ
	//size_t sizeVB = static_cast<size_t>(sizeof(VertexPosNormalUv)) * vertices.size();

	//頂点バッファへのデータ転送
	//GPU上のバッファに対応した仮想メモリ（メインメモリ上）を取得
	VertexPosNormalUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//全頂点に対して
	for (int i = 0; i < vertices.size(); i++)
	{
		vertMap[i] = vertices[i];	//座標をコピー
	}
	//つながりを解除
	vertBuff->Unmap(0, nullptr);
}

void CubeModel::Draw(ID3D12GraphicsCommandList* cmdList)
{
	//頂点バッファをセット
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	//インデックスバッファをセット
	cmdList->IASetIndexBuffer(&ibView);

	//デスクリプタヒープのセット
	ID3D12DescriptorHeap* ppHeaps[] = { srvHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//シェーダリソースビューをセット
	cmdList->SetGraphicsRootDescriptorTable(1, srvHeap->GetGPUDescriptorHandleForHeapStart());

	//描画コマンド
	cmdList->DrawIndexedInstanced((UINT)indices.size(), 1, 0, 0, 0);
}
