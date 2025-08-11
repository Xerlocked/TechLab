#pragma once
#include <cstring>
#include "Vector.h"

struct FMatrix
{
    float M[4][4];
    // 단위 행렬
    static const FMatrix Identity()
    {
        FMatrix m;
        m.SetIdentity();
        return m;
    }

    // 기본 생성자 (0으로 초기화)
    FMatrix()
    {
        memset(M, 0, sizeof(M));
    }

    // 초기화 생성자
    FMatrix(const float InitM[4][4])
    {
        memcpy(M, InitM, sizeof(M));
    }

    // 대입 연산자
    FMatrix& operator=(const FMatrix& Other)
    {
        if (this != &Other)
        {
            memcpy(M, Other.M, sizeof(M));
        }
        return *this;
    }

    // 곱 연산자
    FMatrix operator*(const FMatrix& rhs) const
    {
        FMatrix Result;

        // 첫 번째 행
        Result.M[0][0] = M[0][0] * rhs.M[0][0] + M[0][1] * rhs.M[1][0] + M[0][2] * rhs.M[2][0] + M[0][3] * rhs.M[3][0];
        Result.M[0][1] = M[0][0] * rhs.M[0][1] + M[0][1] * rhs.M[1][1] + M[0][2] * rhs.M[2][1] + M[0][3] * rhs.M[3][1];
        Result.M[0][2] = M[0][0] * rhs.M[0][2] + M[0][1] * rhs.M[1][2] + M[0][2] * rhs.M[2][2] + M[0][3] * rhs.M[3][2];
        Result.M[0][3] = M[0][0] * rhs.M[0][3] + M[0][1] * rhs.M[1][3] + M[0][2] * rhs.M[2][3] + M[0][3] * rhs.M[3][3];

        // 두 번째 행
        Result.M[1][0] = M[1][0] * rhs.M[0][0] + M[1][1] * rhs.M[1][0] + M[1][2] * rhs.M[2][0] + M[1][3] * rhs.M[3][0];
        Result.M[1][1] = M[1][0] * rhs.M[0][1] + M[1][1] * rhs.M[1][1] + M[1][2] * rhs.M[2][1] + M[1][3] * rhs.M[3][1];
        Result.M[1][2] = M[1][0] * rhs.M[0][2] + M[1][1] * rhs.M[1][2] + M[1][2] * rhs.M[2][2] + M[1][3] * rhs.M[3][2];
        Result.M[1][3] = M[1][0] * rhs.M[0][3] + M[1][1] * rhs.M[1][3] + M[1][2] * rhs.M[2][3] + M[1][3] * rhs.M[3][3];

        // 세 번째 행
        Result.M[2][0] = M[2][0] * rhs.M[0][0] + M[2][1] * rhs.M[1][0] + M[2][2] * rhs.M[2][0] + M[2][3] * rhs.M[3][0];
        Result.M[2][1] = M[2][0] * rhs.M[0][1] + M[2][1] * rhs.M[1][1] + M[2][2] * rhs.M[2][1] + M[2][3] * rhs.M[3][1];
        Result.M[2][2] = M[2][0] * rhs.M[0][2] + M[2][1] * rhs.M[1][2] + M[2][2] * rhs.M[2][2] + M[2][3] * rhs.M[3][2];
        Result.M[2][3] = M[2][0] * rhs.M[0][3] + M[2][1] * rhs.M[1][3] + M[2][2] * rhs.M[2][3] + M[2][3] * rhs.M[3][3];

        // 네 번째 행
        Result.M[3][0] = M[3][0] * rhs.M[0][0] + M[3][1] * rhs.M[1][0] + M[3][2] * rhs.M[2][0] + M[3][3] * rhs.M[3][0];
        Result.M[3][1] = M[3][0] * rhs.M[0][1] + M[3][1] * rhs.M[1][1] + M[3][2] * rhs.M[2][1] + M[3][3] * rhs.M[3][1];
        Result.M[3][2] = M[3][0] * rhs.M[0][2] + M[3][1] * rhs.M[1][2] + M[3][2] * rhs.M[2][2] + M[3][3] * rhs.M[3][2];
        Result.M[3][3] = M[3][0] * rhs.M[0][3] + M[3][1] * rhs.M[1][3] + M[3][2] * rhs.M[2][3] + M[3][3] * rhs.M[3][3];

        return Result;
    }

