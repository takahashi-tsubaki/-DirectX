#include "Sprite.h"

void Sprite::Initialize(SpriteManager* spManager)
{
	HRESULT result;
	spManager_ = spManager;

	//頂点データの座標    x       y       z      u   v
	vertices[0] = { { 0.0f, 100.0f, 0.0f }, {0.0f,1.0f} };//左下
	vertices[1] = { { 0.0f, 0.0f, 0.0f }, {0.0f,0.0f} };//右下
	vertices[2] = { {100.0f, 100.0f, 0.0f }, {1.0f,1.0f} };//左上
	vertices[3] = { { 100.0f, 0.0f, 0.0f }, {1.0f,0.0f} };//右上
	//頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点の要素数
	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());

#pragma region 頂点バッファ
	//頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{};//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB;//頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//頂点バッファの作成
	ID3D12Resource* vertBuff = nullptr;
	result = spManager_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&heapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	Vertex* vertMap = nullptr;
	/*XMFLOAT3* vertMap = nullptr;*/
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//全頂点に対して
	for (int i = 0; i < vertices.size(); i++)
	{
		vertMap[i] = vertices[i];//座標をコピー
	}

	//繋がりを解除
	vertBuff->Unmap(0, nullptr);

	//GPU仮想アドレス
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//頂点バッファのサイズ
	vbView.SizeInBytes = sizeVB;
	//頂点1つ分のデータサイズ
	vbView.StrideInBytes = sizeof(Vertex);


	//テクスチャ
	TexMetadata metaData{};
	ScratchImage scratchImg{};

	result = LoadFromWICFile(
		L"Resources/mario.jpg",
		WIC_FLAGS_NONE,
		&metaData, scratchImg
	);

	ScratchImage mipChain{};
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain
	);
	if (SUCCEEDED(result))
	{
		scratchImg = std::move(mipChain);
		metaData = scratchImg.GetMetadata();
	}

	//読み込んだディフューズテクスチャをSRGBとして扱う
	metaData.format = MakeSRGB(metaData.format);

	//ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metaData.format;
	textureResourceDesc.Width = metaData.width;
	textureResourceDesc.Height = (UINT)metaData.height;
	textureResourceDesc.DepthOrArraySize = (UINT16)metaData.arraySize;
	textureResourceDesc.MipLevels = (UINT16)metaData.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	//テクスチャバッファを生成
	ID3D12Resource* textureBuff = nullptr;
	result = spManager_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&textureHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureBuff));

	for (size_t i = 0; i < metaData.mipLevels; i++)
	{
		//ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//テクスチャバッファにデータ転送
		result = textureBuff->WriteToSubresource(
			(UINT)i,//
			nullptr,//全領域へコピー
			img->pixels,//元データアドレス
			(UINT)img->rowPitch,//1ラインサイズ
			(UINT)img->slicePitch//全サイズ
		);
		assert(SUCCEEDED(result));
	}

	const size_t kMaxSRVCount = 2056;//SRV = シェーダリソースビュー
	//デスクリプタヒープの設定(生成)
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NumDescriptors = kMaxSRVCount;


	result = spManager_->GetDxCommon()->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));

	//SPVヒープの先頭ハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();

	//シェーダリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = resDesc.MipLevels;

	//ハンドルのさす位置にシェーダリソースビュー作成
	spManager_->GetDxCommon()->GetDevice()->CreateShaderResourceView(textureBuff, &srvDesc, srvHandle);
#pragma endregion

#pragma region 定数バッファ
	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//3D変換リソース
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファの生成
	result = spManager_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial));
	assert(SUCCEEDED(result));

	//定数バッファの生成(3D)
	result = spManager_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffTransform));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);
	assert(SUCCEEDED(result));


	result = constBuffTransform->Map(0, nullptr, (void**)&constMapTransform);
	assert(SUCCEEDED(result));

	//値を書き込むと自動的に転送される
	constMapMaterial->color = XMFLOAT4(1, 0, 0, 0.5f);

	// 単位行列を代入
	constMapTransform->mat = XMMatrixIdentity();
	constMapTransform->mat.r[0].m128_f32[0] = 2.0f / WinApp::window_width;
	constMapTransform->mat.r[1].m128_f32[1] = -2.0f / WinApp::window_height;
	constMapTransform->mat.r[3].m128_f32[0] = -1.0f;
	constMapTransform->mat.r[3].m128_f32[1] = 1.0f;

#pragma endregion
}

void Sprite::Draw()
{

	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(0.0f);
	matTrans = XMMatrixTranslation(position_.x, position_.y, 0.0f);
	//単位行列の代入
	matWorld = XMMatrixIdentity();
	matWorld *= matRot;
	matWorld *= matTrans;

	//定数バッファへ転送
	constMapTransform->mat *= matWorld;

	sCommandList_ = spManager_->GetDxCommon()->GetCommandList();
	
	sCommandList_->SetPipelineState(spManager_->GetPipelineState());
	sCommandList_->SetGraphicsRootSignature(spManager_->GetRootSignature());

		//定数バッファビュー(CBV)の設定コマンド
	sCommandList_->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

	//頂点バッファービューの設定コマンド
	sCommandList_->IASetVertexBuffers(0, 1, &vbView);


	//プリミティブ形状の設定コマンド
	sCommandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//SRVヒープ設定コマンド
	sCommandList_->SetDescriptorHeaps(1, &srvHeap);

	//SRVヒープの先頭ハンドルを取得
	srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();

	//SRVヒープの先頭にあるSRVをルートパラメータの1番に設定
	sCommandList_->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

	//定数バッファビュー(CBV)の設定コマンド
	sCommandList_->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());

	//全ての頂点を使って描画
	sCommandList_->DrawInstanced(vertices.size(), 1, 0, 0);
}

void Sprite::preDraw()
{

}

void Sprite::postDraw()
{

}

void Sprite::SetPosition(XMFLOAT2 position)
{
	position_ = position;

	TransferVertex();
}

void Sprite::TransferVertex()
{

}
