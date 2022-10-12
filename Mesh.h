#pragma once
#include <d3d12.h>
#include <vector>
#include <DirectXMath.h>
#include <d3dcompiler.h>

using namespace DirectX;

//定数バッファ
struct ConstBufferDataMaterial
{
	XMFLOAT4 color;//色
};

class Mesh
{
public:
	//コンストラクタ
	Mesh(XMFLOAT3 vertices[3], ID3D12Device* dev);

	//描画関数
	void Draw(ID3D12GraphicsCommandList* commandList);

private:

	//頂点データ
	XMFLOAT3 vertices[3] =
	{
		
	};

	uint16_t indices[3] =
	{
		0,1,2,
	};

	HRESULT result;

	UINT sizeVB;

	//頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{};//ヒープ設定

	//リソース設定
	D3D12_RESOURCE_DESC resDesc{};

	//GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	XMFLOAT3* vertMap = nullptr;

	//頂点バッファービューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	ID3DBlob* vsBlob = nullptr;//頂点シェーダオブジェクト
	ID3DBlob* psBlob = nullptr;//ピクセルシェーダオブジェクト
	ID3DBlob* errorBlob = nullptr;//エラーオブジェクト

	//頂点バッファの作成
	ID3D12Resource* vertBuff = nullptr;

	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};

	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};

	ID3D12Resource* constBuffMaterial = nullptr;

	//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial = nullptr;

	//ルートパラメータ
	D3D12_ROOT_PARAMETER rootParam = {};

	//インデックスデータサイズ
	UINT sizeIB;

	//インデックスバッファの生成
	ID3D12Resource* indexBuff = nullptr;

	//インデックスバッファをマッピング
	uint16_t* indexMap = nullptr;

	//インデックスバッファビューの生成
	D3D12_INDEX_BUFFER_VIEW ibView{};

	//グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature;

	//ルートシグネチャの設定(生成)
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};

	//ルートシグネチャのシリアライズ
	ID3DBlob* rootSigBlob = nullptr;

	//パイプラインステートの生成
	ID3D12PipelineState* pipelineState;

};