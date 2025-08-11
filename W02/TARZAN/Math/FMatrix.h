#pragma once
#include <cmath>
#include <initializer_list>
#include "FVector.h"
enum class EAxis
{
	X,
	Y,
	Z
};

struct FMatrix {
public:
	float m[4][4];
	FMatrix(float[4][4]);
	FMatrix(FVector4 r[4]);
	FMatrix(const std::initializer_list<float>& m);
	FMatrix(const FMatrix& other);

	FMatrix operator+(const FMatrix& rhs) const;
	FMatrix operator-(const FMatrix& rhs) const;
	FMatrix operator*(const float& rhs) const;
	FMatrix operator*(const FMatrix& rhs) const;
	FVector4 operator[](int row){
		return FVector4(m[row][0], m[row][1], m[row][2], m[row][3]);
	}
	FMatrix& operator=(const FMatrix& other);
	bool operator==(const FMatrix& other) const;
	bool operator!=(const FMatrix& other) const;
	
//FVector4 operator*(const FVector4& rhs) const;

	FVector4 r1() const;
	FVector4 r2() const;
	FVector4 r3() const;
	FVector4 r4() const;

	FVector4 c1() const;
	FVector4 c2() const;
	FVector4 c3() const;
	FVector4 c4() const;

	FMatrix Transpose() const;
	FMatrix Inverse() const;
	friend FVector4 operator*(const FVector4& lhs, const FMatrix& rhs);
	std::wstring to_wstring() const;
	std::string to_string() const;

	FVector TransformCoord(FVector4 vec) {
		FVector4 v = (vec * (*this));
		return FVector(v.x / v.w, v.y / v.w, v.z / v.w);
	}
	FVector TransformCoord(const FVector& v) {
		FVector4 v4 = FVector4(v, 1);
		
		return TransformCoord(v4);
	}

public:
	static const FMatrix Identity;
	FVector GetScaledAxis(EAxis axis) const;
	static FMatrix Scale(float sx, float sy, float sz);
	static FMatrix Scale(FVector xyz);
	static FMatrix RotateX(float rx);
	static FMatrix RotateY(float ry);
	static FMatrix RotateZ(float rz);
	static FMatrix RotateXYZ(FVector xyz);
	static FMatrix Translate(float tx, float ty, float tz);
	static FMatrix Translate(FVector xyz);
	static FMatrix MakeFromX(FVector xaxis);
	static FMatrix MakeFromY(FVector yaxis);
	static FMatrix MakeFromZ(FVector zaxis);
	FMatrix Swap(UINT r1, UINT r2);
};

inline std::wstring FMatrix::to_wstring() const {
	std::wstring str;
	std::wstring sep(L" ");
	for ( int i = 0; i < 4; i++ ) {
		for ( int j = 0; j < 4; j++ ) {
			str += std::to_wstring(m[i][j]);
			str += sep;
		}
		str += std::wstring(L"\n");
	}
	return str;
}

//#pragma once
//#include <cmath>
//#include <initializer_list>
//#include "FVector.h"
//
//struct FMatrix {
//public:
//	float m[4][4];
//	FMatrix(float[4][4]);
//	FMatrix(FVector4 r[4]);
//	FMatrix(const std::initializer_list<float>& m);
//
//	FMatrix operator+(const FMatrix& rhs) const;
//	FMatrix operator-(const FMatrix& rhs) const;
//	FMatrix operator*(const float& rhs) const;
//	FMatrix operator*(const FMatrix& rhs) const;
//	FVector4 operator[](int row){
//		return FVector4(m[row][0], m[row][1], m[row][2], m[row][3]);
//	}
//	
////FVector4 operator*(const FVector4& rhs) const;
//
//	FVector4 r1() const;
//	FVector4 r2() const;
//	FVector4 r3() const;
//	FVector4 r4() const;
//
//	FVector4 c1() const;
//	FVector4 c2() const;
//	FVector4 c3() const;
//	FVector4 c4() const;
//
//	FMatrix Transpose() const;
//	friend FVector4 operator*(const FVector4& lhs, const FMatrix& rhs);
//
//public:
//	static const FMatrix Identity;
//	static FMatrix Scale(float sx, float sy, float sz);
//	static FMatrix RotateX(float rx);
//	static FMatrix RotateY(float ry);
//	static FMatrix RotateZ(float rz);
//	static FMatrix Translate(float tx, float ty, float tz);
//};
