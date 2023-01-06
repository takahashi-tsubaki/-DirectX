#pragma once
#include "../Core/DirectXCommon.h"
class SpriteManager
{
private:

	

	//横方向ピクセル数
	const size_t textureWidth = 256;
	//縦方向ピクセル数
	const size_t textureHeight = 256;
	//配列の要素数
	const size_t imageDataCount = textureWidth * textureHeight;
	//画像イメージデータ配列
	XMFLOAT4* imageData = new XMFLOAT4[imageDataCount];

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature;
	//パイプラインステート
	ID3D12PipelineState* pipelineState = nullptr;

	//スプライト用コマンドリスト
	ID3D12GraphicsCommandList* sCommandList;

	DirectXCommon* dxCommon_ = nullptr;

	

public:

	uint32_t Load(const std::string& fileName);

	static SpriteManager* GetInstance();

	DirectXCommon* GetDxCommon() { return dxCommon_; }

	ID3D12RootSignature* GetRootSignature() {return rootSignature;}

	ID3D12PipelineState* GetPipelineState() { return pipelineState; }

	void Initialize(DirectXCommon* dxCommon);

	void Update();
};