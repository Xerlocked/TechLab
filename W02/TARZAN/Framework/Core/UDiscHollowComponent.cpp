#include "stdafx.h"
#include "UDiscHollowComponent.h"

#define DISC_RESOLUTION 128

static const FVector Colors[] = {
    FVector(1.0f, 0.0f, 0.0f),  // X��: ����
    FVector(0.0f, 1.0f, 0.0f),  // Y��: �ʷ�
    FVector(0.0f, 0.0f, 1.0f)   // Z��: �Ķ�
};

UDiscHollowComponent::UDiscHollowComponent(EPrimitiveColor color, float innerRadius) : inner(innerRadius)
{
    inner = innerRadius;
    FVector color3 = Colors[color];

    if (vertices.empty())
    {
        float angleStep = 2.0f * 3.1415926535f / DISC_RESOLUTION;

        // 원의 외곽 버텍스 추가
        for (int i = 0; i <= DISC_RESOLUTION; ++i) {
            float angle = i * angleStep;
            float x = cos(angle);
            float z = sin(angle);
            vertices.push_back({ x, 0.0f, z ,color3.x, color3.y, color3.z, 1 });
            
            x *= innerRadius;
            z *= innerRadius;
            vertices.push_back({ x, 0.0f, z ,color3.x, color3.y, color3.z, 1 });
        }

        // 인덱스 설정 (삼각형 리스트)
        for (int i = 0; i <= DISC_RESOLUTION+1; ++i) {
            int pivot = 2 * i;
            indices.push_back(pivot);      // 중심점
            indices.push_back(pivot + 3);      // 현재 점
            indices.push_back(pivot+2);  // 다음 점 (마지막 점 예외 처리)

            indices.push_back(pivot);      // 중심점
            indices.push_back(pivot + 1);      // 현재 점
            indices.push_back(pivot+3);  // 다음 점 (마지막 점 예외 처리)

        }
    }

    CGraphics* graphics = CRenderer::Instance()->GetGraphics();
    _vertexBuffer = new CVertexBuffer<FVertexSimple>(graphics->GetDevice());
    _vertexBuffer->Create(vertices);
    _indexBuffer = new CIndexBuffer(graphics->GetDevice());
    _indexBuffer->Create(indices);
}

UDiscHollowComponent::UDiscHollowComponent()
{
    inner = 0.5;
    if (vertices.empty())
    {
        float angleStep = 2.0f * 3.1415926535f / DISC_RESOLUTION;

        // 원의 외곽 버텍스 추가
        for (int i = 0; i <= DISC_RESOLUTION; ++i) {
            float angle = i * angleStep;
            float x = cos(angle);
            float z = sin(angle);
            vertices.push_back({ x, 0.0f, z ,1,1,1, 1 });

            x *= 0.5;
            z *= 0.5;
            vertices.push_back({ x, 0.0f, z ,1,1,1, 1 });
        }

        // 인덱스 설정 (삼각형 리스트)
        for (int i = 0; i <= DISC_RESOLUTION + 1; ++i) {
            int pivot = 2 * i;
            indices.push_back(pivot);      // 중심점
            indices.push_back(pivot + 3);      // 현재 점
            indices.push_back(pivot + 2);  // 다음 점 (마지막 점 예외 처리)

            indices.push_back(pivot);      // 중심점
            indices.push_back(pivot + 1);      // 현재 점
            indices.push_back(pivot + 3);  // 다음 점 (마지막 점 예외 처리)

        }
    }

    CGraphics* graphics = CRenderer::Instance()->GetGraphics();
    _vertexBuffer = new CVertexBuffer<FVertexSimple>(graphics->GetDevice());
    _vertexBuffer->Create(vertices);
    _indexBuffer = new CIndexBuffer(graphics->GetDevice());
    _indexBuffer->Create(indices);
}

UDiscHollowComponent::~UDiscHollowComponent()
{
}

//void UDiscHollowComponent::Update()
//{
//    UPrimitiveComponent::Update();
//}

bool UDiscHollowComponent::IntersectsRay(const FVector& rayOrigin, const FVector& rayDir, float& dist)
{
    float denom = rayDir.Dot(this->Front());
    if (std::abs(denom) < FLT_EPSILON) return false;

    auto t = (this->GetComponentLocation() - rayOrigin).Dot(this->Front()) / denom;

    if (t < 0) return false; // Ray와 반대방향

    FVector intersection = rayOrigin + rayDir * t;

    FVector4 intersectionModelSpace4 = FVector4(intersection, 1) * this->GetComponentTransform().Inverse();
    FVector intersectionModelSpace = (intersectionModelSpace4).xyz() / intersectionModelSpace4.w;

    return (inner < intersectionModelSpace.MagnitudeSquared() && intersectionModelSpace.MagnitudeSquared() < 1);
}
