#include "GizmoComponent.h"
#include "Renderer/URenderer.h"
#include "Types/CommonTypes.h"
#include <Input/InputManager.h>

UGizmoComponent::UGizmoComponent(URenderer* InRenderer)
{
	Renderer = InRenderer;

    CurrentType = EGizmoType::Translation;

    RelativeLocation.X = 1;
    RelativeLocation.Y = 1;
    RelativeLocation.Z = 1;
}

UGizmoComponent::~UGizmoComponent()
{

}

UClass* UGizmoComponent::GetClass()
{
    static UClass GizmoClass("UGizmoComponent", UObject::GetClass());
    return &GizmoClass;
}

UClass* UGizmoComponent::GetInstanceClass() const
{
    return GetClass();
}

void UGizmoComponent::Render(FMatrix WorldMatrix, FMatrix ViewMatrix, FMatrix ProjectionMatrix)
{
    if (FInputManager::GetInst().GetKey(0x31) == EKeyState::Pressed)
    {
        CurrentType = EGizmoType::Translation;
    }

    if (FInputManager::GetInst().GetKey(0x32) == EKeyState::Pressed)
    {
        CurrentType = EGizmoType::Rotation;
    }

    if (FInputManager::GetInst().GetKey(0x33) == EKeyState::Pressed)
    {
        CurrentType = EGizmoType::Scale;
    }

    switch (CurrentType)
    {
        case EGizmoType::Translation:
            
            NumVerticesX = sizeof(translationX_vertices) / sizeof(FVertexType);
            VertexBufferX = Renderer->CreateVertexBuffer(translationX_vertices, sizeof(translationX_vertices));

            NumVerticesY = sizeof(translationY_vertices) / sizeof(FVertexType);
            VertexBufferY = Renderer->CreateVertexBuffer(translationY_vertices, sizeof(translationY_vertices));

            NumVerticesZ = sizeof(translationZ_vertices) / sizeof(FVertexType);
            VertexBufferZ = Renderer->CreateVertexBuffer(translationZ_vertices, sizeof(translationZ_vertices));

            break;
        case EGizmoType::Rotation:
            NumVerticesX = sizeof(rotationX_vertices) / sizeof(FVertexType);
            VertexBufferX = Renderer->CreateVertexBuffer(rotationX_vertices, sizeof(rotationX_vertices));

            NumVerticesY = sizeof(rotationY_vertices) / sizeof(FVertexType);
            VertexBufferY = Renderer->CreateVertexBuffer(rotationY_vertices, sizeof(rotationY_vertices));

            NumVerticesZ = sizeof(rotationZ_vertices) / sizeof(FVertexType);
            VertexBufferZ = Renderer->CreateVertexBuffer(rotationZ_vertices, sizeof(rotationZ_vertices));
            break;
        case EGizmoType::Scale:
            NumVerticesX = sizeof(scaleX_vertices) / sizeof(FVertexType);
            VertexBufferX = Renderer->CreateVertexBuffer(scaleX_vertices, sizeof(scaleX_vertices));

            NumVerticesY = sizeof(scaleY_vertices) / sizeof(FVertexType);
            VertexBufferY = Renderer->CreateVertexBuffer(scaleY_vertices, sizeof(scaleY_vertices));

            NumVerticesZ = sizeof(scaleZ_vertices) / sizeof(FVertexType);
            VertexBufferZ = Renderer->CreateVertexBuffer(scaleZ_vertices, sizeof(scaleZ_vertices));
            break;
        default:

            break;
    }

    FMatrix localMatrix = FMatrix::Identity();

    FMatrix FinalMatrix = localMatrix * WorldMatrix;

    // 셰이더 상수 버퍼 업데이트
    Renderer->UpdateShaderParameters(FinalMatrix, ViewMatrix, ProjectionMatrix);

    Renderer->RenderGizmo(VertexBufferX, NumVerticesX);
    Renderer->RenderGizmo(VertexBufferY, NumVerticesY);
    Renderer->RenderGizmo(VertexBufferZ, NumVerticesZ);
}

void UGizmoComponent::SetGizmoType(EGizmoType NewType)
{
    CurrentType = NewType;
}
