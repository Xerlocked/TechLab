#pragma once
#include <PxRigidDynamic.h>
#include <PxVehicleComponents.h>
#include <PxVehicleUpdate.h>

#include "Engine/StaticMeshActor.h"
#include "Core/Delegates/DelegateCombination.h"
#include "UObject/FunctionRegistry.h"

namespace physx
{
    class PxVehicleDrive4W;
    class PxVehicleWheelsSimData;
}

enum FilterGroup
{
    DRIVABLE_SURFACE = (1 << 0), // 주행 가능 표면
    VEHICLE          = (1 << 1)  // 차량 (섀시 및 휠 레이캐스트용)
};

struct MyVehicleQueryFilterCallback : public physx::PxQueryFilterCallback
{
    physx::PxRigidDynamic* vehicleActorToIgnore;

    MyVehicleQueryFilterCallback() : vehicleActorToIgnore(nullptr) {}

    void setVehicleActorToIgnore(physx::PxRigidDynamic* actor) {
        vehicleActorToIgnore = actor;
    }

    virtual physx::PxQueryHitType::Enum preFilter(
        const physx::PxFilterData& queryFilterData, // 레이캐스트의 필터 데이터
        const physx::PxShape* shape,
        const physx::PxRigidActor* actor,
        physx::PxHitFlags& queryFlags) override
    {
        if (actor == vehicleActorToIgnore)
        {
            return physx::PxQueryHitType::eNONE; // 자기 자신이면 무시
        }

        // 그 외에는 일반적인 필터 로직 수행
        // queryFilterData.word1은 레이캐스트가 찾고자 하는 대상의 마스크
        // shape->getQueryFilterData().word0은 현재 검사 대상 셰이프의 타입
        if ((queryFilterData.word1 & shape->getQueryFilterData().word0) != 0)
        {
            return physx::PxQueryHitType::eBLOCK;
        }
        return physx::PxQueryHitType::eNONE;
    }

    virtual physx::PxQueryHitType::Enum postFilter(const physx::PxFilterData& filterData, const physx::PxQueryHit& hit) override
    {
        // 보통 preFilter에서 충분하지만, 필요시 사용
        return physx::PxQueryHitType::eBLOCK; // 또는 hit.flags 기반으로 결정
    }
};

class UStaticMeshComponent;

class ADodge : public AStaticMeshActor
{
    DECLARE_CLASS(ADodge, AStaticMeshActor)
    DECLARE_MULTICAST_DELEGATE(TEST_DELIGATE)
public:
    void CreateDrivablePlane(physx::PxPhysics* gPhysics, physx::PxScene* gScene);
    void SetupVehicle(physx::PxPhysics* gPhysics, physx::PxScene* gScene);
    void SetupSuspensionRaycastBatchQuery(physx::PxPhysics* gPhysics, physx::PxScene* gScene);
    void SetupFrictionPairs();
    void CleanupVehicleSDK();
    void UpdateVehicle(float DeltaTime, const physx::PxScene* PxScene);
    
    ADodge();
    virtual ~ADodge() override = default;
    
    void BeginPlay() override;
    void Tick(float DeltaTime) override;
    
    void Destroyed() override;
    void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    bool Destroy() override;
    void TestTranslate();
    void TestRotate();
    
    void PostDuplicate() override;
    
    void LoadAndConstruct(const TArray<std::unique_ptr<FActorComponentInfo>>& InfoArray) override;
    FActorInfo GetActorInfo() override;
    physx::PxRigidDynamic* createVehicleActor(const physx::PxVehicleChassisData& chassisData,
                                              physx::PxMaterial** wheelMaterials, physx::PxConvexMesh** wheelConvexMeshes,
                                              physx::PxU32 numWheels, const physx::PxFilterData& wheelSimFilterData,
                                              physx::PxMaterial** chassisMaterials,
                                              physx::PxConvexMesh** chassisConvexMeshes, physx::PxU32 numChassisMeshes,
                                              const physx::PxFilterData& chassisSimFilterData, physx::PxPhysics& physics);

    TEST_DELIGATE TestDelegate;

private:
    void SetupWheelsSimulationData(const physx::PxF32 wheelMass, const physx::PxF32 wheelMOI,
    const physx::PxF32 wheelRadius, const physx::PxF32 wheelWidth, const physx::PxF32 numWheels,
    const physx::PxVec3* wheelCenterActorOffsets, const physx::PxVec3& chassisCMOffset,
    const physx::PxF32 chassisMass, physx::PxVehicleWheelsSimData* wheelsSimData);

    float AutoBehaviorTime = 0.0f;
    float ThrottleInput = 0.0f;
    float SteerInput = 0.0f;

    physx::PxRigidDynamic* VehicleActor;

    physx::PxVehicleDrive4W* gVehicle4W = nullptr;
    physx::PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = nullptr;
    physx::PxBatchQuery* gSuspensionRaycastBatchQuery = nullptr;
    physx::PxRaycastQueryResult* gRawRaycastQueryResults = nullptr;
    physx::PxRaycastHit* gRawRaycastHits = nullptr;
    physx::PxVehicleWheelQueryResult gVehicleWheelQueryResults[1];

    physx::PxMaterial* gMaterial = nullptr; // 공용 머티리얼

    MyVehicleQueryFilterCallback mQueryFilterCallback;
};
