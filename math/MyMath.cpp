#include "MyMath.h"
#include <cassert>
#include <cmath>

namespace MyMath
{
	float Vector2Length(const Vector2& v) {
		return (float)std::sqrt(v.x * v.x + v.y * v.y);
	}

	const Vector3 Vector3Zero() {
		static const Vector3 result{ 0.0f, 0.0f, 0.0f };

		return result;
	}

	bool Vector3Equal(const Vector3& v1, const Vector3& v2) {
		return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
	}

	float Vector3Length(const Vector3& v) {
		return (float)std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	}

	Vector3& Vector3Normalize(Vector3& v) {
		float len = Vector3Length(v);
		if (len != 0) {
			return v /= len;
		}
		return v;
	}

	float Vector3Dot(const Vector3& v1, const Vector3& v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	Vector3 Vector3Cross(const Vector3& v1, const Vector3& v2) {
		Vector3 temp;
		temp.x = v1.y * v2.z - v1.z * v2.y;
		temp.y = v1.z * v2.x - v1.x * v2.z;
		temp.z = v1.x * v2.y - v1.y * v2.x;
		return temp;
	}

	// 2項演算子オーバーロード
	const Vector3 operator+(const Vector3& v1, const Vector3& v2) {
		Vector3 temp(v1);
		return temp += v2;
	}

	const Vector3 operator-(const Vector3& v1, const Vector3& v2) {
		Vector3 temp(v1);
		return temp -= v2;
	}

	const Vector3 operator*(const Vector3& v, float s) {
		Vector3 temp(v);
		return temp *= s;
	}

	const Vector3 operator*(float s, const Vector3& v) { return v * s; }

	const Vector3 operator/(const Vector3& v, float s) {
		Vector3 temp(v);
		return temp /= s;
	}

	// 単位行列を求める
	Matrix4 Matrix4Identity() {
		static const Matrix4 result{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
									0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

		return result;
	}

	Matrix4 Matrix4Transpose(const Matrix4& m) {
		Matrix4 result;

		for (size_t i = 0; i < 4; i++) {
			for (size_t j = 0; j < 4; j++) {
				result.m[i][j] = m.m[j][i];
			}
		}

		return result;
	}

	float Matrix4Determinant(const Matrix4& m) {

		float det = +m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3]
			+ m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1]
			+ m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]

