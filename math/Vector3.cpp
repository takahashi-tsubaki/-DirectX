#include "Vector3.h"

Vector3::Vector3() : Vector3(0, 0, 0) {}

Vector3::Vector3(float x, float y, float z) :
	x(x), y(y), z(z)
{}

Vector3& Vector3::operator+=(const Vector3& v) {
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3& v) {
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Vector3& Vector3::operator*=(float s) {
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

Vector3& Vector3::operator/=(float s) {
	x /= s;
	y /= s;
	z /= s;
	return *this;
}

Vector3 Vector3::operator+() const { return *this; }

Vector3 Vector3::operator-() const { return Vector3(-x, -y, -z); }

