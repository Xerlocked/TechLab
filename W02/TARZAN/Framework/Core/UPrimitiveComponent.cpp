#include "stdafx.h"
#include "UPrimitiveComponent.h"
#include "./Framework/Core/CRenderer.h"

UPrimitiveComponent::~UPrimitiveComponent() {
	if (_vertexBuffer != nullptr)
		delete _vertexBuffer;
	if ( _indexBuffer != nullptr )
		delete _indexBuffer;
}

void UPrimitiveComponent::Render() {
	CGraphics* graphics = CRenderer::Instance()->GetGraphics();
	ID3D11Buffer* vertexBuffer = _vertexBuffer->Get();
	uint32 stride = _vertexBuffer->GetStride();
	uint32 offset = _vertexBuffer->GetOffset();
	graphics->GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	graphics->GetDeviceContext()->IASetIndexBuffer(_indexBuffer->Get(), DXGI_FORMAT_R32_UINT, 0);
	FMatrix m = GetComponentTransform();
	CRenderer::Instance()->SetTransformToConstantBuffer(m);
	CRenderer::Instance()->SetFlagsToConstantBuffer({ renderFlags });
	if (indices.size() > 0)
		graphics->GetDeviceContext()->DrawIndexed(indices.size(), 0, 0);
	else
		graphics->GetDeviceContext()->Draw(vertices.size(), 0);
}


void UPrimitiveComponent::GenerateRayForPicking(const FVector& pickPosition, const FMatrix& viewMatrix, FVector* pickRayOrigin, FVector* rayDirection)
{
	FMatrix xmf4x4WorldView = GetComponentTransform() * viewMatrix;
	FMatrix xmf4x4Inverse = xmf4x4WorldView.Inverse();
	FVector xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	//카메라 좌표계의 원점을 모델 좌표계로 변환한다. 
	*pickRayOrigin = xmf4x4Inverse.TransformCoord(xmf3CameraOrigin);
	//카메라 좌표계의 점(마우스 좌표를 역변환하여 구한 점)을 모델 좌표계로 변환한다. 
	*rayDirection = xmf4x4Inverse.TransformCoord(pickPosition);
	//광선의 방향 벡터를 구한다. 
	*rayDirection = (*rayDirection - *pickRayOrigin).Normalized();
}

int UPrimitiveComponent::PickObjectByRayIntersection(const FVector& pickPosition, const FMatrix& viewMatrix, float* hitDistance)
{
    int inters = 0;
		FVector pickRayOrigin, pickRayDirection;
		GenerateRayForPicking(pickPosition, viewMatrix, &pickRayOrigin, &pickRayDirection);

        inters = CheckRayIntersection(pickRayOrigin,pickRayDirection, hitDistance);

		if (inters)
		{
			OutputDebugString(L"Res :: hit!!!!-----------------\n");
		}
		else
		{
			//OutputDebugString(L"Res :: NOT!!!!-----------------\n");
			//OutputDebugString(L"Ray did not hit the cube\n");
		}
		return inters;
}

bool UPrimitiveComponent::IntersectRayTriangle(const FVector& rayOrigin, const FVector& rayDirection, const FVector& v0, const FVector& v1, const FVector& v2, float& hitDistance)
{
    
        const float epsilon = 1e-6f;
        FVector edge1 = v1 - v0;
        const FVector edge2 = v2 - v0;
        FVector FrayDirection = rayDirection;
        FVector h = FrayDirection.Cross(edge2);
        float a = edge1.Dot(h);

        if (fabs(a) < epsilon)
            return false; // Ray와 삼각형이 평행한 경우

        float f = 1.0f / a;
        FVector s = rayOrigin - v0;
        float u = f * s.Dot(h);
        if (u < 0.0f || u > 1.0f)
            return false;

        FVector q = s.Cross(edge1);
        float v = f * FrayDirection.Dot(q);
        if (v < 0.0f || (u + v) > 1.0f)
            return false;

        float t = f * edge2.Dot(q);
        if (t > epsilon) // 유효한 교차 (ray의 시작점 이후)
        {
            FVector localIntersection = rayOrigin + rayDirection * t;
            FVector worldIntersection = GetComponentTransform().TransformCoord(localIntersection);
            hitDistance = (worldIntersection).MagnitudeSquared();
            return true;
        }

        return false;
    
}

int UPrimitiveComponent::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float* pfNearHitDistance)
{
    int nIntersections = 0;
    if (vertices.empty())
        return 0;

    BYTE* pbPositions = reinterpret_cast<BYTE*>(vertices.data());

    int nPrimitives = indices.empty() ? (vertices.size() / 3) : (indices.size() / 3);

    float fNearHitDistance = FLT_MAX;
    for (int i = 0; i < nPrimitives; i++)
    {
        int idx0, idx1, idx2;
        if (indices.empty())
        {
            idx0 = i * 3;
            idx1 = i * 3 + 1;
            idx2 = i * 3 + 2;
        }
        else
        {
            idx0 = indices[i * 3];
            idx1 = indices[i * 3 + 1];
            idx2 = indices[i * 3 + 2];
        }

        // 각 삼각형의 버텍스 위치를 FVector로 불러옵니다.
        uint32 stride = _vertexBuffer->GetStride();
        FVector v0 = *reinterpret_cast<FVector*>(pbPositions + idx0 * stride);
        FVector v1 = *reinterpret_cast<FVector*>(pbPositions + idx1 * stride);
        FVector v2 = *reinterpret_cast<FVector*>(pbPositions + idx2 * stride);

        float fHitDistance = 0.0f;
        if (IntersectRayTriangle(rayOrigin, rayDirection, v0, v1, v2, fHitDistance)) {
            if (fHitDistance < fNearHitDistance)
                fNearHitDistance = fHitDistance;
            nIntersections++;
        }
    }

    *pfNearHitDistance = fNearHitDistance;
    return nIntersections;
}
