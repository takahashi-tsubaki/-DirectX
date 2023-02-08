#pragma once

#include "collisionPrimitive.h"

class Collision
{
public:
	static bool CheckSphere2Plane(const Sphere& sphere,const Plane& plane,DirectX::XMVECTOR* inter = nullptr);
};