#include "stdafx.h"
#include "UCubeComponent.h"

UCubeComponent::UCubeComponent() {

	vertices = {
		{1.f, 1.f, -1.f, 1.f, 0.f, 0.f, 1.f},   // Vertex 0.
	{ -1.f, 1.f, -1.f, 0.f, 1.f, 0.f, 1.f },  // Vertex 1.
	{-1.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f },    // And so on.
	{1.f, 1.f, 1.f, 1.f, 1.f, 0.f, 1.f},
	{1.f, -1.f, -1.f,1.f, 0.f, 1.f, 1.f},
	{-1.f, -1.f, -1.f, 0.f, 1.f, 1.f, 1.f},
	{-1.f, -1.f, 1.f, 0.f, 0.f, 0.f, 1.f},
	{1.f, -1.f, 1.f, 1.f, 1.f, 1.f, 1.f},
	};

	indices =
	{
	0,1,2,               // Face 0 has three vertices.
	0,2,3,                // And so on.
	0,4,5,
	0,5,1,
	1,5,6,
	1,6,2,
	2,6,7,
	2,7,3,
	3,7,4,
	3,4,0,
	4,7,6,
	4,6,5,
	};


	CGraphics* graphics = CRenderer::Instance()->GetGraphics();
	_vertexBuffer = new CVertexBuffer<FVertexSimple>(graphics->GetDevice());
	_vertexBuffer->Create(vertices);
	_indexBuffer = new CIndexBuffer(graphics->GetDevice());
	_indexBuffer->Create(indices);
}

void UCubeComponent::Update() {
	//RelativeRotation += FVector(1.f, 1.f, 1.f) * Time::GetDeltaTime();
	//RelativeLocation.x = sin(Time::GetElapsedTime());
}

bool UCubeComponent::IntersectsRay(const FVector& rayOrigin, const FVector& rayDir, float& Distance)
{
	// AABB �ּ�, �ִ� ��ǥ ����
	FVector min = { -1.f, -1.f, -1.f };
	FVector max = { 1.f,  1.f,  1.f };

	// t ���� ������ �ʱ�ȭ
	double tMin = -FLT_MAX;
	double tMax = FLT_MAX;
	const double epsilon = FLT_EPSILON;

	if (fabs(rayDir.x) < epsilon) {
		if (rayOrigin.x < min.x || rayOrigin.x > max.x)
			return false;
	}
	else {
		double t1 = (min.x - rayOrigin.x) / rayDir.x;
		double t2 = (max.x - rayOrigin.x) / rayDir.x;
		if (t1 > t2)
			std::swap(t1, t2);
		tMin = (tMin < t1) ? t1 : tMin;
		tMax = (tMax < t2) ? tMax : t2;
	}

	// y�࿡ ���� ���� �׽�Ʈ
	if (fabs(rayDir.y) < epsilon) {
		if (rayOrigin.y < min.y || rayOrigin.y > max.y)
			return false;
	}
	else {
		double t1 = (min.y - rayOrigin.y) / rayDir.y;
		double t2 = (max.y - rayOrigin.y) / rayDir.y;
		if (t1 > t2) std::swap(t1, t2);
		tMin = (tMin < t1) ? t1 : tMin;
		tMax = (tMax < t2) ? tMax : t2;
	}

	if (fabs(rayDir.z) < epsilon) {
		if (rayOrigin.z < min.z || rayOrigin.z > max.z)
			return false;
	}
	else
	{
		double t1 = (min.z - rayOrigin.z) / rayDir.z;
		double t2 = (max.z - rayOrigin.z) / rayDir.z;
		if (t1 > t2) std::swap(t1, t2);
		tMin = (tMin < t1) ? t1 : tMin;
		tMax = (tMax < t2) ? tMax : t2;
	}

	if (tMax >= tMin && tMax >= 0) {
		FVector localIntersection = rayOrigin + rayDir * tMin;
		FVector worldIntersection = GetComponentTransform().TransformCoord(localIntersection);
		Distance = (worldIntersection).MagnitudeSquared();
		return true;
	}

	return false;
}
