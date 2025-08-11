#include "TriangleComponent.h"
#include "Renderer/URenderer.h"

UTriangleComponent::UTriangleComponent()
{
}
UTriangleComponent::UTriangleComponent(URenderer* InRenderer)
{
    Renderer = InRenderer;
    Initialize();
}
UTriangleComponent::UTriangleComponent(URenderer* InRenderer, const FVector& InLocation)
{
    Renderer = InRenderer;
    RelativeLocation = InLocation;
    Initialize();
}
UTriangleComponent::UTriangleComponent(const UTriangleComponent&)
{
}
UTriangleComponent::~UTriangleComponent()
{
}

UClass* UTriangleComponent::GetClass()
{
    static UClass TriangleClass("UTriangleComponent", UObject::GetClass());
    return &TriangleClass;
}

UClass* UTriangleComponent::GetInstanceClass() const
{
    return GetClass();
}

struct triangle3
{
    FVector a;
    FVector b;
    FVector c;
};

bool ray_intersects_triangle(const FVector& ray_origin,
    const FVector& ray_vector,
    const triangle3& triangle,
    FVector& hit_point)
{
    constexpr float epsilon = std::numeric_limits<float>::epsilon();

    // 삼각형의 두 에지 계산
    FVector edge1 = triangle.b - triangle.a;
    FVector edge2 = triangle.c - triangle.a;

    // 광선 벡터와 edge2의 외적
    FVector ray_cross_e2 = ray_vector.Cross(edge2);
    float det = edge1.Dot(ray_cross_e2);

    if (det > -epsilon && det < epsilon)
        return false; // 광선이 삼각형과 평행함

    float inv_det = 1.0f / det;
    FVector s = ray_origin - triangle.a;
    float u = inv_det * s.Dot(ray_cross_e2);

    // u가 [0,1] 범위 내에 있는지 확인
    if (u < 0.0f || u > 1.0f)
        return false;

    FVector s_cross_e1 = s.Cross(edge1);
    float v = inv_det * ray_vector.Dot(s_cross_e1);

    // v가 [0,1] 범위 내에 있고 u+v<=1 인지 확인
    if (v < 0.0f || (u + v) > 1.0f)
        return false;

    // t를 계산하여 광선과 삼각형의 교차점을 구함
    float t = inv_det * edge2.Dot(s_cross_e1);
    if (t > epsilon) // 광선의 앞쪽에서 교차
    {
        hit_point = ray_origin + ray_vector * t;
        return true;
    }
    return false;
}

bool UTriangleComponent::CheckRayIntersection(FVector RayOrigin, FVector RayDirection, FHitResult& OutHitResult)
{
    // 로컬 공간의 삼각형 정점들 (중심이 0,0,0)
    FVector v0_local = FVector(0.0f, 1.0f, 0.0f);
    FVector v1_local = FVector(1.0f, -1.0f, 0.0f);
    FVector v2_local = FVector(-1.0f, -1.0f, 0.0f);

    // 모델 변환 행렬 생성 (스케일 -> 회전 -> 이동 순서)
    FMatrix Scaling = FMatrix::Scaling(RelativeScale3D.X, RelativeScale3D.Y, RelativeScale3D.Z);
    FMatrix Rotation = FMatrix::CreateRotationRollPitchYaw(RelativeRotation.X, RelativeRotation.Y, RelativeRotation.Z);
    FMatrix Translation = FMatrix::Translation(RelativeLocation.X, RelativeLocation.Y, RelativeLocation.Z);
    FMatrix ModelMatrix = Scaling * Rotation * Translation;

    // 월드 공간으로 변환된 삼각형 정점들
    FVector v0_world = ModelMatrix * v0_local;
    FVector v1_world = ModelMatrix * v1_local;
    FVector v2_world = ModelMatrix * v2_local;

    triangle3 tri;
    tri.a = v0_world;
    tri.b = v1_world;
    tri.c = v2_world;

    // 교차 검사를 수행합니다.
    FVector hitPoint;
    if (ray_intersects_triangle(RayOrigin, RayDirection, tri, hitPoint))
    {
        OutHitResult.bHit = true;
        OutHitResult.HitLocation = hitPoint;
        OutHitResult.Distance = (hitPoint - RayOrigin).Length();
        OutHitResult.HitObject = this;
        return true;
    }
    else
    {
        return false;
    }
}

void UTriangleComponent::Initialize()
{
    NumVertices = sizeof(triangle_vertices) / sizeof(FVertexType);
    VertexBuffer = Renderer->CreateVertexBuffer(triangle_vertices, sizeof(triangle_vertices));
}