
#include "Input.h"
#include "Core/WinApp.h"
#include "Core/DirectXCommon.h"
#include "Core/FPS.h"

void DebugOutputFormatString(const char* format, ...) {
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	vprintf(format, valist);
	va_end(valist);
#endif
}

//関数のプロトタイプ宣言
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

#pragma region 入力関数
//キーの初期化処理関数
void keyInitialize(BYTE* key, BYTE* oldkey, int array);
//キーが押されてる時
bool pushKey(BYTE key, int keyNum);
//キーが押されてない時
bool notPushKey(BYTE* key, int keyNum);
//キーが押した時
bool pressKey(BYTE* key, BYTE* oldkey, int keyNum);
//キー長押ししてる時
bool triggerKey(BYTE* key, BYTE* oldkey, int keyNum);
//キーを離した時
bool releaseKey(BYTE* key, BYTE* oldkey, int keyNum);
#pragma endregion 入力関数

//定数バッファ
struct ConstBufferDataMaterial
{
	XMFLOAT4 color;//色
};
//定数バッファ用データ構造体(3D変換行列)
struct ConstBufferDataTransform
{
	XMMATRIX mat;//3D変換行列
};

//3Dオブジェクト型
struct Object3d
{
	//定数バッファ(行列用)
	ComPtr<ID3D12Resource> constBuffTransform;
	//定数バッファマップ(行列用)
	ConstBufferDataTransform* constMapTransform = 0;
	//アフィン変換情報
	XMFLOAT3 scale = { 1,1,1 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 translation = { 0,0,0 };
	//ワールド変換行列
	XMMATRIX matworld = XMMatrixIdentity();

	Object3d* parent = nullptr;
};

//オブジェクト初期化処理
void InitializeObject3d(Object3d* object, ID3D12Device* dev);
//オブジェクト更新処理
void UpdateObject3d(Object3d* object, XMMATRIX& matview, XMMATRIX& matProjection);
//オブジェクト描画処理
void DrawObject3d(Object3d* object, ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView, D3D12_INDEX_BUFFER_VIEW& ibView,UINT numIndices);

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	OutputDebugStringA("Hello DirectX!!\n");

	Input* input = nullptr;
	WinApp* winApp = nullptr;
	DirectXCommon* dxCommon = nullptr;
	FPS* fps = new FPS;

	winApp = new WinApp();
	winApp->Initialize();

	input = new Input();
	input->Initialize(winApp);

	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	MSG msg{};

	//ここからDirectX初期化処理


	HRESULT result;
	

	//ここから描画初期化処理

	//頂点データ構造体
	struct Vertex
	{
		XMFLOAT3 pos;//xyz座標
		XMFLOAT3 normal;
		XMFLOAT2 uv;//uv座標
	};

	Vertex vertices[] = 
	{
		//   x       y       z      法線   u    v
		//前
		{ {-5.0f , -5.0f , -5.0f} , {}, {0.0f,1.0f}},//左下 
		{ {-5.0f ,  5.0f , -5.0f} , {}, {0.0f,0.0f}},//左上 
		{ { 5.0f , -5.0f , -5.0f} , {}, {1.0f,1.0f}},//右下 
		{ { 5.0f ,  5.0f , -5.0f} , {}, {1.0f,0.0f}},//右上 
		//後
		{ {-5.0f , -5.0f ,  5.0f} , {}, {0.0f,1.0f}},//左下 
		{ {-5.0f ,  5.0f ,  5.0f} , {}, {0.0f,0.0f}},//左上 
		{ { 5.0f , -5.0f ,  5.0f} , {}, {1.0f,1.0f}},//右下 
		{ { 5.0f ,  5.0f ,  5.0f} , {}, {1.0f,0.0f}},//右上 
		//左						   
		{ {-5.0f , -5.0f ,  5.0f} , {}, {0.0f,1.0f}},//左上 → 左下
		{ {-5.0f ,  5.0f ,  5.0f} , {}, {0.0f,0.0f}},//右上 → 左上
		{ {-5.0f , -5.0f , -5.0f} , {}, {1.0f,1.0f}},//左下 → 右下
		{ {-5.0f ,  5.0f , -5.0f} , {}, {1.0f,0.0f}},//右下 → 右上
		//右						   
		{ { 5.0f , -5.0f ,  5.0f} , {}, {1.0f,1.0f}},//左上 → 左下
		{ { 5.0f ,  5.0f ,  5.0f} , {}, {1.0f,0.0f}},//右上 → 左上
		{ { 5.0f , -5.0f , -5.0f} , {}, {0.0f,1.0f}},//左下 → 右下
		{ { 5.0f ,  5.0f , -5.0f} , {}, {0.0f,0.0f}},//右下 → 右上
		//下
		{ { -5.0f ,  -5.0f , -5.0f} ,{}, {0.0f,1.0f}},//左下 
		{ { -5.0f ,  -5.0f ,  5.0f} ,{}, {0.0f,0.0f}},//左上 
		{ {  5.0f ,  -5.0f , -5.0f} ,{}, {1.0f,1.0f}},//右下 
		{ {  5.0f ,  -5.0f ,  5.0f} ,{}, {1.0f,0.0f}},//右上 

		//上
		{ { -5.0f ,  5.0f , -5.0f} ,{}, {0.0f,1.0f}},//左下 
		{ { -5.0f ,  5.0f ,  5.0f} ,{}, {0.0f,0.0f}},//左上 
		{ {  5.0f ,  5.0f , -5.0f} ,{}, {1.0f,1.0f}},//右下 
		{ {  5.0f ,  5.0f ,  5.0f} ,{}, {1.0f,0.0f}},//右上 
		
	};

