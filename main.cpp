#include <Windows.h>
#include <tchar.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#define DIRECTINPUT_VERSION  0x0800
#include <dinput.h>
#include <wrl.h>

#include <DirectXTex.h> 

using namespace DirectX;
using namespace Microsoft::WRL;

//リンクの設定
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")//directInputのバージョン
#pragma comment(lib,"dxguid.lib")

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
	ID3D12Resource* constBuffTransform;
	//定数バッファマップ(行列用)
	ConstBufferDataTransform* constMapTransform;
	//アフィン変換情報
	XMFLOAT3 scale = { 1,1,1 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 translation = { 0,0,0 };
	//ワールド変換行列
	XMMATRIX matworld;

	Object3d* parent = nullptr;
};

//オブジェクト初期化処理
void InitializeObject3d(Object3d* object, ID3D12Device* dev);
//オブジェクト更新処理
void UpdateObject3d(Object3d* object, XMMATRIX& matview, XMMATRIX& matProjection);
//オブジェクト描画処理
void DrawObject3d(Object3d* object, ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView, D3D12_INDEX_BUFFER_VIEW& ibView,UINT numIndices);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	OutputDebugStringA("Hello DirectX!!\n");

	const int window_width = 1280;
	const int window_height = 720;

	WNDCLASSEX w = {};

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;//コールバック関数の指定
	w.lpszClassName = _T("DirectXTest");//アプリケーションクラス名(適当でいいです)
	w.hInstance = GetModuleHandle(0);//ハンドルの取得
	w.hCursor = LoadCursor(NULL, IDC_ARROW);


	RegisterClassEx(&w);//アプリケーションクラス(こういうの作るからよろしくってOSに予告する)

	RECT wrc = { 0,0,window_width, window_height };//{0,0,window_width,window_height}

	//関数を使ってウィンドウのサイズを補正する
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウオブジェクトの生成
	HWND hwnd = CreateWindow(w.lpszClassName,//クラス名指定
		_T("DX12Test"),//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,//タイトルバーと境界線があるウィンドウ
		CW_USEDEFAULT,//表示x座標はOSにお任せ
		CW_USEDEFAULT,//表示y座標はOSにお任せ
		wrc.right - wrc.left,//ウィンドウ幅
		wrc.bottom - wrc.top,//ウィンドウ高
		nullptr,//親ウィンドウハンドル
		nullptr,//メニューハンドル
		w.hInstance,//呼び出しアプリケーションハンドル
		nullptr);//追加パラメーター

	ShowWindow(hwnd, SW_SHOW);

	MSG msg{};

	//ここからDirectX初期化処理