			- m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1]
			- m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3]
			- m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]

			- m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3]
			- m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1]
			- m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]

			+ m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1]
			+ m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3]
			+ m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]

			+ m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3]
			+ m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1]
			+ m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]

			- m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1]
			- m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3]
			- m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]

			- m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0]
			- m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0]
			- m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]

			+ m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0]
			+ m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0]
			+ m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];
		return det;
	}

	Matrix4 Matrix4Inverse(const Matrix4& m, float* pdet) {

		// 行列式を得る
		float det = Matrix4Determinant(m);
		if (pdet) {
			*pdet = det;
		}

		assert(std::abs(det) < 1.0e-10 && "Determinant is nearly equal to zero");

		float divDet = 1.0f / det;

		Matrix4 result;

		result.m[0][0] =
			divDet * (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] +
				m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] -
				m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]);

		result.m[0][1] =
			divDet * (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] -
				m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] +
				m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]);

		result.m[0][2] =
			divDet * (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] +
				m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] -
				m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]);

		result.m[0][3] =
			divDet * (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] -
				m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] +
				m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]);

		result.m[1][0] =
			divDet * (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] -
				m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] +
				m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]);

		result.m[1][1] =
			divDet * (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] +
				m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] -
				m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]);

		result.m[1][2] =
			divDet * (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] -
				m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] +
				m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]);

		result.m[1][3] =
			divDet * (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] +
				m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] -
				m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]);

		result.m[2][0] =
			divDet * (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] +
				m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] -
				m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]);

		result.m[2][1] =
			divDet * (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] -
				m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] +
				m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]);

		result.m[2][2] =
			divDet * (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] +
				m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] -
				m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]);

		result.m[2][3] =
			divDet * (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] -
				m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] +
				m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]);

		result.m[3][0] =
			divDet * (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] -
				m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] +
				m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]);

		result.m[3][1] =
			divDet * (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] +
				m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] -
				m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]);

		result.m[3][2] =
			divDet * (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] -
				m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] +
				m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]);

		result.m[3][3] =
			divDet * (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] +
				m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] -
				m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]);

		return result;
	}

	Matrix4 Matrix4Scaling(float sx, float sy, float sz) {
		Matrix4 result{ sx,   0.0f, 0.0f, 0.0f, 0.0f, sy,   0.0f, 0.0f,
					   0.0f, 0.0f, sz,   0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

		return result;
	}

	Matrix4 Matrix4RotationX(float angle) {
		float sin = std::sin(angle);
		float cos = std::cos(angle);

		Matrix4 result{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cos,  sin,  0.0f,
					   0.0f, -sin, cos,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

		return result;
	}

	Matrix4 Matrix4RotationY(float angle) {
		float sin = std::sin(angle);
		float cos = std::cos(angle);

		Matrix4 result{ cos, 0.0f, -sin, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
					   sin, 0.0f, cos,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

		return result;
	}

	Matrix4 Matrix4RotationZ(float angle) {
		float sin = std::sin(angle);
		float cos = std::cos(angle);

		Matrix4 result{ cos,  sin,  0.0f, 0.0f, -sin, cos,  0.0f, 0.0f,
					   0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

		return result;
	}

	Matrix4 Matrix4Translation(float tx, float ty, float tz) {
		Matrix4 result{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
					   0.0f, 0.0f, 1.0f, 0.0f, tx,   ty,   tz,   1.0f };

		return result;
	}

	Matrix4 Matrix4LookAtLH(const Vector3& eye, const Vector3& target, const Vector3& up) {
		assert(!Vector3Equal(eye, target));
		assert(!Vector3Equal(up, Vector3Zero()));

		// 視線方向
		Vector3 eyeDirection = target - eye;

		// 各軸ベクトルを算出
		Vector3 R2 = Vector3Normalize(eyeDirection);

		Vector3 R0 = Vector3Cross(up, R2);
		R0 = Vector3Normalize(R0);

		Vector3 R1 = Vector3Cross(R2, R0);

		// 平行移動成分を算出
		Vector3 NegEyePosition = -eye;

		float D0 = Vector3Dot(R0, NegEyePosition);
		float D1 = Vector3Dot(R1, NegEyePosition);
		float D2 = Vector3Dot(R2, NegEyePosition);

		Matrix4 m{ R0.x, R1.x, R2.x, 0.0f, R0.y, R1.y, R2.y, 0.0f,
				  R0.z, R1.z, R2.z, 0.0f, D0,   D1,   D2,   1.0f };

		return m;
	}

	Matrix4 Matrix4Orthographic(
		float viewLeft, float viewRight, float viewBottom, float viewTop, float nearZ, float farZ) {
		assert(fabsf(viewRight - viewLeft) > 0.00001f);
		assert(fabsf(viewTop - viewBottom) > 0.00001f);
		assert(fabsf(farZ - nearZ) > 0.00001f);

		float width = 1.0f / (viewRight - viewLeft);
		float height = 1.0f / (viewTop - viewBottom);
		float fRange = 1.0f / (farZ - nearZ);
		float sx = width * 2.0f;
		float sy = height * 2.0f;
		float sz = fRange;
		float tx = -(viewLeft + viewRight) * width;
		float ty = -(viewTop + viewBottom) * height;
		float tz = -fRange * nearZ;

		Matrix4 m{ sx, 0.0f, 0.0f, 0.0f, 0.0f, sy, 0.0f, 0.0f, 0.0f, 0.0f, sz, 0.0f, tx, ty, tz, 1.0f };

		return m;
	}

	Matrix4 Matrix4Perspective(float fovAngleY, float aspectRatio, float nearZ, float farZ) {
		assert(nearZ > 0.0f && farZ > 0.f);
		assert(fabsf(fovAngleY) > 0.00001f * 2.0f);
		assert(fabsf(aspectRatio) > 0.00001f);
		assert(fabsf(farZ - nearZ) > 0.00001f);

		float sinFov = std::sin(0.5f * fovAngleY);
		float cosFov = std::cos(0.5f * fovAngleY);

		float height = cosFov / sinFov;
		float width = height / aspectRatio;
		float fRange = farZ / (farZ - nearZ);

		Matrix4 m{ width, 0.0f,   0.0f, 0.0f, 0.0f, height,          0.0f, 0.0f, 0.0f,
				  0.0f,  fRange, 1.0f, 0.0f, 0.0f, -fRange * nearZ, 0.0f };

		return m;
	}

	Vector3 Vector3Transform(const Vector3& v, const Matrix4& m) {

		Vector3 result{
		  v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0],
		  v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1],
		  v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2] };

		return result;
	}

	Vector3 Vector3TransformCoord(const Vector3& v, const Matrix4& m) {
		float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];

		Vector3 result{
		  (v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0]) / w,
		  (v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1]) / w,
		  (v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2]) / w };

		return result;
	}

	Vector3 Vector3TransformNormal(const Vector3& v, const Matrix4& m) {

		Vector3 result{
		  v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
		  v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
		  v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] };

		return result;
	}

	Matrix4 operator*(const Matrix4& m1, const Matrix4& m2) {
		Matrix4 result = m1;

		return result *= m2;
	}

	Vector3 operator*(const Vector3& v, const Matrix4& m) { return Vector3Transform(v, m); }

	float Lerp(float a, float b, float t) { return a + t * (b - a); }

} // namespace MathUtility
