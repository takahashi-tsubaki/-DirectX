#pragma once
#include "../Input.h"
#include "../Global.h"
#include "../2D/Sprite.h"
class resultScene
{
public:

	void Initialize();

	void Update();

	void Draw();

private:

	Sprite* numSprite_ = nullptr;

	Scene scene;

	int winNum = 0;

};