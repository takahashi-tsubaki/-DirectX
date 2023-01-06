#pragma once
#include "SpriteManager.h"
class Sprite
{
private:

	ID3D12GraphicsCommandList* sCommandList_ = nullptr;

	SpriteManager* spManager_ = nullptr;
public:
	void Initialize(SpriteManager*spManager);

	void Draw();

	void preDraw();

	void postDraw();
};