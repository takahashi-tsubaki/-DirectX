#pragma once
#include <d3dx12.h>
#include "ErrorException.h"
using namespace Microsoft::WRL;

class TextureManager
{
public:
	// デスクリプターの数
	static const size_t kNumDescriptors = 256;

	struct Texture {
		// テクスチャリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		// シェーダリソースビューのハンドル(CPU)
		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
		// シェーダリソースビューのハンドル(CPU)
		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
		// 名前
		std::string name;
	};
private:

	//デバイス
	ComPtr<ID3D12Device> dev;

	//デスクリプタヒープ
	ComPtr <ID3D12DescriptorHeap> srvHeap;

	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};

	// テクスチャコンテナ
	std::array<Texture, kNumDescriptors> textures_;
	TextureManager() = default;
	~TextureManager() = default;

	
public:



	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static TextureManager* GetInstance();

	void Initalize();

	/// <returns>テクスチャハンドル</returns>
	static uint32_t Load(const std::string& fileName);

};