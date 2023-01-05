#pragma once

#include "../math//MyMath.h"

#include <d3d12.h>
#include <wrl.h>

// 定数バッファ用データ構造体
struct ConstBufferDataViewProjection {
	Matrix4 view;       // ワールド → ビュー変換行列
	Matrix4 projection; // ビュー → プロジェクション変換行列
	Vector3 cameraPos;  // カメラ座標（ワールド座標）
};

// ビュープロジェクション変換データ
struct ViewProjection {
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	// マッピング済みアドレス
	ConstBufferDataViewProjection* constMap = nullptr;

	// 視点座標
	Vector3 eye = { 0, 0, -50.0f };
	// 注視点座標
	Vector3 target = { 0, 0, 0 };
	// 上方向ベクトル
	Vector3 up = { 0, 1, 0 };


	// 垂直方向視野角
	float fovAngleY = 45.0f * MyMath::PI / 180.0f;
	// ビューポートのアスペクト比
	float aspectRatio = (float)16 / 9;
	// 深度限界（手前側）
	float nearZ = 0.1f;
	// 深度限界（奥側）
	float farZ = 1000.0f;

	// ビュー行列
	Matrix4 matView;

	// 射影行列
	Matrix4 matProjection;

	// 初期化
	void Initialize();

	//定数バッファの生成
	void CreateConstBuffer();

	/// マッピング
	void Map();

	// 行列の更新
	void UpdateMatrix();

	// 行列の転送
	void TransferMatrix();
};