#ifdef _DEBUG
	//デバックレイヤーをオンに
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif

	HRESULT result;
	//受け皿となる変数
	ID3D12Device* dev = nullptr;
	IDXGIFactory7* dxgiFactory = nullptr;
	IDXGISwapChain4* swapChain = nullptr;
	ID3D12CommandAllocator* cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12DescriptorHeap* rtvHeap = nullptr;


	//DXGIファクトリーの生成
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(result));

	//アダプターの列挙用
	std::vector<IDXGIAdapter4*> adapters;


	//ここに特定の名前を持つアダプターオブジェクトが入る
	IDXGIAdapter4* tmpAdapter ;

	//パフォーマンスの高い順から、全てのアダプターを列挙する
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
		IID_PPV_ARGS(&tmpAdapter))
		!= DXGI_ERROR_NOT_FOUND; i++)
	{
		//動的配列に追加する
		adapters.push_back(tmpAdapter);
	}

	//妥当なアダプターを選別する
	for (size_t i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC3 adapterDesc;
		//アダプターの情報を取得する
		adapters[i]->GetDesc3(&adapterDesc);

		//ソフトウェアデバイスを回避
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			//デバイスを採用してループを抜ける
			tmpAdapter = adapters[i];
			break;
		}
	}


	//対応レベルの取得
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;
	for (size_t i = 0; i < _countof(levels); i++)
	{
		//採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(tmpAdapter, levels[i], IID_PPV_ARGS(&dev));
		if (result == S_OK)
		{
			//デバイスを生成した時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}

	//コマンドアロケータを生成
	result = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator));
	assert(SUCCEEDED(result));

	//コマンドリストを生成
	result = dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator, nullptr, IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(result));

	//コマンドキューの設定
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	//コマンドキューを生成
	result = dev->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	assert(SUCCEEDED(result));

	//スワップチェーンの設定
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = window_width;
	swapChainDesc.Height = window_height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//色情報の書式
	swapChainDesc.SampleDesc.Count = 1;//マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;//バックバッファ用
	swapChainDesc.BufferCount = 2;//バッファ数を2つに設定
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//フリップ後は破棄
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;//
	//スワップチェーンの生成
	result = dxgiFactory->CreateSwapChainForHwnd(commandQueue, hwnd, &swapChainDesc, nullptr, nullptr, (IDXGISwapChain1**)&swapChain);
	assert(SUCCEEDED(result));

	//デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;//レンダーターゲットビュー
	rtvHeapDesc.NumDescriptors = swapChainDesc.BufferCount;//裏表の2つ

	//デスクリプタヒープの生成
	dev->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));//先にデスクリプタヒープを作る

	//バックバッファ
	std::vector<ID3D12Resource*>backBuffers;

	backBuffers.resize(swapChainDesc.BufferCount);//スワップチェーン内に生成されたバックバッファのアドレスを入れておくためのポインタを用意する

	//スワップチェーンの全てのバッファについて処理する
	for (size_t i = 0; i < backBuffers.size(); i++)
	{
		//スワップチェーンからバッファを取得
		swapChain->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers[i]));
		//デスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		//表か裏かでアドレスがずれる
		rtvHandle.ptr += i * dev->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		//レンダーターゲットビューの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		//シェーダーの計算結果をSRGBに変換して書き込む
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		//レンダーターゲットビューの生成
		dev->CreateRenderTargetView(backBuffers[i], &rtvDesc, rtvHandle);
	}

	//フェンスの生成
	ID3D12Fence* fence = nullptr;
	UINT64 fenceVal = 0;

	result = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	//DirectInputの初期化
	IDirectInput8* directInput = nullptr;
	result = DirectInput8Create(w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	//キーボードデバイスの生成
	IDirectInputDevice8* keyboard = nullptr;
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
	//DISCL_FOREGROUND   画面が手前にある場合のみ入力を受け付ける
	//DISCL_NONEXCLUSIVE デバイスをこのアプリだけで専有しない
	//DISCL_NOWINKEY     Windowsキーを無効にする



	//ここまでDirectX初期化処理

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

	BYTE keys[256] = {};
	BYTE oldkeys[256] = {};
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
	ID3D12Resource* vertBuff = nullptr;
	result = dev->CreateCommittedResource(
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

	ID3DBlob* vsBlob = nullptr;//頂点シェーダオブジェクト
	ID3DBlob* psBlob = nullptr;//ピクセルシェーダオブジェクト
	ID3DBlob* errorBlob = nullptr;//エラーオブジェクト

	

	//定数バッファ用GPUリソースポインタ
	ID3D12Resource* constBuffMaterial = nullptr;

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
	result = dev->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial));
	assert(SUCCEEDED(result));

	for (int i = 0; i < _countof(object3ds); i++)
	{
		InitializeObject3d(&object3ds[i],dev);

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
		(float)window_width / window_height,//アスペクト比
		0.1f, 1000.0f//前端、奥端
		);

	//ビュー変換行列
	XMMATRIX matView;

	XMFLOAT3 eye(0, 0, -150);//視点座標
	XMFLOAT3 target(0,0,0);//注視点座標
	XMFLOAT3 up(0,1,0);//上方向ベクトル
	//ビュー変換行列の計算
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	////ワールド変換行列
	//XMMATRIX matWorld0;
	//XMMATRIX matWorld1;
	////スケーリング行列
	//XMMATRIX matScale0;
	//XMMATRIX matScale1;
	////回転行列
	//XMMATRIX matRot0;
	//XMMATRIX matRot1;
	////平行移動行列
	//XMMATRIX matTrans0;
	//XMMATRIX matTrans1;


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
	ID3D12Resource* indexBuff = nullptr;
	result = dev->CreateCommittedResource(
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
	ID3D12Resource* textureBuff = nullptr;
	result = dev->CreateCommittedResource(
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
	ID3D12DescriptorHeap* srvHeap = nullptr;
	result = dev->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
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
	dev->CreateShaderResourceView(textureBuff, &srvDesc, srvHandle);
	//テクスチャ(2枚目)
	TexMetadata metaData2{};
	ScratchImage scratchImg2{};

	result = LoadFromWICFile(
		L"Resource/kuribo-.jpg",
		WIC_FLAGS_NONE,
		&metaData2, scratchImg2
	);

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
	ID3D12Resource* textureBuff2 = nullptr;
	result = dev->CreateCommittedResource(
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
	UINT incrementSize = dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvHandle.ptr += incrementSize;

	//シェーダリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = textureResourceDesc2.Format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc2.Texture2D.MipLevels = textureResourceDesc2.MipLevels;

	//ハンドルのさす位置にシェーダリソースビュー作成
	dev->CreateShaderResourceView(textureBuff2, &srvDesc2, srvHandle);

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


	//深度バッファのリソース設定
	//リソース設定
	D3D12_RESOURCE_DESC depthResourceDesc{};
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Width = window_width;//レンダーターゲットに合わせる
	depthResourceDesc.Height = window_height;//レンダーターゲットに合わせる
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット
	depthResourceDesc.SampleDesc.Count = 1;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//デプスステンシル

	//深度値用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;//深度値1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット

	//深度バッファの生成
	ID3D12Resource* depthBuff = nullptr;
	result = dev->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度値書き込みに使用
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff));

	//深度ビュー用デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	ID3D12DescriptorHeap* dsvHeap = nullptr;
	result = dev->CreateDescriptorHeap(&dsvHeapDesc,IID_PPV_ARGS(&dsvHeap));

	//深度ステンシルビューの生成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dev->CreateDepthStencilView(
		depthBuff,
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart());


	//頂点シェーダの読み込みとコンパイル(頂点シェーダは頂点の座標変換)
	result = D3DCompileFromFile(
		L"BasicVS.hlsl",//シェーダーファイル名
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
		L"BasicPS.hlsl",//シェーダファイル名
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
	//pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//カリングしない
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;//背面をカリング
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
	ID3D12RootSignature* rootSignature;

	//ルートシグネチャの設定(生成)
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;//ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams);//ルートパラメータ数

	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	//ルートシグネチャのシリアライズ
	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();

	//パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature;

	//パイプラインステートの生成
	ID3D12PipelineState* pipelineState = nullptr;
	result = dev->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));



	//ここまで描画初期化処理

	FLOAT clearColor[] = { 0.1f, 0.25f, 0.5f, 0.0f };//青っぽい色

	//ゲームループ
	while (true)
	{
		//ウィンドウメッセージ処理
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}

		if (msg.message == WM_QUIT)
		{
			break;
		}

		//ここからDirectX毎フレーム処理
		//キーボード情報の取得開始
		keyboard->Acquire();

		//全キーの入力状態を取得する
		keyboard->GetDeviceState(sizeof(keys), keys);

		if (keys[DIK_D] || keys[DIK_A]||keys[DIK_R])
		{
			if (keys[DIK_D])
			{
				angle += XMConvertToRadians(1.0f);
			}
			else if (keys[DIK_A])
			{
				angle -= XMConvertToRadians(1.0f);
			}
			else if (keys[DIK_R])
			{
				angle = 0.0f;
			}
			//アングルラジアンだけY軸周りに回転。半径は-150
			eye.x = -150 * sinf(angle);
			eye.z = -150 * cosf(angle);

			//ビュー変換行列の計算
			matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
		};
		
		//
		if (keys[DIK_UP] || keys[DIK_DOWN] || keys[DIK_LEFT] || keys[DIK_RIGHT] )
		{
			if (keys[DIK_UP])
			{
				object3ds[0].translation.y += 0.5f;
			}
			if (keys[DIK_DOWN])
			{
				object3ds[0].translation.y -= 0.5f;
			}
			if (keys[DIK_LEFT])
			{
				object3ds[0].translation.x -= 0.5f;
			}
			if (keys[DIK_RIGHT])
			{
				object3ds[0].translation.x += 0.5f;
			}
			
		};

		//
		//matScale0 = XMMatrixScaling(scale.x, scale.y, scale.z);
	
		////単位行列を代入
		//matRot0 = XMMatrixIdentity();

		//matRot0 = XMMatrixRotationZ(rotation.z);
		//matRot0 = XMMatrixRotationX(rotation.x);
		//matRot0 = XMMatrixRotationY(rotation.y);

		//matTrans0 = XMMatrixTranslation(position.x, position.y, position.z);
		//
		////単位行列を代入
		//matWorld0 = XMMatrixIdentity();
		////行列に計算
		//matWorld0*= matScale0;
		//matWorld0 *= matRot0;
		//matWorld0 *= matTrans0;

		//matScale1 = XMMatrixScaling(scale.x, scale.y, scale.z);

		////単位行列を代入
		//matRot1 = XMMatrixIdentity();

		//matRot1 = XMMatrixRotationZ(rotation.z);
		//matRot1 = XMMatrixRotationX(rotation.x);
		//matRot1 = XMMatrixRotationY(XM_PI / 4.0f);

		//matTrans1 = XMMatrixTranslation(-20.0f, 0.0f, 0.0f);

		////単位行列を代入
		//matWorld1 = XMMatrixIdentity();
		////行列に計算
		//matWorld1 *= matScale1;
		//matWorld1 *= matRot1;
		//matWorld1 *= matTrans1;

		////定数バッファに転送
		//constMapTransform0->mat = matWorld0 * matView * matProjection;
		//constMapTransform1->mat = matWorld1 * matView * matProjection;

		for (int i = 0; i < _countof(object3ds); i++)
		{
			UpdateObject3d(&object3ds[i], matView, matProjection);
		}


		//バックバッファの番号を解除
		UINT bbIndex = swapChain->GetCurrentBackBufferIndex();

		// １．リソースバリアで書き込み可能に変更
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = backBuffers[bbIndex];//バックバッファを指定
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;//表示状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;//描画状態へ
		commandList->ResourceBarrier(1, &barrierDesc);

		//　２．描画先の変更
		//レンダーターゲットビューのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * dev->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		
		//深度ステンシル用デスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();

		commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

		// ３．画面クリア          R     G     B     A(alpha)

		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		commandList->ClearDepthStencilView(dsvHandle,D3D12_CLEAR_FLAG_DEPTH,1.0f,0,0,nullptr);

		//　４．ここから描画コマンド
		//ビューポートの設定コマンド
		D3D12_VIEWPORT viewport[4]{};

		viewport[0].Width = window_width;
		viewport[0].Height = window_height;
		viewport[0].TopLeftX = 0;
		viewport[0].TopLeftY = 0;
		viewport[0].MinDepth = 0.0f;
		viewport[0].MaxDepth = 1.0f;

		/*viewport[1].Width = window_width;
		viewport[1].Height = window_height;
		viewport[1].TopLeftX = -300;
		viewport[1].TopLeftY = 200;
		viewport[1].MinDepth = 0.0f;
		viewport[1].MaxDepth = 1.0f;
		viewport[2].Width = window_width;
		viewport[2].Height = window_height;
		viewport[2].TopLeftX = 200;
		viewport[2].TopLeftY = -100;
		viewport[2].MinDepth = 0.0f;
		viewport[2].MaxDepth = 1.0f;
		viewport[3].Width = window_width;
		viewport[3].Height = window_height / 2;
		viewport[3].TopLeftX = 200;
		viewport[3].TopLeftY = 400;
		viewport[3].MinDepth = 0.0f;
		viewport[3].MaxDepth = 1.0f;*/


		//ビューポート設定コマンドを、コマンドリストに積む
		commandList->RSSetViewports(1, &viewport[0]);

		//シザー矩形
		D3D12_RECT scissorRect{};
		scissorRect.left = 0;//切り抜き座標左
		scissorRect.right = scissorRect.left + window_width;//切り抜き座標右
		scissorRect.top = 0;//切り抜き座標上
		scissorRect.bottom = scissorRect.top + window_height;//切り抜き座標下

		//シザー矩形設定コマンドを、コマンドリストに積む
		commandList->RSSetScissorRects(1, &scissorRect);

		//パイプラインステートとルートシグネチャの設定コマンド

		commandList->SetPipelineState(pipelineState);


		commandList->SetGraphicsRootSignature(rootSignature);

		//プリミティブ形状の設定コマンド
		/*commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);*/

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//頂点バッファービューの設定コマンド
		commandList->IASetVertexBuffers(0, 1, &vbView);

		//定数バッファビュー(CBV)の設定コマンド
		commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

		//SRVヒープ設定コマンド
		commandList->SetDescriptorHeaps(1, &srvHeap);

		//SRVヒープの先頭ハンドルを取得
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();

		//SRVヒープの先頭にあるSRVをルートパラメータの1番に設定
		
		if (keys[DIK_SPACE])
		{
			srvGpuHandle.ptr += incrementSize;
		}
		
		//SRVヒープの先頭にあるSRVをルートパラメータの1番に設定
		commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

		//インデックスバッファビューの設定コマンド
		commandList->IASetIndexBuffer(&ibView);
		for (int i = 0; i < _countof(object3ds); i++)
		{
			DrawObject3d(&object3ds[i], commandList, vbView,ibView,_countof(indices));
		}

		////0番定数バッファビュー(CBV)の設定コマンド
		//commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform0->GetGPUVirtualAddress());
		////0番描画コマンド
		//commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);//全ての頂点を使って描画
		////1番定数バッファビュー(CBV)の設定コマンド
		//commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform1->GetGPUVirtualAddress());
		////1番描画コマンド
		//commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);//全ての頂点を使って描画

		//ビューポート設定コマンドを、コマンドリストに積む
		//commandList->RSSetViewports(1, &viewport[1]);
		//commandList->DrawInstanced(_countof(vertices), 1, 0, 0);//全ての頂点を使って描画

		//commandList->RSSetViewports(1, &viewport[2]);
		//commandList->DrawInstanced(_countof(vertices), 1, 0, 0);//全ての頂点を使って描画

		//commandList->RSSetViewports(1, &viewport[3]);
		//commandList->DrawInstanced(_countof(vertices), 1, 0, 0);//全ての頂点を使って描画
		//　４．ここまで描画コマンド

		//　５．リソースバリアを戻す
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;//描画状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;//表示状態へ
		commandList->ResourceBarrier(1, &barrierDesc);

		//命令のクローズ
		result = commandList->Close();
		assert(SUCCEEDED(result));

		//コマンドリストの実行
		ID3D12CommandList* commandLists[] = { commandList };
		commandQueue->ExecuteCommandLists(1, commandLists);

		//画面に表示するバッファをフリップ (裏表の入れ替え)
		result = swapChain->Present(1, 0);
		assert(SUCCEEDED(result));

		//コマンドの実行完了を待つ
		commandQueue->Signal(fence, ++fenceVal);
		if (fence->GetCompletedValue() != fenceVal)
		{
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			fence->SetEventOnCompletion(fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}

		//キューをクリア
		result = cmdAllocator->Reset();
		assert(SUCCEEDED(result));
		//再びコマンドリストをためる準備
		result = commandList->Reset(cmdAllocator, nullptr);
		assert(SUCCEEDED(result));



		//ここまでDirectX毎フレーム処理
	}
	UnregisterClass(w.lpszClassName, w.hInstance);


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