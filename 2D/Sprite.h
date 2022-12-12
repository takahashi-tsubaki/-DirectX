#pragma once
#include "SpriteManager.h"
class Sprite
{
private:


	SpriteManager* spManager_ = nullptr;
public:
	void Initialize(SpriteManager*spManager);

	void Draw();

	void preDraw();

	void postDraw();
};