#pragma once
#include "../Core/DirectXCommon.h"
class SpriteManager
{
private:
	//定数バッファ用データ構造体
	struct ConstBufferDataMaterial
	{
		XMFLOAT4 color;//色
	};
	//定数バッファ用データ構造体(3D変換行列)
	struct ConstBufferDataTransform
	{
		XMMATRIX mat;//3D変換行列
	};
	//頂点データ構造体
	struct Vertex
	{
		XMFLOAT3 pos;//xyz座標
		XMFLOAT2 uv;//uv座標
	};

	static const int vertexCount = 4;

	//頂点データ
	std::array<Vertex, vertexCount> vertices;

	//横方向ピクセル数
	const size_t textureWidth = 256;
	//縦方向ピクセル数
	const size_t textureHeight = 256;
	//配列の要素数
	const size_t imageDataCount = textureWidth * textureHeight;
	//画像イメージデータ配列
	XMFLOAT4* imageData = new XMFLOAT4[imageDataCount];

	//頂点バッファービューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	//定数バッファの生成
	ID3D12Resource* constBuffMaterial = nullptr;

	ID3D12Resource* constBuffTransform = nullptr;
	//定数バッファのマッピング用ポインタ
	ConstBufferDataTransform* constMapTransform = nullptr;

	//設定を元にSRV用デスクリプタヒープを生成
	ID3D12DescriptorHeap* srvHeap = nullptr;

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature;
	//パイプラインステート
	ID3D12PipelineState* pipelineState = nullptr;

	//SRVヒープの先頭ハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;

	//スプライト用コマンドリスト
	ID3D12GraphicsCommandList* sCommandList;

	DirectXCommon* dxCommon_ = nullptr;

	//射影変換行列
	XMMATRIX matProjection;
	//ワールド変換行列
	XMMATRIX matWorld;
	//回転行列
	XMMATRIX matRot;
	//平行移動行列
	XMMATRIX matTrans;
	float rota;
	XMFLOAT3 position;

public:

	uint32_t Load(const std::string& fileName);

	static SpriteManager* GetInstance();

	void Initialize(DirectXCommon* dxCommon);

	void Update();

	void Draw();
};