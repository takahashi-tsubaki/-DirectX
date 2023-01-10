#pragma once
#include "../Input.h"
#include "../Global.h"
class titleScene
{
public:

	void Initialize(Scene scene);

	void Update();

	void Draw();

private:
	Input* input_ = nullptr;

	Scene scene_;
};