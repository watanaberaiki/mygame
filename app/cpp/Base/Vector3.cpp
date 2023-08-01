#include "Vector3.h"
#include <cmath>

//零ベクトルに初期化する
Vector3::Vector3()
	:x(0), y(0), z(0)
{
}

//引数2つのコンストラクタ
Vector3::Vector3(float x, float y, float z)
	: x(x), y(y), z(z)
{
}

//ベクトルの長さを求める
float Vector3::length() const
{
	return sqrt((x * x) + (y * y) + (z * z));
}

//ベクトルを正規化(単位ベクトル)にする
Vector3& Vector3::normalize()
{
	float len = length();
	if (len != 0)
	{
		return *this /= len;
	}
	return *this;
}

//内積を求める
float Vector3::dot(const Vector3& v) const
{
	return (x * v.x) + (y * v.y) + (z * v.z);
}

//外積を求める
Vector3 Vector3::cross(const Vector3& v) const
{
	return { v.y * z - y * v.z,
			 v.x * z - x * v.z,
			 x * v.y - y * v.x };
}

//単項演算子＋を使えるようにする
Vector3 Vector3::operator+() const
{
	return *this;
}

//単項演算子－を使えるようにする
Vector3 Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}

//+=を使えるようにする
Vector3& Vector3::operator+=(const Vector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

//-=を使えるようにする
Vector3& Vector3::operator-=(const Vector3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

//ベクトルをｓ倍にする
Vector3& Vector3::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

//ベクトルを１/ｓ倍にする
Vector3& Vector3::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	return *this;
}

Vector3& Vector3::operator=(const XMFLOAT3& v) 
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

//Vector2クラスに属さない関数群
//二項演算子
const Vector3 operator+(const Vector3& v1, const Vector3& v2)
{
	Vector3 temp(v1);
	return temp += v2;
}

const Vector3 operator-(const Vector3& v1, const Vector3& v2)
{
	Vector3 temp(v1);
	return temp -= v2;
}

const Vector3 operator*(const Vector3& v, float s)
{
	Vector3 temp(v);
	return temp *= s;
}

const Vector3 operator*(float s, const Vector3& v)
{
	return v * s;
}

const Vector3 operator/(const Vector3& v, float s)
{
	Vector3 temp(v);
	return temp /= s;
}

//線形補完
const Vector3 lerp(const Vector3& start, const Vector3& end, const float t)
{
	return start * (1.0f - t) + end * t;
}
