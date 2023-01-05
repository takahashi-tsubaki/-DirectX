#pragma once

#include "../math/Vector2.h"
#include "../math/Vector3.h"

#include "Material.h"
#include <Windows.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <unordered_map>
#include <vector>
#include <wrl.h>

class Mesh {
private:
	// Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	//頂点データ構造体（テクスチャあり）
	struct VertexPosNormalUv {
		Vector3 pos;    // xyz座標
		Vector3 normal; // 法線ベクトル
		Vector2 uv;     // uv座標
	};

// メンバ関数

	// 名前を取得
	const std::string& GetName() { return name_; }

	// 名前をセット
	void SetName(const std::string& name_);

	///頂点データの追加
	void AddVertex(const VertexPosNormalUv& vertex);

	// インデックスデータの追加
	void AddIndex(unsigned short index);

	//　頂点データの数を取得
	inline size_t GetVertexCount() { return vertices_.size(); }

	// エッジ平滑化データの追加
	void AddSmoothData(unsigned short indexPosition, unsigned short indexVertex);

	// 平滑化された頂点法線の計算
	void CalculateSmoothedVertexNormals();

	// マテリアルの取得
	Material* GetMaterial() { return material_; }

	// マテリアルの割り当て
	void SetMaterial(Material* material);

	// バッファの生成
	void CreateBuffers();

	// 頂点バッファの取得
	const D3D12_VERTEX_BUFFER_VIEW& GetVBView() { return vbView_; }

	/// インデックスバッファの取得
	const D3D12_INDEX_BUFFER_VIEW& GetIBView() { return ibView_; }

	// 描画
	void Draw(
		ID3D12GraphicsCommandList* commandList, UINT rooParameterIndexMaterial,
		UINT rooParameterIndexTexture);

	// 描画（テクスチャ差し替え版）
	//　差し替えるテクスチャハンドル
	void Draw(
		ID3D12GraphicsCommandList* commandList, UINT rooParameterIndexMaterial,
		UINT rooParameterIndexTexture, uint32_t textureHandle);

	// 頂点配列の取得
	inline const std::vector<VertexPosNormalUv>& GetVertices() { return vertices_; }

	// インデックス配列の取得
	inline const std::vector<unsigned short>& GetIndices() { return indices_; }

private: // メンバ変数
  // 名前
	std::string name_;
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff_;
	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};
	// 頂点データ配列
	std::vector<VertexPosNormalUv> vertices_;
	// 頂点インデックス配列
	std::vector<unsigned short> indices_;
	// 頂点法線スムージング用データ
	std::unordered_map<unsigned short, std::vector<unsigned short>> smoothData_;
	// マテリアル
	Material* material_ = nullptr;
};