	//インデックスデータ
	unsigned short indices[] =
	{
		//前
		0,1,2,//三角形1つ目
		2,1,3,//三角形2つ目

		//奥(前の面に4加算)
		5,4,6,//三角形3つ目
		5,6,7,//三角形4つ目

		//左
		8,9,10,//三角形5つ目
		10,9,11,//三角形6つ目
		
		//右
		13,12,14,//三角形7つ目
		13,14,15,//三角形8つ目
		
		//下
		16,17,18,//三角形9つ目
		18,17,19,//三角形10つ目

		//上
		20,22,21,//三角形11つ目
		21,22,23,//三角形12つ目
		

	};

	/*BYTE keys[256] = {};
	BYTE oldkeys[256] = {};*/
	//カメラアングル
	float angle = 0.0f;

	//座標
	XMFLOAT3 scale;
	XMFLOAT3 rotation;
	XMFLOAT3 position;
	scale = {1.0f,1.0f,1.0f};
	rotation = {0.0f,0.0f,0.0f};
	position = { 0.0f,0.0f,0.0f };

	//頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点の要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

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
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	result = dxCommon->GetDevice()->CreateCommittedResource(
		&heapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	for (int i = 0; i < _countof(indices) / 3; i++)
	{
	//三角形1つ毎に計算する
		//三角形のインデックスを取り出して、一時的な変数に入れる
		unsigned short indexZero = indices[i * 3 + 0];
		unsigned short indexOne  = indices[i * 3 + 1];
		unsigned short indexTwo  = indices[i * 3 + 2];

		//三角形を構成する頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&vertices[indexZero].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[indexOne].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[indexTwo].pos);

		//p0→p1、p0→p2ベクトルを計算(減算)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);

		//外積は両方から垂直なベクトル
		XMVECTOR normal = XMVector3Cross(v1,v2);

		//正規化(長さを1にする)
		normal = XMVector3Normalize(normal);

		//求めた法線を頂点データに代入
		XMStoreFloat3(&vertices[indexZero].normal, normal);
		XMStoreFloat3(&vertices[indexOne].normal, normal);
		XMStoreFloat3(&vertices[indexTwo].normal, normal);
	}

	//GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//全頂点に対して
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i];//座標をコピー
	}

	//繋がりを解除
	vertBuff->Unmap(0, nullptr);

	//頂点バッファービューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//GPU仮想アドレス
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//頂点バッファのサイズ
	vbView.SizeInBytes = sizeVB;
	//頂点1つ分のデータサイズ
	vbView.StrideInBytes = sizeof(vertices[0]);

	ComPtr<ID3DBlob> vsBlob = nullptr;//頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob = nullptr;//ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob = nullptr;//エラーオブジェクト

	//定数バッファ用GPUリソースポインタ
	ComPtr<ID3D12Resource> constBuffMaterial = nullptr;

	//ID3D12Resource* constBuffTransform0 = nullptr;
	////定数バッファのマッピング用ポインタ
	//ConstBufferDataTransform* constMapTransform0 = nullptr;

	//ID3D12Resource* constBuffTransform1 = nullptr;
	////定数バッファのマッピング用ポインタ
	//ConstBufferDataTransform* constMapTransform1 = nullptr;

	//3Dオブジェクトの数
	const size_t kObjectCount = 50;
	//3Dオブジェクトの配列
	Object3d object3ds[kObjectCount];


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
	result = dxCommon->GetDevice()->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial));
	assert(SUCCEEDED(result));

	for (int i = 0; i < _countof(object3ds); i++)
	{
		InitializeObject3d(&object3ds[i],dxCommon->GetDevice());

	//ここから↓は親子サンプル
		//先頭以外なら
		if (i > 0)
		{
			////1つ前のオブジェクトを親オブジェクトとする
			//object3ds[i].parent = &object3ds[i - 1];

			//親オブジェクトの9割の大きさ
			object3ds[i].scale = { 0.9f,0.9f,0.9f };
			//親オブジェクトに対してZ軸周りに30度回転
			object3ds[i].rotation = { 0.0f,0.0f,XMConvertToRadians(30.0f) };
			//親オブジェクトに対してZ方向に-8動かす
			object3ds[i].translation = { 0.0f,0.0f,-8.0f };
		}
	}

	////定数バッファの生成(3D)
	//result = dev->CreateCommittedResource(
	//	&cbHeapProp,//ヒープ設定
	//	D3D12_HEAP_FLAG_NONE,
	//	&cbResourceDesc,//リソース設定
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&constBuffTransform0));
	//assert(SUCCEEDED(result));

	////定数バッファの生成(3D)
	//result = dev->CreateCommittedResource(
	//	&cbHeapProp,//ヒープ設定
	//	D3D12_HEAP_FLAG_NONE,
	//	&cbResourceDesc,//リソース設定
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&constBuffTransform1));
	//assert(SUCCEEDED(result));


	////定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);
	assert(SUCCEEDED(result));

	//result = constBuffTransform0->Map(0, nullptr, (void**)&constMapTransform0);
	//assert(SUCCEEDED(result));

	//result = constBuffTransform1->Map(0, nullptr, (void**)&constMapTransform1);
	//assert(SUCCEEDED(result));

	//値を書き込むと自動的に転送される
	constMapMaterial->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	////単位行列を代入
	//constMapTransform0->mat = XMMatrixIdentity();

	//透視投影行列の計算	
	XMMATRIX matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),//上下画角45度
		(float)WinApp::window_width / WinApp::window_height,//アスペクト比
		0.1f, 1000.0f//前端、奥端
		);

	//ビュー変換行列
	XMMATRIX matView;

	XMFLOAT3 eye(0, 0, -150);//視点座標
	XMFLOAT3 target(0,0,0);//注視点座標
	XMFLOAT3 up(0,1,0);//上方向ベクトル
	//ビュー変換行列の計算
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));


	//インデックスデータ全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t)) * _countof(indices);

	//リソース設定
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//インデックスバッファの生成
	ComPtr<ID3D12Resource> indexBuff = nullptr;
	result = dxCommon->GetDevice()->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	//インデックスバッファをマッピング
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	//全インデックスに対して
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i];
	}
	//マッピング解除
	indexBuff->Unmap(0, nullptr);

	//インデックスバッファビューの生成
	D3D12_INDEX_BUFFER_VIEW ibView{};
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	//テクスチャ
	TexMetadata metaData{};
	ScratchImage scratchImg{};

	result = LoadFromWICFile(
		L"Resource/mario.jpg",
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
	ComPtr<ID3D12Resource> textureBuff = nullptr;
	result = dxCommon->GetDevice()->CreateCommittedResource(
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

	//設定を元にSRV用デスクリプタヒープを生成
	ComPtr<ID3D12DescriptorHeap> srvHeap = nullptr;
	result = dxCommon->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
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
	dxCommon->GetDevice()->CreateShaderResourceView(textureBuff.Get(), &srvDesc, srvHandle);
	//テクスチャ(2枚目)
	TexMetadata metaData2{};
	ScratchImage scratchImg2{};

	result = LoadFromWICFile(
		L"Resource/kuribo-.jpg",
		WIC_FLAGS_NONE,
		&metaData2, scratchImg2
	);

	/*ScratchImage mipChain{};*/
	result = GenerateMipMaps(
		scratchImg2.GetImages(), scratchImg2.GetImageCount(), scratchImg2.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain
	);
	if (SUCCEEDED(result))
	{
		scratchImg2 = std::move(mipChain);
		metaData2 = scratchImg2.GetMetadata();
	}

	metaData2.format = MakeSRGB(metaData2.format);

	//リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc2{};
	textureResourceDesc2.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc2.Format = metaData2.format;
	textureResourceDesc2.Width = metaData2.width;
	textureResourceDesc2.Height = (UINT)metaData2.height;
	textureResourceDesc2.DepthOrArraySize = (UINT16)metaData2.arraySize;
	textureResourceDesc2.MipLevels = (UINT16)metaData2.mipLevels;
	textureResourceDesc2.SampleDesc.Count = 1;

	//テクスチャバッファを生成
	ComPtr<ID3D12Resource> textureBuff2 = nullptr;
	result = dxCommon->GetDevice()->CreateCommittedResource(
		&textureHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc2,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureBuff2));

	for (size_t i = 0; i < metaData2.mipLevels; i++)
	{
		//ミップマップレベルを指定してイメージを取得
		const Image* img2 = scratchImg2.GetImage(i, 0, 0);
		//テクスチャバッファにデータ転送
		result = textureBuff2->WriteToSubresource(
			(UINT)i,//
			nullptr,//全領域へコピー
			img2->pixels,//元データアドレス
			(UINT)img2->rowPitch,//1ラインサイズ
			(UINT)img2->slicePitch//全サイズ
		);
		assert(SUCCEEDED(result));
	}

	//
	UINT incrementSize = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvHandle.ptr += incrementSize;

	//シェーダリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = textureResourceDesc2.Format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc2.Texture2D.MipLevels = textureResourceDesc2.MipLevels;

	//ハンドルのさす位置にシェーダリソースビュー作成
	dxCommon->GetDevice()->CreateShaderResourceView(textureBuff2.Get(), &srvDesc2, srvHandle);

	//デスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメータ
	D3D12_ROOT_PARAMETER rootParams[3] = {};
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams[0].Descriptor.ShaderRegister = 0;//定数バッファ番号
	rootParams[0].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える

	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//種類
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;//デスクリプタレンジ
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;//デスクリプタレンジ数
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える

	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams[2].Descriptor.ShaderRegister = 1;//定数バッファ番号
	rootParams[2].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える

	//テクスチャサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//横繰り返し
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//縦繰り返し
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//奥行繰り返し
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;//全てリニア補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;//ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;//ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダからのみ使用可能


	//頂点シェーダの読み込みとコンパイル(頂点シェーダは頂点の座標変換)
	result = D3DCompileFromFile(
		L"Resource/shaders/BasicVS.hlsl",//シェーダーファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルードを可能にする
		"main",//エントリーポイント
		"vs_5_0",//シェーダモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバック用設定
		0,
		&vsBlob, &errorBlob);

	////コンパイルエラーなら
	//if (FAILED(result))
	//{
	//	//errorBlobからエラー内容をstirng型にコピー
	//	std::string error;
	//	error.resize(errorBlob->GetBufferSize());

	//	std::copy_n((char*)errorBlob->GetBufferPointer(),
	//		errorBlob->GetBufferPointer(),
	//		error.begin());
	//	error += "\n";
	//	//エラー内容を出力ウィンドウに表示
	//	OutputDebugStringA(error.c_str());
	//	assert(0);
	//}

	//ピクセルシェーダの読み込みとコンパイル(ピクセルの役割は描画色の設定)
	result = D3DCompileFromFile(
		L"Resource/shaders/BasicPS.hlsl",//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob);

	////コンパイルエラーなら
	//if (FAILED(result))
	//{
	//	//errorBlobからエラー内容をstirng型にコピー
	//	std::string error;
	//	error.resize(errorBlob->GetBufferSize());

	//	std::copy_n((char*)errorBlob->GetBufferPointer(),
	//		errorBlob->GetBufferPointer(),
	//		error.begin());
	//	error += "\n";
	//	//エラー内容を出力ウィンドウに表示
	//	OutputDebugStringA(error.c_str());
	//	assert(0);
	//}


	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{
			//xyz座標
			"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0,
		},

		{
			//法線ベクトル
			"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0,
		},

		{
			//uv座標
			"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0,
		},
	};

	//グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	//シェーダの設定
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	//デプスステンシルステートの設定
	pipelineDesc.DepthStencilState.DepthEnable = true;//深度テストを行う
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//書き込み許可
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;//小さければ合格
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット

	//サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//標準設定

	//ラスタライザの設定(頂点のピクセル化)
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//カリングしない
	//pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;//背面をカリング
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//ポリゴン内塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true;//深度クリッピングを有効に

	//ブレンドステート
	/*pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;*///RGBA全てのチャンネルを描画
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	////共通設定
	//blenddesc.BlendEnable    = true;//ブレンドを有効にする
	//blenddesc.BlendOpAlpha   = D3D12_BLEND_OP_ADD;//加算
	//blenddesc.SrcBlendAlpha  = D3D12_BLEND_ONE;//ソースの値を100%使う
	//blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;//ソースの値を0%使う

	////	加算合成
	//blenddesc.BlendOp   = D3D12_BLEND_OP_ADD;//加算
	//blenddesc.SrcBlend  = D3D12_BLEND_ONE;//ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;//ソースの値を100%使う

	////減算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;//テストからソースを減算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;//ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;//ソースの値を100%使う

	////色反転
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//加算
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1.0f-デストカラーの値
	//blenddesc.DestBlend = D3D12_BLEND_ZERO;//使わない

	////半透明
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//加算
	//blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースのアルファ値
	//blenddesc.DestBlend = D3D12_BLEND_SRC_ALPHA;//1.0f-ソースのアルファ値

	//頂点レイアウトの設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	//図形の形状設定
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//その他の設定
	pipelineDesc.NumRenderTargets = 1;//描画対象は1つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0~255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1;//1ピクセルにつき1回レンダリング

	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature;

	//ルートシグネチャの設定(生成)
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;//ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams);//ルートパラメータ数

	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	//ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = dxCommon->GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));

	//パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature.Get();

	//パイプラインステートの生成
	ComPtr<ID3D12PipelineState> pipelineState = nullptr;
	result = dxCommon->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));

	//ここまで描画初期化処理

	//FLOAT clearColor[] = { 0.1f, 0.25f, 0.5f, 0.0f };//青っぽい色

	//ゲームループ
	while (true)
	{

		fps->FpsControlBegin();
		if (winApp->ProcessMessage())
		{
			//ゲームループを抜ける
			break;
		}
		//ここからDirectX毎フレーム処理
		////キーボード情報の取得開始
		
	

		//ビュー変換行列の計算
		matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
		
		//keyboard->Acquire();

		////全キーの入力状態を取得する
		//keyboard->GetDeviceState(sizeof(keys), keys);

		//if (keys[DIK_D] || keys[DIK_A]||keys[DIK_R])
		//{
		//	if (keys[DIK_D])
		//	{
		//		angle += XMConvertToRadians(1.0f);
		//	}
		//	else if (keys[DIK_A])
		//	{
		//		angle -= XMConvertToRadians(1.0f);
		//	}
		//	else if (keys[DIK_R])
		//	{
		//		angle = 0.0f;
		//	}
		//	//アングルラジアンだけY軸周りに回転。半径は-150
		//	eye.x = -150 * sinf(angle);
		//	eye.z = -150 * cosf(angle);

		//	//ビュー変換行列の計算
		//	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
		//};
		//
		////
		//if (keys[DIK_UP] || keys[DIK_DOWN] || keys[DIK_LEFT] || keys[DIK_RIGHT] )
		//{
		//	if (keys[DIK_UP])
		//	{
		//		object3ds[0].translation.y += 0.5f;
		//	}
		//	if (keys[DIK_DOWN])
		//	{
		//		object3ds[0].translation.y -= 0.5f;
		//	}
		//	if (keys[DIK_LEFT])
		//	{
		//		object3ds[0].translation.x -= 0.5f;
		//	}
		//	if (keys[DIK_RIGHT])
		//	{
		//		object3ds[0].translation.x += 0.5f;
		//	}
		//	
		//};

		input->Update();

		if (input->PushKey(DIK_D))
		{
			angle += XMConvertToRadians(1.0f);
		}

		if (input->TriggerKey(DIK_SPACE))
		{
			OutputDebugStringA("Hit 0\n");
		}

		if (input->ReleaseKey(DIK_RETURN))
		{
			OutputDebugStringA("Hit 0\n");
		}


		for (int i = 0; i < _countof(object3ds); i++)
		{
			UpdateObject3d(&object3ds[i], matView, matProjection);
		}

		//描画前処理
		dxCommon->preDraw();

		//パイプラインステートとルートシグネチャの設定コマンド

		dxCommon->GetCommandList()->SetPipelineState(pipelineState.Get());


		dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

		//プリミティブ形状の設定コマンド
		/*commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);*/

		dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//頂点バッファービューの設定コマンド
		dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vbView);

		//定数バッファビュー(CBV)の設定コマンド
		dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

		//SRVヒープ設定コマンド
		dxCommon->GetCommandList()->SetDescriptorHeaps(1, srvHeap.GetAddressOf());

		//SRVヒープの先頭ハンドルを取得
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();

		////SRVヒープの先頭にあるSRVをルートパラメータの1番に設定
		//commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

		srvGpuHandle.ptr += incrementSize;
		//SRVヒープの先頭にあるSRVをルートパラメータの1番に設定
		dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

		//インデックスバッファビューの設定コマンド
		dxCommon->GetCommandList()->IASetIndexBuffer(&ibView);
		for (int i = 0; i < _countof(object3ds); i++)
		{
			DrawObject3d(&object3ds[i], dxCommon->GetCommandList(), vbView, ibView, _countof(indices));
		}

		//　４．ここまで描画コマンド
		dxCommon->postDraw();
		


		fps->FpsControlEnd();
		//ここまでDirectX毎フレーム処理
	}
	//windowAPIの終了処理
	winApp->Finalize();
	//入力解放
	delete input;
	delete winApp;
	winApp = nullptr;
	delete dxCommon;
	return 0;
}

LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して,アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void keyInitialize(BYTE* key, BYTE* oldkey, int array)
{
	for (int i = 0; i < array; i++)
	{
		oldkey[i] = key[i];
	}
};
bool pushKey(BYTE* key, int keyNum)
{
	if (key[keyNum])
	{
		return true;
	}
	else
	{
		return false;
	}
};

bool notPushKey(BYTE* key, int keyNum)
{
	if (!key[keyNum])
	{
		return true;
	}
	else
	{
		return false;
	}

};

bool pressKey(BYTE* key, BYTE* oldkey, int keyNum)
{
	if (key[keyNum] && oldkey[keyNum])
	{
		return true;
	}
	else
	{
		return false;
	}
};

bool triggerKey(BYTE* key, BYTE* oldkey, int keyNum)
{
	if (key[keyNum] && !oldkey[keyNum])
	{
		return true;
	}
	else
	{
		return false;
	}
};

bool releaseKey(BYTE* key, BYTE* oldkey, int keyNum)
{
	if (!key[keyNum] && oldkey[keyNum])
	{
		return true;
	}
	else
	{
		return false;
	}
};

void InitializeObject3d(Object3d*object, ID3D12Device* dev)
{

	HRESULT result;

	//定数バッファのヒープ設定
	//ヒープ設定
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	//リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファの生成
	result = dev->CreateCommittedResource(
		&heapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&object->constBuffTransform));
	assert(SUCCEEDED(result));

	////定数バッファのマッピング
	result = object->constBuffTransform->Map(0, nullptr, (void**)&object->constMapTransform);
	assert(SUCCEEDED(result));
}

