#include "stdafx.h"
#include "UDiscComponent.h"

#define DISC_RESOLUTION 64

static const FVector Colors[] = {
    FVector(1.0f, 0.0f, 0.0f),  // X��: ����
    FVector(0.0f, 1.0f, 0.0f),  // Y��: �ʷ�
    FVector(0.0f, 0.0f, 1.0f)   // Z��: �Ķ�
};

UDiscComponent::UDiscComponent(EPrimitiveColor color)
{
    FVector color3 = Colors[color];

    if (vertices.empty())
    {
        vertices.push_back({ 0.0f, 0.0f, 0.0f ,color3.x, color3.x, color3.x, 1 });

        float angleStep = 2.0f * 3.1415926535f / DISC_RESOLUTION;

        // 원의 외곽 버텍스 추가
        for (int i = 0; i <= DISC_RESOLUTION; ++i) {
            float angle = i * angleStep;
            float x = cos(angle);
            float z = sin(angle);
            vertices.push_back({ x, 0.0f, z ,color3.x, color3.y, color3.z, 1});
        }

        // 인덱스 설정 (삼각형 리스트)
        for (int i = 1; i <= DISC_RESOLUTION; ++i) {
            indices.push_back(0);      // 중심점
            indices.push_back(i+1);      // 현재 점
            indices.push_back(i);  // 다음 점 (마지막 점 예외 처리)
        }
    }

    CGraphics* graphics = CRenderer::Instance()->GetGraphics();
    _vertexBuffer = new CVertexBuffer<FVertexSimple>(graphics->GetDevice());
    _vertexBuffer->Create(vertices);
    _indexBuffer = new CIndexBuffer(graphics->GetDevice());
    _indexBuffer->Create(indices);
}

UDiscComponent::UDiscComponent()
{
    if (vertices.empty())
    {
        vertices.push_back({ 0.0f, 0.0f, 0.0f });

        float angleStep = 2.0f * 3.1415926535f / DISC_RESOLUTION;

        // 원의 외곽 버텍스 추가
        for (int i = 0; i <= DISC_RESOLUTION; ++i) {
            float angle = i * angleStep;
            float x = cos(angle);
            float z = sin(angle);
            vertices.push_back({ x, 0.0f, z ,1,1,1,1 });
        }

        // 인덱스 설정 (삼각형 리스트)
        for (int i = 1; i <= DISC_RESOLUTION; ++i) {
            indices.push_back(0);      // 중심점
            indices.push_back(i + 1);      // 현재 점
            indices.push_back(i);  // 다음 점 (마지막 점 예외 처리)
        }
    }

    CGraphics* graphics = CRenderer::Instance()->GetGraphics();
    _vertexBuffer = new CVertexBuffer<FVertexSimple>(graphics->GetDevice());
    _vertexBuffer->Create(vertices);
    _indexBuffer = new CIndexBuffer(graphics->GetDevice());
    _indexBuffer->Create(indices);
}

UDiscComponent::~UDiscComponent()
{
}

//void UDiscComponent::Update()
//{
//}

bool UDiscComponent::IntersectsRay(const FVector& rayOrigin, const FVector& rayDir, float& dist)
{

    float denom = rayDir.Dot(this->Front());
    if (std::abs(denom) < FLT_EPSILON) return false;

    auto t = (this->GetComponentLocation() - rayOrigin).Dot(this->Front()) / denom;

    if (t < 0) return false; // Ray와 반대방향

    FVector intersection = rayOrigin + rayDir * t;

    FVector intersectionModelSpace = (FVector4(intersection, 0) * this->GetAttachParent()->GetComponentTransform().Inverse()).xyz();

    return (intersectionModelSpace.MagnitudeSquared() < 1);


    //FVector disp = intersection - this->GetComponentLocation();

    //FVector dispModelSpace = (FVector4(disp, 0) * this->GetComponentTransform().Inverse()).xyz();

    //float lenModelSpace = dispModelSpace
    //if(disp.MagnitudeSquared() <= )

    // Disc를 포함하는 평면
    //return 1;
}