    FVector operator*(const FVector& v) const
    {
        // v를 (v.X, v.Y, v.Z, 1.0f) 로 확장하여 곱셈 수행
        float x = v.X * M[0][0] + v.Y * M[1][0] + v.Z * M[2][0] + 1.0f * M[3][0];
        float y = v.X * M[0][1] + v.Y * M[1][1] + v.Z * M[2][1] + 1.0f * M[3][1];
        float z = v.X * M[0][2] + v.Y * M[1][2] + v.Z * M[2][2] + 1.0f * M[3][2];
        float w = v.X * M[0][3] + v.Y * M[1][3] + v.Z * M[2][3] + 1.0f * M[3][3];

        // w가 0에 가까우면 분모가 0이 되는 것을 방지
        if (std::fabs(w) > 1e-6f)
        {
            x /= w;
            y /= w;
            z /= w;
        }

        return FVector(x, y, z);
    }

    // 정적 함수: Roll, Pitch, Yaw (단위: 라디안)를 이용하여 회전 행렬 생성
    static FMatrix CreateRotationRollPitchYaw(float Pitch, float Yaw, float Roll)
    {
        // 각 축에 대한 삼각함수 값 계산
        float sp = std::sin(Pitch);
        float cp = std::cos(Pitch);
        float sy = std::sin(Yaw);
        float cy = std::cos(Yaw);
        float sr = std::sin(Roll);
        float cr = std::cos(Roll);

        FMatrix result;

        // 첫 번째 행
        result.M[0][0] = cr * cy + sr * sp * sy;
        result.M[0][1] = sr * cp;
        result.M[0][2] = -cr * sy + sr * sp * cy;
        result.M[0][3] = 0.0f;

        // 두 번째 행
        result.M[1][0] = -sr * cy + cr * sp * sy;
        result.M[1][1] = cr * cp;
        result.M[1][2] = sr * sy + cr * sp * cy;
        result.M[1][3] = 0.0f;

        // 세 번째 행
        result.M[2][0] = cp * sy;
        result.M[2][1] = -sp;
        result.M[2][2] = cp * cy;
        result.M[2][3] = 0.0f;

        // 네 번째 행 (동차 좌표)
        result.M[3][0] = 0.0f;
        result.M[3][1] = 0.0f;
        result.M[3][2] = 0.0f;
        result.M[3][3] = 1.0f;

        return result;
    }

    static FMatrix Translation(float tx, float ty, float tz)
    {
        // 행렬을 행 우선(row-major) 순서로 초기화
        float initM[4][4] = {
            { 1.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f, 0.0f },
            { tx,   ty,   tz,   1.0f }
        };
        return FMatrix(initM);
    }

