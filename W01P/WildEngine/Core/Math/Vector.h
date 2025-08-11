#pragma once
#include "Types/Types.h"


struct FVector
{
    float X;
    float Y;
    float Z;

    // 생성자
    FVector() : X(0), Y(0), Z(0) {}
    FVector(float X, float Y, float Z) : X(X), Y(Y), Z(Z) {}

    // 내적
    float Dot(const FVector& rhs) const
    {
        return X * rhs.X + Y * rhs.Y + Z * rhs.Z;
    }

    void Set(float x, float y, float z)
    {
        X = x;
        Y = y;
        Z = z;
    }

    // 외적
    FVector Cross(const FVector& rhs) const
    {
        return FVector{
            Y * rhs.Z - Z * rhs.Y,
            Z * rhs.X - X * rhs.Z,
            X * rhs.Y - Y * rhs.X,
        };
    }

    // 벡터의 길이
    float Length() const
    {
        return sqrt(X * X + Y * Y + Z * Z);
    }

    // 벡터 정규화
    void Normalize()
    {
        float L = Length();
        if (L > 0.0001f)
        {
            X /= L;
            Y /= L;
            Z /= L;
        }
    }

    // 벡터 정규화 반환
    FVector Normalized() const
    {
        float L = Length();
        if (L > 0.0001f)
        {
            return FVector(X / L, Y / L, Z / L);
        }
        return *this;
    }

    FVector Negative() const
    {
        return FVector(-X, -Y, -Z);
    }

    FVector& operator=(const FVector& rhs)
    {
        X = rhs.X;
        Y = rhs.Y;
        Z = rhs.Z;
        return *this;
    }

    // 벡터 덧셈 연산자
    FVector operator+(const FVector& rhs) const
    {
        return FVector(X + rhs.X, Y + rhs.Y, Z + rhs.Z);
    }

    // 벡터 뺄셈 연산자
    FVector operator-(const FVector& rhs) const
    {
        return FVector(X - rhs.X, Y - rhs.Y, Z - rhs.Z);
    }

    // 벡터 스칼라 곱 연산자 (오른쪽 스칼라 곱)
    FVector operator*(float Scalar) const
    {
        return FVector(X * Scalar, Y * Scalar, Z * Scalar);
    }

    // 벡터 스칼라 나누기 연산자
    FVector operator/(float Scalar) const
    {
        return FVector(X / Scalar, Y / Scalar, Z / Scalar);
    }

    // 벡터 덧셈 후 대입 연산자
    FVector& operator+=(const FVector& rhs)
    {
        X += rhs.X;
        Y += rhs.Y;
        Z += rhs.Z;
        return *this;
    }

    // 벡터 뺄셈 후 대입 연산자
    FVector& operator-=(const FVector& rhs)
    {
        X -= rhs.X;
        Y -= rhs.Y;
        Z -= rhs.Z;
        return *this;
    }

    // 벡터 스칼라 곱 후 대입 연산자
    FVector& operator*=(float Scalar)
    {
        X *= Scalar;
        Y *= Scalar;
        Z *= Scalar;
        return *this;
    }

    // 벡터 스칼라 나누기 후 대입 연산자
    FVector& operator/=(float Scalar)
    {
        X /= Scalar;
        Y /= Scalar;
        Z /= Scalar;
        return *this;
    }

    FVector operator-() const
    {
        return FVector(-X, -Y, -Z);
    }

};