#pragma once
#include "../Core/DirectXCommon.h"
class SpriteManager
{
private:

	//頂点データ
	std::array<XMFLOAT3, 3> vertices;

	//頂点バッファービューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature;
	//パイプラインステート
	ID3D12PipelineState* pipelineState = nullptr;

	//スプライト用コマンドリスト
	ID3D12GraphicsCommandList* sCommandList;

	DirectXCommon* dxCommon_ = nullptr;

public:
	void Initialize(DirectXCommon*dxCommon);

	void Update();

	void Draw();
};