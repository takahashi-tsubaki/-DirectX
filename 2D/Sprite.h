#pragma once
#include "SpriteManager.h"
class Sprite
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

	ID3D12GraphicsCommandList* sCommandList_ = nullptr;

	SpriteManager* spManager_ = nullptr;
	
	//頂点バッファービューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	//設定を元にSRV用デスクリプタヒープを生成
	ID3D12DescriptorHeap* srvHeap = nullptr;
	//SRVヒープの先頭ハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;

	//射影変換行列
	XMMATRIX matProjection;
	//ワールド変換行列
	XMMATRIX matWorld;
	//回転行列
	XMMATRIX matRot;
	//平行移動行列
	XMMATRIX matTrans;
	DirectX::XMFLOAT2 rotation = { 0.0f,0.0f};
	DirectX::XMFLOAT2 position_ = { 0.0f,0.0f };
	

	//定数バッファの生成
	ID3D12Resource* constBuffMaterial = nullptr;

	ID3D12Resource* constBuffTransform = nullptr;
	//定数バッファのマッピング用ポインタ
	ConstBufferDataTransform* constMapTransform = nullptr;

public:
	void Initialize(SpriteManager*spManager);

	void Draw();

	void preDraw();

	void postDraw();

	void SetPosition(XMFLOAT2 position);

	DirectX::XMFLOAT2 GetPosition() {return position_;}

	void TransferVertex();
};