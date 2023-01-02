#pragma once
#pragma once

#include "../Core/DirectXCommon.h"

#include "../2D/Sprite.h"

#include "../3D/Model.h"
#include "../3D/ViewProjection.h"
#include "../3D/WorldTransform.h"
#include "../Input.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
  /// <summary>
  /// コンストクラタ
  /// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Model* model_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
#pragma once
