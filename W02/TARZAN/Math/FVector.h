#pragma once
#include <cmath>

struct FVector {
	float x, y, z;
	static const FVector Up;
	static const FVector Forward;
	static const FVector Right;
	static const FVector One;
	static const FVector Zero;
	FVector(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}
	FVector operator*(float scalar) const {
		return FVector(x * scalar, y * scalar, z * scalar);
	}
	FVector operator*(const FVector& rhs) const {
		return FVector(x * rhs.x, y * rhs.y, z * rhs.z);
	}
	FVector operator-(const FVector& rhs) const {
		return FVector(x - rhs.x, y - rhs.y, z - rhs.z);
	}
	FVector operator+(const FVector& rhs) const {
		return FVector(x + rhs.x, y + rhs.y, z + rhs.z);
	}
	FVector operator/(float scalar) const {
		if ( scalar == 0 ) return FVector();
		return FVector(x / scalar, y / scalar, z / scalar);
	}
	FVector operator^(const FVector& rhs) const {
		return FVector(
			y * rhs.z - z * rhs.y,
			z * rhs.x - x * rhs.z,
			x * rhs.y - y * rhs.x
		);
	}
	FVector& operator*=(float scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}
	FVector& operator+=(const FVector& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}
	FVector& operator-=(const FVector& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}
	bool operator ==(const FVector& rhs) const {
		return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
	}
	bool operator !=(const FVector& rhs) const {
		return !(*this == rhs);
	}
	float Dot(const FVector& rhs) const {
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	float operator[] (int index) {
		switch (index)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			OutputDebugString(L"FVector3 Error: Out of range");
			assert(0);
			return 0;
		}
	}
	FVector Cross(const FVector& rhs) {
		return FVector(
			y*rhs.z - z*rhs.y,
			z*rhs.x - x*rhs.z,
			x*rhs.y - y*rhs.x
		);
	}
	float MagnitudeSquared() const {
		return (x * x + y * y + z * z);
	}
	float Magnitude() const {
		return std::sqrt(x * x + y * y + z * z);
	}
	FVector Normalized() const {
		float length = Magnitude();

		if ( length != 0 )
			return *this / length;

		return FVector();
	}
	operator std::string() const {
		return "(" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + ")";
	}
	FVector ProjectOn(FVector vec) const {
		FVector unit = vec.Normalized();
		return unit * unit.Dot(*this);
	}
};

struct FVector4 {
	float x, y, z, w;
	FVector4(float _x = 0, float _y = 0, float _z = 0, float _w = 0) : x(_x), y(_y), z(_z), w(_w) {}
	FVector4(FVector vec, float _w) : x(vec.x), y(vec.y), z(vec.z), w(_w) {} // Add
	FVector4 operator*(float scalar) const {
		return FVector4(x * scalar, y * scalar, z * scalar, w * scalar);
	}
	FVector4 operator*(const FVector4& rhs) const {
		return FVector4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
	}
	FVector4 operator-(const FVector4& rhs) const {
		return FVector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
	}
	FVector4 operator+(const FVector4& rhs) const {
		return FVector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
	}
	FVector4 operator/(float scalar) const {

		if (scalar == 0) {
#ifdef _DEBUG
			OutputDebugString(L"-------------------\ndivided Zero!\n-------------------");
			assert(0);
			return FVector4();
#endif // DEBUG
			return FVector4();
		}
		return FVector4(x / scalar, y / scalar, z / scalar, w / scalar);
	}
	FVector4& operator*=(float scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		w += scalar;
		return *this;
	}
	FVector4& operator+=(const FVector4& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		w += rhs.w;
		return *this;
	}
	FVector4& operator-=(const FVector4& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		w -= rhs.w;
		return *this;
	}
	bool operator ==(const FVector4& rhs) const {
		return (x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.w);
	}
	bool operator !=(const FVector4& rhs) const {
		return !(*this == rhs);
	}
	float operator[] (int index){
		switch (index)
		{
		case 0:
			return x;
			break;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		default:
			OutputDebugString(L"FVector4 Error: Out of range");
			assert(0);
			return 0;
		}
	}
	friend FVector4 operator*(float lhs, const FVector4& rhs) {
		return rhs * lhs;
	}
	float Dot(const FVector4& rhs) const {
		return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
	}
	float MagnitudeSquared() const {
		return (x * x + y * y + z * z + w * w);
	}
	float Magnitude() const {
		return std::sqrt(x * x + y * y + z * z + w * w);
	}
	FVector4 Normalized() const {
		float length = Magnitude();

		if ( length != 0 )
			return *this / length;

		return FVector4();
	}
	operator std::string() const {
		return "(" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + "," + std::to_string(w) + ")";
	}
	FVector xyz() const {
		return FVector(x, y, z);
	}
	FVector GetCoord() const {
		return xyz() / w;
	}
};