void UpdateObject3d(Object3d* object, XMMATRIX& matview, XMMATRIX& matProjection)
{
	XMMATRIX matScale, matRot, matTrans;

	//拡大、回転、移動行列の計算
	matScale = XMMatrixScaling(object->scale.x, object->scale.y, object->scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(object->rotation.z);
	matRot *= XMMatrixRotationX(object->rotation.x);
	matRot *= XMMatrixRotationY(object->rotation.y);
	matTrans = XMMatrixTranslation(object->translation.x, object->translation.y, object->translation.z);

	//ワールド行列の合成
	object->matworld = XMMatrixIdentity();
	object->matworld *= matScale;
	object->matworld *= matRot;
	object->matworld *= matTrans;

	//親オブジェクトがあれば
	if (object->parent != nullptr)
	{
		//親オブジェクトのワールド行列を掛ける
		object->matworld *= object->parent->matworld;
	}

	//定数バッファにデータ転送
	object->constMapTransform->mat = object->matworld * matview * matProjection;
}
void DrawObject3d(Object3d* object, ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView, D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices)
{
	//頂点バッファービューの設定コマンド
	commandList->IASetVertexBuffers(0, 1, &vbView);

	//インデックスバッファビューの設定コマンド
	commandList->IASetIndexBuffer(&ibView);

	//定数バッファビュー(CBV)の設定コマンド
	commandList->SetGraphicsRootConstantBufferView(2, object->constBuffTransform->GetGPUVirtualAddress());

	//描画コマンド
	commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);//全ての頂点を使って描画
}