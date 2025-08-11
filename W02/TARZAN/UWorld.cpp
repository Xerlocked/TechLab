#include "stdafx.h"
#include "UWorld.h"
#include "Framework/Core/UGizmoComponent.h"

extern UGizmoComponent* gGizmo;

UWorld::UWorld()
{
    UCameraComponent* camera = SpawnCamera();
    camera->SetRelativeLocation({ 0, 4.0f, -5.0f });
    camera->SetRelativeRotation({ -0.7f, 0, 0 });
    CRenderer::Instance()->SetMainCamera(camera);
    SpawnCoordArrowActor();
}

UWorld::~UWorld()
{
    ClearWorld();
}

void UWorld::Update()
{
    for (auto* comp : actorList)
    {
        auto current = comp;
        if (current) comp->Update();
    }
}

void UWorld::Render()
{
    for (auto* comp : actorList)
    {
        auto current = comp;
        if (current) comp->Render();
    }
}

void UWorld::AddActor(UActorComponent* comp)
{
    actorList.push_back(comp);
}

void UWorld::RemoveActor(UActorComponent* comp)
{
    actorList.remove(comp);
    delete comp;
}

void UWorld::ClearWorld()
{
    gGizmo->Detach();
    UCameraComponent* cam = nullptr;
    while (!actorList.empty())
    {
        UActorComponent* actor = actorList.front();
        UCameraComponent* downcast = dynamic_cast<UCameraComponent*>(actor);
        actorList.pop_front();
        if ( downcast ) {
            cam = downcast;
            continue;
        }
        
        delete actor;
    }
    if (cam)
        AddActor(cam);
}

void UWorld::ConvertNDC_VIEW(int mouse_X, int mouse_Y, FVector& pickPosition, FMatrix& viewMatrix)
{
    UCameraComponent* mainCamera = CRenderer::Instance()->GetMainCamera();
    if (!mainCamera) return;

    D3D11_VIEWPORT viewport = CRenderer::Instance()->GetGraphics()->GetViewport();
    viewMatrix = mainCamera->GetComponentTransform().Inverse();
    FMatrix projectionMatrix = mainCamera->PerspectiveProjection();
    pickPosition.x = ((2.0f * mouse_X / viewport.Width) - 1) / projectionMatrix[0][0];
    pickPosition.y = -((2.0f * mouse_Y / viewport.Height) - 1) / projectionMatrix[1][1];
    pickPosition.z = 1.0f; // Near Plane
}

UActorComponent* UWorld::PickingByRay(int mouse_X, int mouse_Y, float& dist)
{

    FVector pickPosition;
    FMatrix viewMatrix = FMatrix::Identity;
   
    ConvertNDC_VIEW(mouse_X, mouse_Y, pickPosition,viewMatrix);
    
    float hitDistance = FLT_MAX;
    float nearestDistance = FLT_MAX;
    UActorComponent* nearestActorComp = nullptr;
    int intersect = FLT_MIN;
	for (const auto& actorComp : actorList) {
        if (!actorComp) continue;
		int bRes = actorComp->PickObjectByRayIntersection(pickPosition, viewMatrix, &hitDistance);
       if (bRes > intersect && hitDistance < nearestDistance) {
            nearestActorComp = actorComp;
            nearestDistance = hitDistance;
            intersect = bRes;
            UE_LOG((L"\nINTERSECTS::: " + std::to_wstring(bRes)).c_str());
        }
	}
    if (nearestActorComp) {
        UE_LOG((L"\nfind!__" + std::to_wstring(nearestActorComp->GetUUID()) + L" is neareast!! dist: "+ std::to_wstring(nearestDistance)+ L"\n").c_str());
    }
    dist = nearestDistance;
    return nearestActorComp;
}



int UWorld::GetActorCount() const
{
    return actorList.size();
}

const TLinkedList<UActorComponent*>& UWorld::GetActors() const
{
    return actorList;
}


UCameraComponent* UWorld::SpawnCamera()
{
    UCameraComponent* newCamera = SpawnActor<UCameraComponent>(false);
    return newCamera;
}

void UWorld::SetAxisPicked(UArrowComponent* axisX, UArrowComponent* axisY, UArrowComponent* axisZ, EPrimitiveColor pickedAxis)
{
    axisX->SetPicked(pickedAxis == EPrimitiveColor::RED_X);
    axisY->SetPicked(pickedAxis == EPrimitiveColor::GREEN_Y);
    axisZ->SetPicked(pickedAxis == EPrimitiveColor::BLUE_Z);
}

UCubeComponent* UWorld::SpawnCubeActor()
{
    return SpawnActor<UCubeComponent>();
}

USphereComponent* UWorld::SpawnSphereActor()
{
    return SpawnActor<USphereComponent>();
}

UPlaneComponent* UWorld::SpawnPlaneActor()
{
    return SpawnActor<UPlaneComponent>();
}

UCoordArrowComponent* UWorld::SpawnCoordArrowActor()
{
    return SpawnActor<UCoordArrowComponent>(false);
    // return SpawnActor<UCoordArrowComponent>();
}

UDiscComponent* UWorld::SpawnDiscActor()
{
    return SpawnActor<UDiscComponent>();
}

UDiscHollowComponent* UWorld::SpawnDiscHollowActor()
{
    return SpawnActor<UDiscHollowComponent>();
}

void UWorld::SaveWorld(const FString& fileName)
{
    auto actorListCopy = actorList;  // ���纻 ����
    DataManager::Instance()->SaveWorldToJson(this, fileName);
    //DataManager::Instance()->SaveWorldToJson(this, fileName);
}

void UWorld::LoadWorld(const FString& fileName)
{
    ClearWorld();

    TArray<PrimitiveData> primitives = DataManager::Instance()->LoadWorldFromJson(fileName);

    for (const auto& primitive : primitives)
    {
        if (primitive.Type == "Cube")
        {
            UCubeComponent* cube = SpawnCubeActor();
            cube->SetRelativeLocation(primitive.Location);
            cube->SetRelativeRotation(primitive.Rotation);
            cube->SetRelativeScale3D(primitive.Scale);
        }
        else if (primitive.Type == "Sphere")
        {
            USphereComponent* sphere = SpawnSphereActor();
            sphere->SetRelativeLocation(primitive.Location);
            sphere->SetRelativeRotation(primitive.Rotation);
            sphere->SetRelativeScale3D(primitive.Scale);
        }
        else if (primitive.Type == "Plane")
        {
            UPlaneComponent* plane = SpawnPlaneActor();
            plane->SetRelativeLocation(primitive.Location);
            plane->SetRelativeRotation(primitive.Rotation);
            plane->SetRelativeScale3D(primitive.Scale);
        }
    }
}