    static FMatrix Scaling(float sx, float sy, float sz)
    {
        float initM[4][4] = {
            { sx,   0.0f, 0.0f, 0.0f },
            { 0.0f, sy,   0.0f, 0.0f },
            { 0.0f, 0.0f, sz,   0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        };
        return FMatrix(initM);
    }

    void SetIdentity()
    {
        M[0][0] = 1; M[0][1] = 0;  M[0][2] = 0;  M[0][3] = 0;
        M[1][0] = 0; M[1][1] = 1;  M[1][2] = 0;  M[1][3] = 0;
        M[2][0] = 0; M[2][1] = 0;  M[2][2] = 1;  M[2][3] = 0;
        M[3][0] = 0; M[3][1] = 0;  M[3][2] = 0;  M[3][3] = 1;
    }

    // 전치 행렬 (Transpose)
    FMatrix Transpose() const
    {
        FMatrix Result;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                Result.M[i][j] = M[j][i];
        return Result;
    }

    // 행 벡터(Row Vector) 반환
    FVector GetRow(int Row) const
    {
        return FVector(M[Row][0], M[Row][1], M[Row][2]);
    }

    void SetRow(int Row, FVector Value)
    {
        M[Row][0] = Value.X;
        M[Row][1] = Value.Y;
        M[Row][2] = Value.Z;
    }

    // 열 벡터(Column Vector) 반환
    FVector GetColumn(int Col) const
    {
        return FVector(M[0][Col], M[1][Col], M[2][Col]);
    }

    void SetColumn(int Col, FVector Value)
    {
        M[0][Col] = Value.X;
        M[1][Col] = Value.Y;
        M[2][Col] = Value.Z;
    }

    // 행렬식 (Determinant)
    float Determinant() const
    {
        return
            M[0][3] * M[1][2] * M[2][1] * M[3][0] - M[0][2] * M[1][3] * M[2][1] * M[3][0] -
            M[0][3] * M[1][1] * M[2][2] * M[3][0] + M[0][1] * M[1][3] * M[2][2] * M[3][0] +
            M[0][2] * M[1][1] * M[2][3] * M[3][0] - M[0][1] * M[1][2] * M[2][3] * M[3][0] -
            M[0][3] * M[1][2] * M[2][0] * M[3][1] + M[0][2] * M[1][3] * M[2][0] * M[3][1] +
            M[0][3] * M[1][0] * M[2][2] * M[3][1] - M[0][0] * M[1][3] * M[2][2] * M[3][1] -
            M[0][2] * M[1][0] * M[2][3] * M[3][1] + M[0][0] * M[1][2] * M[2][3] * M[3][1] +
            M[0][3] * M[1][1] * M[2][0] * M[3][2] - M[0][1] * M[1][3] * M[2][0] * M[3][2] -
            M[0][3] * M[1][0] * M[2][1] * M[3][2] + M[0][0] * M[1][3] * M[2][1] * M[3][2] +
            M[0][1] * M[1][0] * M[2][3] * M[3][2] - M[0][0] * M[1][1] * M[2][3] * M[3][2] -
            M[0][2] * M[1][1] * M[2][0] * M[3][3] + M[0][1] * M[1][2] * M[2][0] * M[3][3] +
            M[0][2] * M[1][0] * M[2][1] * M[3][3] - M[0][0] * M[1][2] * M[2][1] * M[3][3] -
            M[0][1] * M[1][0] * M[2][2] * M[3][3] + M[0][0] * M[1][1] * M[2][2] * M[3][3];
    }

    // 행렬의 역행렬 (Inverse Matrix)
    FMatrix Inverse() const
    {
        FMatrix Result;
        float det = Determinant();
        if (std::fabs(det) < 1e-6f)
        {
            // 행렬이 역행렬을 가질 수 없는 경우, 단위 행렬 반환
            return FMatrix::Identity();
        }

        float invDet = 1.0f / det;

        Result.M[0][0] = invDet * (M[1][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) - M[1][2] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) + M[1][3] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]));
        Result.M[0][1] = invDet * (M[0][2] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) - M[0][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) - M[0][3] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]));
        Result.M[0][2] = invDet * (M[0][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) - M[0][2] * (M[1][1] * M[3][3] - M[1][3] * M[3][1]) + M[0][3] * (M[1][1] * M[3][2] - M[1][2] * M[3][1]));
        Result.M[0][3] = invDet * (M[0][2] * (M[1][3] * M[2][1] - M[1][1] * M[2][3]) - M[0][1] * (M[1][3] * M[2][2] - M[1][2] * M[2][3]) - M[0][3] * (M[1][1] * M[2][2] - M[1][2] * M[2][1]));

        Result.M[1][0] = invDet * (M[1][2] * (M[2][0] * M[3][3] - M[2][3] * M[3][0]) - M[1][0] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) - M[1][3] * (M[2][0] * M[3][2] - M[2][2] * M[3][0]));
        Result.M[1][1] = invDet * (M[0][0] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) - M[0][2] * (M[2][0] * M[3][3] - M[2][3] * M[3][0]) + M[0][3] * (M[2][0] * M[3][2] - M[2][2] * M[3][0]));
        Result.M[1][2] = invDet * (M[0][2] * (M[1][0] * M[3][3] - M[1][3] * M[3][0]) - M[0][0] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) - M[0][3] * (M[1][0] * M[3][2] - M[1][2] * M[3][0]));
        Result.M[1][3] = invDet * (M[0][0] * (M[1][3] * M[2][2] - M[1][2] * M[2][3]) - M[0][2] * (M[1][3] * M[2][0] - M[1][0] * M[2][3]) + M[0][3] * (M[1][0] * M[2][2] - M[1][2] * M[2][0]));

        Result.M[2][0] = invDet * (M[1][0] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) - M[1][1] * (M[2][0] * M[3][3] - M[2][3] * M[3][0]) + M[1][3] * (M[2][0] * M[3][1] - M[2][1] * M[3][0]));
        Result.M[2][1] = invDet * (M[0][1] * (M[2][0] * M[3][3] - M[2][3] * M[3][0]) - M[0][0] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) - M[0][3] * (M[2][0] * M[3][1] - M[2][1] * M[3][0]));
        Result.M[2][2] = invDet * (M[0][0] * (M[1][1] * M[3][3] - M[1][3] * M[3][1]) - M[0][1] * (M[1][0] * M[3][3] - M[1][3] * M[3][0]) + M[0][3] * (M[1][0] * M[3][1] - M[1][1] * M[3][0]));
        Result.M[2][3] = invDet * (M[0][1] * (M[1][3] * M[2][0] - M[1][0] * M[2][3]) - M[0][0] * (M[1][3] * M[2][1] - M[1][1] * M[2][3]) - M[0][3] * (M[1][0] * M[2][1] - M[1][1] * M[2][0]));

        Result.M[3][0] = invDet * (M[1][1] * (M[2][3] * M[3][0] - M[2][0] * M[3][3]) - M[1][0] * (M[2][3] * M[3][1] - M[2][1] * M[3][3]) - M[1][3] * (M[2][1] * M[3][0] - M[2][0] * M[3][1]));
        Result.M[3][1] = invDet * (M[0][0] * (M[2][3] * M[3][1] - M[2][1] * M[3][3]) - M[0][1] * (M[2][3] * M[3][0] - M[2][0] * M[3][3]) + M[0][3] * (M[2][0] * M[3][1] - M[2][1] * M[3][0]));
        Result.M[3][2] = invDet * (M[0][1] * (M[1][3] * M[3][0] - M[1][0] * M[3][3]) - M[0][0] * (M[1][3] * M[3][1] - M[1][1] * M[3][3]) - M[0][3] * (M[1][0] * M[3][1] - M[1][1] * M[3][0]));
        Result.M[3][3] = invDet * (M[0][0] * (M[1][1] * M[2][3] - M[1][3] * M[2][1]) - M[0][1] * (M[1][0] * M[2][3] - M[1][3] * M[2][0]) + M[0][3] * (M[1][0] * M[2][1] - M[1][1] * M[2][0]));

        return Result;
    }

    // 방향 벡터(e.g. RayDirection)에 행렬(InMatrix)의 회전과 스케일만 적용
    static FVector TransformDirection(const FMatrix& InMatrix, const FVector& InDirection)
    {
        // X, Y, Z만 3×3에 해당하는 부분을 곱하고, 위치(translation)은 무시
        float x = (InDirection.X * InMatrix.M[0][0]) +
            (InDirection.Y * InMatrix.M[1][0]) +
            (InDirection.Z * InMatrix.M[2][0]);

        float y = (InDirection.X * InMatrix.M[0][1]) +
            (InDirection.Y * InMatrix.M[1][1]) +
            (InDirection.Z * InMatrix.M[2][1]);

        float z = (InDirection.X * InMatrix.M[0][2]) +
            (InDirection.Y * InMatrix.M[1][2]) +
            (InDirection.Z * InMatrix.M[2][2]);

        return FVector(x, y, z);
    }
};